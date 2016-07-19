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

#ifndef _PROLAND_RANDOM_PARTICLE_LAYER_H_
#define _PROLAND_RANDOM_PARTICLE_LAYER_H_

#include "ork/math/box3.h"
#include "proland/particles/ParticleProducer.h"

using namespace ork;

namespace proland
{

/**
 * A ParticleLayer to add random 3D coordinates to a %particle. This class provides
 * particle data to store a position for each particle.
 * @ingroup particles
 * @authors Antoine Begault, Eric Bruneton
 */
PROLAND_API class RandomParticleLayer : public ParticleLayer
{
public:
    /**
     * Layer specific particle data for managing random %particles.
     */
    struct RandomParticle
    {
        /**
         * The current particle random position.
         */
        vec3f randomPos;
    };

    /**
     * Creates a new RandomParticleLayer.
     *
     * @param bounds the bounds of the random positions.
     */
    RandomParticleLayer(box3f bounds);

    /**
     * Deletes this RandomParticleLayer.
     */
    virtual ~RandomParticleLayer();

    /**
     * Returns the random specific data of the given particle.
     *
     * @param p a particle.
     */
    inline RandomParticle *getRandomParticle(ParticleStorage::Particle *p)
    {
        return (RandomParticle*) getParticleData(p);
    }

    virtual void addNewParticles();

protected:
    /**
     * Creates an uninitialized RandomParticleLayer.
     */
    RandomParticleLayer();

    /**
     * Initializes this RandomParticleLayer. See #RandomParticleLayer.
     */
    void init(box3f bounds);

    /**
     * Initializes the random position of the given particle.
     */
    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<RandomParticleLayer> p);

private:
    /**
     * The bounds of the random positions.
     */
    box3f bounds;
};

}

#endif
