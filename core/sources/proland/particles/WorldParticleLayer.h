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

#ifndef _PROLAND_WORLD_PARTICLE_LAYER_H_
#define _PROLAND_WORLD_PARTICLE_LAYER_H_

#include "ork/math/vec3.h"
#include "proland/particles/ParticleProducer.h"

using namespace ork;

namespace proland
{

#define UNINITIALIZED -1e9

/**
 * A ParticleLayer to manage %particles in world space. This class provides
 * particle data to store a position and a velocity for each particle. It
 * updates the positions based on velocities at each frame, but it does not
 * compute the velocities itself. This should be done by another layer.
 * @ingroup particles
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class WorldParticleLayer : public ParticleLayer
{
public:
    /**
     * Layer specific particle data for managing %particles in world space.
     */
    struct WorldParticle
    {
        /**
         * The current particle position in world space.
         */
        vec3d worldPos;

        /**
         * The current particle velocity in world space.
         */
        vec3f worldVelocity;
    };

    /**
     * Creates a new WorldParticleLayer.
     *
     * @param speedFactor a global scaling factor to be applied to all particle
     *      velocities.
     */
    WorldParticleLayer(float speedFactor);

    /**
     * Deletes this WorldParticleLayer.
     */
    virtual ~WorldParticleLayer();

    /**
     * Returns the global scaling factor to be applied to all particle
     * velocities.
     */
    float getSpeedFactor() const;

    /**
     * Sets the global scaling factor to be applied to all particle
     * velocities.
     */
    void setSpeedFactor(float speedFactor);

    /**
     * Returns true if this WorldParticleLayer is in paused state.
     */
    bool isPaused() const;

    /**
     * Sets the paused state of this WorldParticleLayer.
     *
     * @param paused if true, particles position won't be updated.
     */
    void setPaused(bool paused);

    /**
     * Returns the world space specific data of the given particle.
     *
     * @param p a particle.
     */
    inline WorldParticle *getWorldParticle(ParticleStorage::Particle *p)
    {
        return (WorldParticle*) getParticleData(p);
    }

    /**
     * Moves the %particles based on their velocity. The velocities are not
     * updated. This should be done by another layer.
     */
    virtual void moveParticles(double dt);

protected:
    /**
     * Creates an uninitialized WorldParticleLayer.
     */
    WorldParticleLayer();

    /**
     * Initializes this WorldParticleLayer. See #WorldParticleLayer.
     */
    void init(float speedFactor);

    /**
     * Initializes the world position and velocity of the given particle.
     */
    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<WorldParticleLayer> p);

private:
    /**
     * Global scaling factor to be applied to all particle velocities.
     */
    float speedFactor;

    /**
     * If true, particles position won't be updated.
     */
    bool paused;
};

}

#endif
