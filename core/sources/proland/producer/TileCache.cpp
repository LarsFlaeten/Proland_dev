/*
 * Proland: a procedural landscape rendering library.
 * Website : http://proland.inrialpes.fr/
 * Copyright (c) 2008-2015 INRIA - LJK (CNRS - Grenoble University)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors 
 * may be used to endorse or promote products derived from this software without 
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Proland is distributed under the Berkeley Software Distribution 3 Licence. 
 * For any assistance, feedback and enquiries about training programs, you can check out the 
 * contact page on our website : 
 * http://proland.inrialpes.fr/
 */
/*
 * Main authors: Eric Bruneton, Antoine Begault, Guillaume Piolat.
 */

#include "proland/producer/TileCache.h"

#include <sstream>

#include "ork/core/Logger.h"
#include "ork/resource/ResourceTemplate.h"
#include "proland/producer/TileProducer.h"

#include <pthread.h>

using namespace std;
using namespace ork;

namespace proland
{

TileCache::Tile::Tile(int producerId, int level, int tx, int ty, ptr<Task> task, TileStorage::Slot *data) :
    producerId(producerId), level(level), tx(tx), ty(ty), task(task), data(data), users(0)
{
    assert(data != NULL);
}

TileCache::Tile::~Tile()
{
}

TileStorage::Slot *TileCache::Tile::getData(bool check)
{
    bool isDone = task->isDone();
    assert(isDone || !check);
    assert(getTId() == data->id || !check);
    return isDone ? data : NULL;
}

TileCache::Tile::Id TileCache::Tile::getId() const
{
    return getId(level, tx, ty);
}

TileCache::Tile::TId TileCache::Tile::getTId() const
{
    return getTId(producerId, level, tx, ty);
}


TileCache::Tile::Id TileCache::Tile::getId(int level, int tx, int ty)
{
    return make_pair(level, make_pair(tx, ty));
}

TileCache::Tile::TId TileCache::Tile::getTId(int producerId, int level, int tx, int ty)
{
    return make_pair(producerId, make_pair(level, make_pair(tx, ty)));
}

TileCache::TileCache(ptr<TileStorage> storage,  std::string name, ptr<Scheduler> scheduler) : Object("TileCache")
{
    init(storage, name, scheduler);
}

TileCache::TileCache() : Object("TileCache")
{
}

void TileCache::init(ptr<TileStorage> storage, std::string name, ptr<Scheduler> scheduler)
{
    this->nextProducerId = 0;
    this->storage = storage;
    this->scheduler = scheduler;
    this->queries = 0;
    this->misses = 0;
    this->name = name;
    mutex = new pthread_mutex_t;
    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t*) mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);
}

TileCache::~TileCache()
{
    pthread_mutex_destroy((pthread_mutex_t*) mutex);
    delete (pthread_mutex_t*) mutex;
    mutex = NULL;
    // The users of a TileCache must release all their tiles with putTile
    // before they erase their reference to the TileCache. Hence a TileCache
    // cannot be deleted before all tiles are unused. So usedTiles should be
    // empty at this point
    assert(usedTiles.size() == 0);
    unusedTiles.clear();
    // releases the storage used by the unused tiles
    list<Tile*>::iterator i = unusedTilesOrder.begin();
    while (i != unusedTilesOrder.end()) {
        storage->deleteSlot((*i)->data);
        delete *i;
        ++i;
    }
    unusedTilesOrder.clear();
    deletedTiles.clear();
}

ptr<TileStorage> TileCache::getStorage()
{
    return storage;
}

ptr<Scheduler> TileCache::getScheduler()
{
    return scheduler;
}

int TileCache::getUsedTiles()
{
    return usedTiles.size();
}

int TileCache::getUnusedTiles()
{
    return unusedTiles.size();
}

TileCache::Tile* TileCache::findTile(int producerId, int level, int tx, int ty, bool includeCache)
{
    assert(producers.find(producerId) != producers.end());
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    Tile::TId id = Tile::getTId(producerId, level, tx, ty);
    map<Tile::TId, Tile*>::iterator i = usedTiles.find(id);
    Tile *t = NULL;
    // looks for the requested tile in the used tiles list
    if (i != usedTiles.end()) {
        t = i->second;
        assert(t->producerId == producerId && t->level == level && t->tx == tx && t->ty == ty);
    }
    // looks for the requested tile in the unused tiles list (if includeCache is true)
    if (t == NULL && includeCache) {
        Cache::iterator i = unusedTiles.find(id);
        if (i != unusedTiles.end()) {
            t = *(i->second);
            assert(t->producerId == producerId && t->level == level && t->tx == tx && t->ty == ty);
        }
    }
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
    return t;
}

TileCache::Tile* TileCache::getTile(int producerId, int level, int tx, int ty, unsigned int deadline, int *users)
{
    assert(producers.find(producerId) != producers.end());
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    Tile::TId id = Tile::getTId(producerId, level, tx, ty);
    map<Tile::TId, Tile*>::iterator i = usedTiles.find(id);
    Tile *t;
    if (i == usedTiles.end()) {
        bool deletedTile = false;
        ++queries;
        Cache::iterator i = unusedTiles.find(id);
        if (i == unusedTiles.end()) {
            // the requested tile is not in storage, it must be created
            TileStorage::Slot *data = storage->newSlot();
            if (data == NULL && unusedTiles.size() > 0) {
                // evict least recently used tile to reuse its data storage
                list<Tile*>::iterator li = unusedTilesOrder.begin();
                t = *li;
                data = t->data;
                unusedTiles.erase(t->getTId());
                unusedTilesOrder.erase(li);
                deletedTiles.insert(make_pair(t->getTId(), t->task.get()));
                delete t;
            }
            if (data == NULL) { // cache is full
                t = NULL;
            } else {
                ++misses;
                ptr<Task> task;
                map<Tile::TId, Task*>::iterator i = deletedTiles.find(id);
                if (i != deletedTiles.end()) {
                    // if the task for creating this tile still exists, we reuse it
                    task = i->second;
                    deletedTile = true;
                    deletedTiles.erase(i);
                }
                task = producers[producerId]->createTile(level, tx, ty, data, deadline, task);
                // creates the requested tile
                t = new Tile(producerId, level, tx, ty, task, data);
            }
        } else {
            // requested tile found in unused tile list
            list<Tile*>::iterator li = i->second;
            t = *li;
            unusedTiles.erase(i);
            unusedTilesOrder.erase(li);
        }
        if (t != NULL) {
            // marks requested tile as used
            usedTiles.insert(make_pair(id, t));
            if (deletedTile) {
                // if the tile data was not in storage and if the task to create it
                // was reused from a deleted tile, we need to reexecute the task
                // to recreate the tile data
                if (scheduler == NULL) {
                    t->task->setIsDone(false, 0, Task::DATA_NEEDED);
                } else {
                    scheduler->reschedule(t->task, Task::DATA_NEEDED, deadline);
                }
            }
        }
        if (Logger::DEBUG_LOGGER != NULL) {
            Logger::DEBUG_LOGGER->logf("CACHE", "%s: tiles: %d used, %d reusable, total %d", name.c_str(), usedTiles.size(), unusedTiles.size(), storage->getCapacity());
//            Logger::DEBUG_LOGGER->logf("CACHE", "%s: queries: %d misses for %d queries", name.c_str(), misses, queries);
        }
    } else {
        // requested tile found in used tiles list -> nothing to do
        t = i->second;
        assert(t->producerId == producerId && t->level == level && t->tx == tx && t->ty == ty);
    }
    if (t != NULL) {
        if (users != NULL) {
            *users = t->users;
        }
        t->users += 1;
    }
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
    return t;
}

ptr<Task> TileCache::prefetchTile(int producerId, int level, int tx, int ty)
{
    assert(producers.find(producerId) != producers.end());
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    Tile::TId id = Tile::getTId(producerId, level, tx, ty);
    ptr<Task> task;
    if (usedTiles.find(id) == usedTiles.end()) {
        if (unusedTiles.find(id) == unusedTiles.end()) {
            // the requested tile is not in storage, it must be created
            TileStorage::Slot *data = storage->newSlot();
            if (data == NULL && unusedTiles.size() > 0) {
                // evict least recently used tile to reuse its data storage
                list<Tile*>::iterator li = unusedTilesOrder.begin();
                Tile *t = *li;
                data = t->data;
                assert(data != NULL);
                unusedTiles.erase(t->getTId());
                unusedTilesOrder.erase(li);
                deletedTiles.insert(make_pair(t->getTId(), t->task.get()));
                delete t;
            }
            if (data != NULL) {
                unsigned int deadline = 1u << 31u;
                bool deletedTile = false;
                map<Tile::TId, Task*>::iterator i = deletedTiles.find(id);
                if (i != deletedTiles.end()) {
                    // if the task for creating this tile still exists, we reuse it
                    task = i->second;
                    deletedTile = true;
                    deletedTiles.erase(i);
                }
                task = producers[producerId]->createTile(level, tx, ty, data, deadline, task);
                // creates the requested tile
                Tile *t = new Tile(producerId, level, tx, ty, task, data);
                list<Tile*>::iterator li = unusedTilesOrder.insert(unusedTilesOrder.end(), t);
                unusedTiles[id] = li;
                if (deletedTile) {
                    // if the tile data was not in storage and if the task to create it
                    // was reused from a deleted tile, we need to reexecute the task
                    // to recreate the tile data
                    if (scheduler == NULL) {
                        task->setIsDone(false, 0, Task::DATA_NEEDED);
                    } else {
                        scheduler->reschedule(task, Task::DATA_NEEDED, deadline);
                    }
                }
                /*if (Logger::DEBUG_LOGGER != NULL) {
                    ostringstream oss;
                    oss << "tiles: " << usedTiles.size() << " used, " << unusedTiles.size() << " reusable";
                    Logger::DEBUG_LOGGER->log("CACHE", oss.str());
                }*/
            }
        }
    }
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
    return task;
}

int TileCache::putTile(Tile *t)
{
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    t->users -= 1;
    if (t->users == 0) {
        // the tile is now unused
        Tile::TId id = t->getTId();
        // removes it from the used tiles list
        map<Tile::TId, Tile*>::iterator i = usedTiles.find(id);
        assert(i != usedTiles.end() && i->second == t);
        usedTiles.erase(i);
        // adds it to the unused tiles list
        assert(unusedTiles.find(id) == unusedTiles.end());
        list<Tile*>::iterator li = unusedTilesOrder.insert(unusedTilesOrder.end(), t);
        unusedTiles[id] = li;
        /*if (Logger::DEBUG_LOGGER != NULL) {
            ostringstream oss;
            oss << "tiles: " << usedTiles.size() << " used, " << unusedTiles.size() << " reusable";
            Logger::DEBUG_LOGGER->log("CACHE", oss.str());
        }*/
    }
    int users = t->users;
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
    return users;
}

void TileCache::invalidateTiles(int producerId)
{
    // marks the tasks to produce the tiles of the given producer as not done
    // so that they will be reexecuted when their result will be needed
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    map<Tile::TId, Tile*>::iterator i = usedTiles.begin();
    while (i != usedTiles.end()) {
        if (i->second->producerId == producerId) {
            if (scheduler == NULL) {
                i->second->task->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule(i->second->task, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        i++;
    }
    list<Tile*>::iterator j = unusedTilesOrder.begin();;
    while (j != unusedTilesOrder.end()) {
        if ((*j)->producerId == producerId) {
            if (scheduler == NULL) {
                (*j)->task->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule((*j)->task, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        j++;
    }
    map<Tile::TId, Task*>::iterator k = deletedTiles.begin();
    while (k != deletedTiles.end()) {
        if (k->first.first == producerId) {
            if (scheduler == NULL) {
                k->second->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule(k->second, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        k++;
    }
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

void TileCache::invalidateTile(int producerId, int level, int tx, int ty)
{
    Tile::TId id = TileCache::Tile::getTId(producerId, level, tx, ty);

    pthread_mutex_lock((pthread_mutex_t*) mutex);
    map<Tile::TId, Tile*>::iterator i = usedTiles.begin();
    while (i != usedTiles.end()) {
        if (i->first == id) {
            if (scheduler == NULL) {
                i->second->task->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule(i->second->task, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        i++;
    }

    list<Tile*>::iterator j = unusedTilesOrder.begin();;
    while (j != unusedTilesOrder.end()) {
        if ((*j)->getTId() == id) {
            if (scheduler == NULL) {
                (*j)->task->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule((*j)->task, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        j++;
    }
    map<Tile::TId, Task*>::iterator k = deletedTiles.begin();
    while (k != deletedTiles.end()) {
        if (k->first == id) {
            if (scheduler == NULL) {
                k->second->setIsDone(false, 0, Task::DATA_CHANGED);
            } else {
                scheduler->reschedule(k->second, Task::DATA_CHANGED, 1u << 31u);
            }
        }
        k++;
    }
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

void TileCache::swap(ptr<TileCache> t)
{
}

void TileCache::createTileTaskDeleted(int producerId, int level, int tx, int ty)
{
    Tile::TId id = Tile::getTId(producerId, level, tx, ty);
    assert(mutex != NULL);
    pthread_mutex_lock((pthread_mutex_t*) mutex);
    map<Tile::TId, Task*>::iterator i = deletedTiles.find(id);
    assert(i != deletedTiles.end());
    deletedTiles.erase(i);
    pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

class TileCacheResource : public ResourceTemplate<1, TileCache>
{
public:
    TileCacheResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<1, TileCache>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        ptr<TileStorage> storage;
        ptr<Scheduler> scheduler;
        checkParameters(desc, e, "name,storage,scheduler,");
        if (e->Attribute("storage") != NULL) {
            string id = getParameter(desc, e, "storage");
            storage = manager->loadResource(id).cast<TileStorage>();
        } else {
            const TiXmlNode *n = e->FirstChild();
            if (n == NULL) {
                if (Logger::ERROR_LOGGER != NULL) {
                    Resource::log(Logger::ERROR_LOGGER, desc, e, "Missing storage attribute or subelement");
                }
                throw exception();
            }
            const TiXmlElement *f = n->ToElement();
            storage = ResourceFactory::getInstance()->create(manager, f->Value(), desc, f).cast<TileStorage>();
        }
        string id = getParameter(desc, e, "scheduler");
        scheduler = manager->loadResource(id).cast<Scheduler>();
        init(storage, name, scheduler);
    }
};

extern const char tileCache[] = "tileCache";

static ResourceFactory::Type<tileCache, TileCacheResource> TileCacheType;

}
