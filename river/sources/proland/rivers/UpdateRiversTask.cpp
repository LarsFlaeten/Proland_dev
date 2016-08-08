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

#include "proland/rivers/UpdateRiversTask.h"

#include "ork/core/Logger.h"
#include "ork/resource/ResourceTemplate.h"

#include "proland/producer/ObjectTileStorage.h"
#include "proland/graph/producer/GetCurveDatasTask.h"
#include "proland/particles/screen/ScreenParticleLayer.h"

using namespace ork;

namespace proland
{

bool UpdateRiversTask::TileInfo::operator==(const TileInfo t) {
    return terrainId == t.terrainId && lp == t.lp && fp == t.fp;
}

UpdateRiversTask::TileInfo::TileInfo()
{
}

UpdateRiversTask::TileInfo::TileInfo(int terrainId, vec3i lp, vec3f fp) :
    terrainId(terrainId), lp(lp), fp(fp)
{
}

UpdateRiversTask::TileInfo::~TileInfo()
{
}

UpdateRiversTask::UpdateRiversTask() : AbstractTask("UpdateRiversTask")
{
}

UpdateRiversTask::UpdateRiversTask(ptr<ParticleProducer> particles, float timeStep) :
    AbstractTask("UpdateRiversTask")
{
    init(particles, timeStep);
}

void UpdateRiversTask::init(ptr<ParticleProducer> particles, float timeStep)
{
    this->particles = particles;
    this->terrainLayer = particles->getLayer<TerrainParticleLayer>();
    assert(terrainLayer != NULL);
    this->timeStep = timeStep;

    this->initialized = false;
}

UpdateRiversTask::~UpdateRiversTask()
{
    for (TileInfos::iterator i = previousFrameTiles.begin(); i != previousFrameTiles.end(); i++) {
        vec3i v = (*i).second->lp;
        ptr<TileProducer> flows = terrainInfos[(*i).first.first].flows;
        TileCache::Tile *t = flows->findTile(v.x, v.y, v.z);
        assert(t != NULL);
        flows->putTile(t);

        delete (*i).second;
    }

    for (TileInfos::iterator i = tileInfos.begin(); i != tileInfos.end(); i++) {
        vec3i v = (*i).second->lp;
        ptr<TileProducer> flows = terrainInfos[(*i).first.first].flows;
        TileCache::Tile *t = flows->findTile(v.x, v.y, v.z);
        assert(t != NULL);
        flows->putTile(t);


        delete (*i).second;
    }

    for (int i = 0; i < (int) terrainInfos.size(); i++) {
        TileCache::Tile *t = terrainInfos[i].flows->findTile(0, 0, 0);
        assert(t != NULL);
        terrainInfos[i].flows->putTile(t);
    }
    tileInfos.clear();
    previousFrameTiles.clear();
    terrainInfos.clear();
}

void UpdateRiversTask::getTilesToUpdate(TerrainInfo &terrain, ptr<TerrainQuad> q)
{
    if (q->visible == SceneManager::INVISIBLE) {
        return;
    }

    if (q->isLeaf() == false) {
        getTilesToUpdate(terrain, q->children[0]);
        getTilesToUpdate(terrain, q->children[1]);
        getTilesToUpdate(terrain, q->children[2]);
        getTilesToUpdate(terrain, q->children[3]);
        return;
    }

    vec3i v = vec3i(q->level, q->tx, q->ty);
    vec3f f = vec3f(q->ox, q->oy, q->l);
    terrain.displayedTiles.push_back(make_pair(v, f));
}

//usedTiles = vector<pair<terrainId, vec3i>>
//TileInfos = map<vec3i, TileInfo>
//foreach usedTiles
//  if tileInfos.find(usedTile)
//     do nothing
//  else
//     create new TileInfo ti
//     getTile(ti.flow)
//  add new GetCurveDataTask<TileInfo>(ti) //or updateRiversTask
//foreach unused Tiles ti
//  TerrainInfos[ti.terrainId].flows->putTile(ti) //done in the next Frame
ptr<Task> UpdateRiversTask::getTask(ptr<Object> context)
{
    ptr<Impl> impl = new Impl(this);
    ptr<TaskGraph> result = new TaskGraph(impl);

    TileInfos::iterator it;
    TileInfo *info;
    vec3i v;
    TileCache::Tile::Id id;
    TileCache::Tile::TId p;

    // Releasing previous frame TileInfos. This is done in the next frame so that CurveDatas don't get deleted if it is unnecessary (i.e. if they are still used).
    for (it = previousFrameTiles.begin(); it != previousFrameTiles.end(); it++) { //Delete every TileInfo that is no longer visible
        info = it->second;
        TerrainInfo t = terrainInfos[info->terrainId];
        TileCache::Tile *tile = t.flows->findTile(info->lp.x, info->lp.y, info->lp.z); //release Flow Tiles
        assert(tile != NULL);
        t.flows->putTile(tile);

        tileInfos.erase(it->first);
        delete it->second; //Delete TileInfo*
    }
    previousFrameTiles.clear();
    previousFrameTiles = tileInfos;

    if (! initialized) {
        map<ptr<TileProducer>, TerrainParticleLayer::TerrainInfo*> infos = terrainLayer->getTerrainInfos();
        for (map<ptr<TileProducer>, TerrainParticleLayer::TerrainInfo*>::iterator i = infos.begin(); i != infos.end(); i++) {
            ptr<SceneNode> tn = i->second->node;

            TerrainInfo ti;
            ti.id = terrainInfos.size();
            ti.tn = tn;
            ti.t = i->second->terrain;

            ti.flows = i->first;
            ti.flows->setRootQuadSize(ti.t->root->l);

            TileCache::Tile *tile = ti.flows->getTile(0, 0, 0, 0);//for getCurveDataTask + correct flow tile lookup
            result->addTask(tile->task);
            result->addDependency(impl, tile->task);

            terrainInfos.push_back(ti);
        }
        initialized = true;
    }

    for (int i = 0; i < (int) terrainInfos.size(); i++) { //Getting the list of tiles to display in the current frame for each terrain
        terrainInfos[i].displayedTiles.clear();
        getTilesToUpdate(terrainInfos[i], terrainInfos[i].t->root);
    }

    for (int j = 0; j < (int) terrainInfos.size(); j++) { //foreach terrain
        TerrainInfo ti = terrainInfos[j];
        for (int i = 0; i < (int) ti.displayedTiles.size(); i++) { //foreach visible tile
            v = ti.displayedTiles[i].first;
            id = TileCache::Tile::getId(v.x, v.y, v.z);
            p = TileCache::Tile::getTId(ti.id, v.x, v.y, v.z);
            it = tileInfos.find(p); //check if it was already visible before
            if (it == tileInfos.end()) { // if not, create TileInfo
                info = new TileInfo(ti.id, v, ti.displayedTiles[i].second);
                tileInfos.insert(make_pair(p, info));

                TileCache::Tile* flowTile = ti.flows->getTile(v.x, v.y, v.z, 0); //WARNING !! can be changed or invalidated
                assert(flowTile != NULL);
                result->addTask(flowTile->task);
                result->addDependency(impl, flowTile->task);

            } else { // else remove it from the temporary list and check if they are ready for use
                previousFrameTiles.erase(p);
                info = it->second;


                TileCache::Tile *flowTile = ti.flows->findTile(v.x, v.y, v.z);
                assert(flowTile != NULL);
                if (flowTile->task->isDone() == false) { //checking HydroFlowFactory
                    ti.flows->putTile(flowTile);
                    flowTile = ti.flows->getTile(v.x, v.y, v.z, 0);
                    result->addTask(flowTile->task);
                    result->addDependency(impl, flowTile->task);
                }
            }
        }
    }

    return result;
}

void UpdateRiversTask::updateRivers()
{
    if (Logger::DEBUG_LOGGER != NULL) {
        Logger::DEBUG_LOGGER->log("RIVERS", "Updating Rivers");
    }
}

UpdateRiversTask::Impl::Impl(ptr<UpdateRiversTask> owner) :
    Task("DrawRivers", true, 0), owner(owner)
{
}

UpdateRiversTask::Impl::~Impl()
{

}

bool UpdateRiversTask::Impl::run()
{
    owner->updateRivers();
    return true;
}

void UpdateRiversTask::swap(ptr<UpdateRiversTask> t)
{
    std::swap(particles, t->particles);
    std::swap(terrainInfos, t->terrainInfos);
    std::swap(tileInfos, t->tileInfos);
    std::swap(initialized, t->initialized);
}

class UpdateRiversTaskResource : public ResourceTemplate<50, UpdateRiversTask>
{
public:
    UpdateRiversTaskResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, UpdateRiversTask>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        float timeStep = 1.0f;
        checkParameters(desc, e, "name,particles,timeStep,");

        if (e->Attribute("timeStep") != NULL) {
            getFloatParameter(desc, e, "timeStep", &timeStep);
        }

        ptr<ParticleProducer> particles = manager->loadResource(getParameter(desc, e, "particles")).cast<ParticleProducer>();
        assert(particles != NULL);

        init(particles, timeStep);
    }
};

extern const char updateRivers[] = "updateRivers";

static ResourceFactory::Type<updateRivers, UpdateRiversTaskResource> UpdateRiversTaskType;

}
