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

#ifndef _PROLAND_UPDATERIVERSTASK_H_
#define _PROLAND_UPDATERIVERSTASK_H_

#include "ork/scenegraph/AbstractTask.h"

#include "proland/rivers/HydroFlowProducer.h"
#include "proland/terrain/TileSampler.h"
#include "proland/particles/ParticleProducer.h"
#include "proland/particles/terrain/TerrainParticleLayer.h"

using namespace ork;

namespace proland
{

/**
 * This class is used to draw multi-resolution Animated Rivers as described in
 * Qizhi Yu's Thesis [Models of Animated Rivers for the Interactive Exploration
 * of Landscapes - November 2008].
 * See http://www-evasion.imag.fr/Membres/Qizhi.Yu/phd/ for more details about this work.
 * It represents Rivers on 3 different scales :
 * - Macro scale : Overall visual impression of rivers.
 * - Meso scale  : Local waves and effects applied to the river, such as quasi-stationary
 * waves caused by an obstacle, hydraulic jump caused by bed topography change, surface boils...
 * - Micro scale : Small waves on river surface, which conveys the flow motion of the river.
 * @ingroup rivers
 * @author Antoine Begault
 */
PROLAND_API class UpdateRiversTask : public AbstractTask
{
public:
    /**
     * Creates a new UpdateRiversTask.
     *
     * @param particles a ParticleProducer.
     * @param timeStep time step at each frame. Changes the speed of the river.
     */
    UpdateRiversTask(ptr<ParticleProducer> particles, float timeStep = 1.0f);

    /**
     * Deletes this UpdateRiversTask.
     */
    virtual ~UpdateRiversTask();

    /**
     * Returns the task(s) to be executed for this object.
     * It checks which tiles ParticleProducer needs to produce, depending
     * on the current view, and puts it in the returned TaskGraph. The
     * corresponding ParticleManagers will be produced <i>before</i> the
     * #run() method call.
     *
     * @param context see Method.
     */
    virtual ptr<Task> getTask(ptr<Object> context);

protected:
    /**
     * Creates a new UpdateRiversTask.
     */
    UpdateRiversTask();

    /**
     * Initializes UpdateRiversTask fields.
     *
     * @param particles a ParticleProducer.
     * @param timeStep time step at each frame. Changes the speed of the river.
     */
    void init(ptr<ParticleProducer> particles, float timeStep = 1.0f);

    void swap(ptr<UpdateRiversTask> t);

private:
    class Impl : public Task
    {
    public:
        ptr<UpdateRiversTask> owner;

        Impl(ptr<UpdateRiversTask> owner);

        virtual ~Impl();

        virtual bool run();
    };

    /**
     * Information about a Tile, such as the terrain it belongs to,
     * its coordinates etc... Used to determine which tiles need to be
     * produced in ParticleProducer, as well as which tiles need to be
     * released after usage.
     */
    struct TileInfo
    {
        bool operator==(const TileInfo t);

        /**
         * Creates a new TileInfo.
         */
        TileInfo();

        /**
         * Creates a new TileInfo.
         *
         * @param terrainId the id of the terrain that contains this Tile.
         * @param lp tile logical coordinates (level, tx, ty).
         * @param fp tile real coordinates (x, y, width).
         */
        TileInfo(int terrainId, vec3i lp, vec3f fp);

        /**
         * Deletes this TileInfo.
         */
        ~TileInfo();

        /**
         * Id of the terrain to which this tile belongs.
         */
        int terrainId;

        /**
         * Tile logical coordinates (level, tx, ty).
         */
        vec3i lp;

        /**
         * Tile real coordinates (x, y, width).
         */
        vec3f fp;

    };

    /**
     * Information about a terrainNode, such as the corresponding
     * particleProducer, elevation cache, display informations etc..
     */
    struct TerrainInfo
    {
        /**
         * Id of this terrain.
         */
        int id;

        /**
         * SceneNode (terrainNode + texture caches).
         */
        ptr<SceneNode> tn;

        /**
         * TerrainNode of the terrain.
         */
        ptr<TerrainNode> t;

        /**
         * The FlowDataFactory used to create river flows.
         */
        ptr<TileProducer> flows;

        /**
         * Currently selected tile in tile caches.
         */
        int currentTile;

        /**
         * Currently visible tiles in this terrain.
         */
        vector<pair<vec3i, vec3f> > displayedTiles;
    };

    /**
     * A ParticleProducer.
     */
    ptr<ParticleProducer> particles;

    /**
     * A TerrainParticleLayer.
     */
    ptr<TerrainParticleLayer> terrainLayer;

    /**
     * See #tileInfos.
     */
    typedef map<TileCache::Tile::TId, TileInfo*> TileInfos;

    /**
     * Time step at each frame. Changes the speed of the river.
     */
    float timeStep;

    /**
     * List of terrain for which we want to draw Roads.
     * Will contain every terrain from the parent Node whose name start with #terrain's value.
     */
    vector<TerrainInfo> terrainInfos;

    /**
     * Maps a TerrainInfo and a Tile Id to a TileInfo.
     */
    vector<TileCache::Tile::TId> usedTiles;

    /**
     * Currently visible TileInfos, for each Terrain.
     * This is used to determine which tiles should be released when displaying a new frame, if the camera moved.
     */
    TileInfos tileInfos;

    /**
     * Previous frame visible TileInfos, for each Terrain.
     * See #tileInfos.
     */
    TileInfos previousFrameTiles;

    /**
     * Determines whether we need to recover the list of terrains associated to this Task.
     */
    bool initialized;

    /**
     * Adds the list of tiles that should be produced to #tileInfos.
     * These tiles correspond to the visible leaf-tiles.
     */
    void getTilesToUpdate(TerrainInfo &terrain, ptr<TerrainQuad> q);

    /**
     * Main method called for updating rivers.
     * Calls the ParticleManager#compute() & ParticleManager#MoveParticles() methods.
     */
    void updateRivers();

    /**
     * Releases the unused tiles.
     */
    void putTiles();
};

}

#endif
