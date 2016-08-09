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

#ifndef _PROLAND_PLANTS_PRODUCER_H_
#define _PROLAND_PLANTS_PRODUCER_H_

#include "ork/scenegraph/SceneNode.h"

#include "proland/producer/TileProducer.h"
#include "proland/terrain/TerrainNode.h"
#include "proland/terrain/TileSampler.h"
#include "proland/plants/Plants.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * TODO.
 * @ingroup plants
 * @author Eric Bruneton
 */
PROLAND_API class PlantsProducer : public TileProducer
{
public:
    /**
     * TODO.
     */
    ptr<SceneNode> node;

    /**
     * TODO.
     */
    ptr<TerrainNode> terrain;

    /**
     * TODO.
     */
    vec3d localCameraPos;

    /**
     * TODO.
     */
    vec3d tangentCameraPos;

    /**
     * TODO.
     */
    mat4d localToTangentFrame;

    /**
     * TODO.
     */
    mat4d localToScreen;

    /**
     * TODO.
     */
    mat4d screenToLocal;

    /**
     * TODO.
     */
    vec3d frustumV[8];

    /**
     * TODO.
     */
    vec4d frustumP[6];

    /**
     * TODO.
     */
    vec4d frustumZ;

    /**
     * TODO.
     */
    double zNear;

    /**
     * TODO.
     */
    double zRange;

    /**
     * TODO.
     */
    mat4d tangentFrameToScreen;

    /**
     * TODO.
     */
    mat4d tangentFrameToWorld;

    /**
     * TODO.
     */
    mat3d tangentSpaceToWorld;

    /**
     * TODO.
     */
    mat4d cameraToTangentFrame;

    /**
     * TODO.
     */
    vec3d cameraRefPos;

    /**
     * TODO.
     */
    vec3d tangentSunDir;

    /**
     * TODO.
     */
    int *offsets;

    /**
     * TODO.
     */
    int *sizes;

    /**
     * TODO.
     */
    int count;

    /**
     * TODO.
     */
    int total;

    /**
     * TODO.
     */
    vector<vec4d> plantBounds; // frustum z min max, altitude min max

    /**
     * TODO.
     */
    box3d plantBox; // bounding box in local space

    /**
     * TODO.
     */
    vector< ptr<PlantsProducer> > slaves;

    /**
     * TODO.
     */
    PlantsProducer *master;

    /**
     * TODO.
     */
    PlantsProducer(ptr<SceneNode> node, ptr<TerrainNode> terrain, ptr<Plants> plants,
        ptr<TileSampler> lcc, ptr<TileSampler> z, ptr<TileSampler> n, ptr<TileSampler> occ, ptr<TileCache> cache);

    /**
     * TODO.
     */
    PlantsProducer(PlantsProducer *master);

    /**
     * TODO.
     */
    virtual ~PlantsProducer();

    virtual bool hasTile(int level, int tx, int ty);

    /**
     * TODO.
     */
    void produceTiles();

    /**
     * TODO.
     */
    ptr<MeshBuffers> getPlantsMesh();

    /**
     * TODO.
     */
    static ptr<PlantsProducer> getPlantsProducer(ptr<SceneNode> terrain, ptr<Plants> plants);

protected:
    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

private:
    struct Tree
    {
        int tileCount;

        bool needTile;

        bool *needTiles;

        TileCache::Tile **tiles;

        Tree *children[4];

        Tree(int tileCount);

        ~Tree();

        void recursiveDelete(TileProducer *owner);
    };

    ptr<Plants> plants;

    ptr<TileSampler> lcc;

    ptr<TileSampler> z;

    ptr<TileSampler> n;

    ptr<TileSampler> occ;

    ptr<Uniform3f> tileOffsetU;

    ptr<UniformMatrix2f> tileDeformU;

    Tree *usedTiles;

    static map<pair<SceneNode*, Plants*>, PlantsProducer*> producers;

    bool mustAmplifyTile(double ox, double oy, double l);

    void putTiles(Tree **t, ptr<TerrainQuad> q);

    void getTiles(Tree **t, ptr<TerrainQuad> q);

    void updateTerrainHeights(ptr<TerrainQuad> q);
};

}

#endif
