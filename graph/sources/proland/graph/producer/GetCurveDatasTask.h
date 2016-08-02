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

#ifndef _PROLAND_GETCURVESDATATASK_H
#define _PROLAND_GETCURVESDATATASK_H

#include "proland/producer/ObjectTileStorage.h"
#include "proland/graph/producer/GraphProducer.h"
#include "proland/graph/producer/CurveDataFactory.h"

namespace proland
{

/**
 * GetCurveDatasTask is used for Layers that need "random" Tiles
 * produced from an other Producer.
 *
 * For example, if it requires all the data about a given Curve,
 * it will need every tiles crossed by this Curve.
 * It may also be used to prefetch CurveData (call to
 * CurveDataFactory#getCurveData()). Prefetched CurveDatas
 * will have to be accessed using the CurveDataFactory#findCurveData()
 * method, and released via the CurveDataFactory#putCurveData() method
 * (The release should be automatic).
 * @ingroup producer
 * @author Antoine Begault
 */
template <class OWNER>
class GetCurveDatasTask : public Task
{
public:
    /**
     * The task that created this AnalyseTile.
     */
    Task *task;

    /**
     * The parent TaskGraph of task.
     */
    TaskGraph *parent;

    /**
     * The layer that requires this AnalyseTile.
     */
     OWNER *owner;

    /**
     * The GraphProducer for which we need the data.
     */
    GraphProducer *graphProducer;

    /**
     * The TileProducer required for owner's tile creation.
     */
    TileProducer *target;

    /**
     * The factory that creates the CurveDatas.
     */
    CurveDataFactory *factory;

    /**
     * The level of the created Tile.
     */
    int level;

    /**
     * The x coordinate of the created Tile.
     */
    int tx;

    /**
     * The y coordinate of the created Tile.
     */
    int ty;

    /**
     * Creates a new GetCurveDatasTask.
     * GetCurveDatasTask is used for Layers that need "random" Tiles produced from an other Producer.
     * For example, if it requires all the data about a given Curve, it will need every tiles crossed by this Curve.
     *
     * @param task the task that created this GetCurveDatasTask.
     * @param parent the parent TaskGraph of task.
     * @param owner the layer that requires this GetCurveDatasTask.
     * @param graphProducer the GraphProducer for which we need the data.
     * @param target the TileProducer required for owner's tile creation.
     * @param factory the factory that creates the CurveDatas.
     * @param level the level of the created Tile.
     * @param tx the x coordinate of the created Tile.
     * @param ty the y coordinate of the created Tile.
     */
     GetCurveDatasTask(Task* task, TaskGraph *parent, OWNER *owner, GraphProducer *graphProducer, TileProducer *target, CurveDataFactory *factory, int level, int tx, int ty, unsigned deadline) :
        Task("GetCurveDatasTask", false, deadline), task(task), parent(parent), owner(owner), graphProducer(graphProducer), target(target), factory(factory), level(level), tx(tx), ty(ty)
    {
    }

    virtual ~GetCurveDatasTask()
    {
    }

    virtual bool run()
    {
        if (Logger::DEBUG_LOGGER != NULL) {
            ostringstream oss;
            oss << "GetCurveDatasTask " << level << " " << tx << " " << ty;
            Logger::DEBUG_LOGGER->log("GRAPH", oss.str());
        }
        bool changes = true;
        assert(!isDone());
        // Getting the Graph that should have been already computed.
        TileCache::Tile *t = graphProducer->findTile(level, tx, ty);
        assert(t != NULL);
        ObjectTileStorage::ObjectSlot *graphData = dynamic_cast<ObjectTileStorage::ObjectSlot*>(t->getData());
        GraphPtr g = graphData->data.cast<Graph>();

        // Now browsing the entire list of curves in order to get the tiles containing them.
        ptr<Graph::CurveIterator> ci = g->getCurves();

        set<TileCache::Tile::Id> tileIds;
        set<TileCache::Tile *> usedTiles;
        set<CurveId> usedDatas;
        vec3d q = owner->getTileCoords(level, tx, ty);
        float scale = 2.0f * (owner->getTileSize() - 1.0f - (2.0f * owner->getTileBorder())) / q.z;
        float tileSize = 0.0f;

        if (target != NULL) {
            tileSize = target->getRootQuadSize() / (target->getCache()->getStorage()->getTileSize() - target->getBorder() * 2.0f - 1.0f);
        }
        while (ci->hasNext()) {
            CurvePtr c = ci->next();
            if (c->getWidth() * scale < 1) {
                continue;
            }
            CurveData *cd = factory->getCurveData(c);
            usedDatas.insert(cd->getId());
            if (target != NULL) {
                cd->getUsedTiles(tileIds, tileSize); // Returns the list of usedTiles
            }
        }
        unsigned int d = getDeadline();
        if (target != NULL) {
            ptr<Scheduler> scheduler = target->getCache()->getScheduler();
            int n = 0;
            for (set<TileCache::Tile::Id>::iterator i = tileIds.begin(); i != tileIds.end(); i++) {
                TileCache::Tile* t = target->getTile(i->first, i->second.first, i->second.second, d);
                ptr<Task> tt = t->task;
                assert(t != NULL);
                if (!tt->isDone()) {
                    parent->addTask(tt);
                    parent->addDependency(task, tt);
                    ++n;
                }
                usedTiles.insert(t);
            }
            if (n > 0) {
                scheduler->schedule(parent);
            }
            owner->addUsedTiles(level, tx, ty, target, usedTiles);
        }
        factory->addUsedCurveDatas(level, tx, ty, usedDatas);
        return changes;
    }

};

}

#endif
