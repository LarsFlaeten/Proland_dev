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

#ifndef _PROLAND_TERRAIN_PARTICLE_LAYER_H_
#define _PROLAND_TERRAIN_PARTICLE_LAYER_H_

#include "ork/scenegraph/SceneNode.h"
#include "proland/particles/LifeCycleParticleLayer.h"
#include "proland/particles/WorldParticleLayer.h"
#include "proland/particles/screen/ScreenParticleLayer.h"
#include "proland/particles/terrain/FlowTile.h"
#include "proland/producer/TileProducer.h"
#include "proland/terrain/TerrainNode.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup terrainl terrain
 * Provides a particle layer to advect particles one a terrain.
 * @ingroup particles
 */

/**
 * A ParticleLayer to advect %particles in world space by using a velocity
 * field defined on one or more terrains. This layer requires %particles
 * world positions and velocities to be managed by a WorldParticleLayer.
 * @ingroup terrainl
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class TerrainParticleLayer : public ParticleLayer
{
public:
    /**
     * Layer specific particle data for managing %particles on terrains.
     */
    struct TerrainParticle
    {
        /**
         * The current particle position in local space inside a terrain.
         */
        vec3d terrainPos;

        /**
         * The current particle velocity in local space inside a terrain.
         */
        vec2d terrainVelocity;

        /**
         * The TileProducer that produces the FlowTile on which this %particle is.
         */
        TileProducer *producer;

        /**
         * Current particle status. See FlowTile#status.
         */
        int status;

        int terrainId;

        /**
         * True if the current particle velocity was not computed yet.
         */
        bool firstVelocityQuery;
    };

    /**
     * Contains a SceneNode and its corresponding TerrainNode.
     * The SceneNode is used to determine on which terrain the
     *   particle is via the worldToLocal methods.
     * The TerrainNode is used to determine if the particle is
     *   in the bounds of the terrain.
     */
    struct TerrainInfo
    {
    public:
        /**
         * Creates a new TerrainInfo.
         *
         * @param n a SceneNode.
         */
        TerrainInfo(ptr<SceneNode> n, int id) {
            this->node = n;
            this->terrain = (node->getField("terrain").cast<TerrainNode>());
            this->id = id;
        }

        /**
         * A SceneNode.
         */
        ptr<SceneNode> node;

        /**
         * The TerrainNode associated to #node.
         */
        ptr<TerrainNode> terrain;

        /**
         * Current terrain id. also used in particles to store the terrain on
         * which they are located.
         */
        int id;
    };

    /**
     * Creates a new TerrainParticleLayer.
     *
     * @param infos each flow producer mapped to its SceneNode.
     */
    TerrainParticleLayer(std::map<ptr<TileProducer>, TerrainInfo *> infos);

    /**
     * Deletes this LifeCycleParticleLayer.
     */
    virtual ~TerrainParticleLayer();

    /**
     * Returns the screen space specific data of the given particle.
     *
     * @param p a particle.
     */
    inline TerrainParticle *getTerrainParticle(ParticleStorage::Particle *p)
    {
        return (TerrainParticle*) getParticleData(p);
    }

    inline std::map<ptr<TileProducer>, TerrainInfo *> getTerrainInfos()
    {
        return infos;
    }

    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const;

    virtual void moveParticles(double dt);

protected:
    /**
     * Creates an uninitialized TerrainParticleLayer.
     */
    TerrainParticleLayer();

    /**
     * Initializes this TerrainParticleLayer. See #TerrainParticleLayer.
     */
    void init(std::map<ptr<TileProducer>, TerrainInfo *> infos);

    virtual void initialize();

    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<TerrainParticleLayer> p);

    /**
     * Finds the FlowTile from a given TileProducer at given local coordinates.
     *
     * @param producer the TileProducer used to create the FlowTiles.
     * @param t the last browsed FlowTile.
     * @param pos local terrain coordinates for which we need the FlowTile.
     */
    ptr<FlowTile> findFlowTile(ptr<TileProducer> producer, TileCache::Tile *t, vec3d &pos);

    /**
     * Returns the FlowTile required to compute the velocity of a given TerrainParticle.
     */
    ptr<FlowTile> getFlowTile(TerrainParticle *t);

    /**
     * Returns the TileProducer associated to the terrain on which a given Particle is.
     */
    ptr<TileProducer> getFlowProducer(ParticleStorage::Particle *p);

    /**
     * Each flow producer mapped to its SceneNode.
     */
    std::map<ptr<TileProducer>, TerrainInfo*> infos;

private:
    /**
     * The layer managing the life cycle of %particles.
     */
    LifeCycleParticleLayer *lifeCycleLayer;

    /**
     * The layer managing the %particles in screen space.
     */
    ScreenParticleLayer *screenLayer;

    /**
     * The layer managing the %particles in world space.
     */
    WorldParticleLayer *worldLayer;
};

}

#endif
