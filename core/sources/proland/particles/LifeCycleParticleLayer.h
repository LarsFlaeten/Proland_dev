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

#ifndef _PROLAND_LIFECYCLE_PARTICLE_LAYER_H_
#define _PROLAND_LIFECYCLE_PARTICLE_LAYER_H_

#include "proland/particles/ParticleProducer.h"

namespace proland
{

/**
 * A ParticleLayer to manage the lifecycle of %particles. This class manages
 * a simple lifecycle where %particles can be fading in, active, or fading out.
 * The transitions between the three states are based on the particle's age,
 * and on globally defined fading in, active, and fading out delays.
 * @ingroup particles
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class LifeCycleParticleLayer : public ParticleLayer
{
public:
    /**
     * Layer specific particle data for managing the lifecycle of %particles.
     */
    struct LifeCycleParticle
    {
        /**
         * The birth date of this particle, in microseconds. This birth date
         * allows one to compute the age of the particle. If this age is
         * between 0 and fadeInDelay the particle is fading in. If this age is
         * between fadeInDelay and fadeInDelay + activeDelay, the particle
         * is active. Otherwise it is fading out. Note that we do not store the
         * particle's age directly to avoid updating it at each frame.
         */
        float birthDate;
    };

    /**
     * Creates a new LifeCycleParticleLayer.
     *
     * @param fadeInDelay the fade in delay of %particles, in microseconds.
     *      0 means that %particles are created directly in active state.
     * @param activeDelay the active delay of %particles, in microseconds.
     * @param fadeOutDelay the fade out delay of %particles, in microseconds.
     *      0 means that %particles are deleted when the become inactive.
     */
    LifeCycleParticleLayer(float fadeInDelay, float activeDelay, float fadeOutDelay);

    /**
     * Deletes this LifeCycleParticleLayer.
     */
    virtual ~LifeCycleParticleLayer();

    /**
     * Returns the fade in delay of %particles, in microseconds.
     * 0 means that %particles are created directly in active state.
     */
    float getFadeInDelay() const;

    /**
     * Sets the fade in delay of %particles, in microseconds.
     * 0 means that %particles are created directly in active state.
     *
     * @param delay the new fade in delay.
     */
    void setFadeInDelay(float delay);

    /**
     * Returns the active delay of %particles, in microseconds.
     */
    float getActiveDelay() const;

    /**
     * Sets the active delay of %particles, in microseconds.
     *
     * @param delay the new active delay.
     */
    void setActiveDelay(float delay);

    /**
     * Returns the fade out delay of %particles, in microseconds.
     * 0 means that %particles are deleted when they become inactive.
     */
    float getFadeOutDelay() const;

    /**
     * Sets the fade out delay of %particles, in microseconds.
     * 0 means that %particles are deleted when they become inactive.
     *
     * @param delay the new fade out delay.
     */
    void setFadeOutDelay(float delay);

    /**
     * Returns the lifecycle specific data of the given particle.
     *
     * @param p a particle.
     */
    inline LifeCycleParticle *getLifeCycle(ParticleStorage::Particle *p)
    {
        return (LifeCycleParticle*) getParticleData(p);
    }

    /**
     * Returns the birth date of the given particle.
     *
     * @param p a particle.
     * @return the birth date of the given particle, in microseconds.
     */
    inline float getBirthDate(ParticleStorage::Particle *p)
    {
        return getLifeCycle(p)->birthDate;
    }

    /**
     * Returns true if the given particle is fading in.
     */
    bool isFadingIn(ParticleStorage::Particle *p);

    /**
     * Returns true if the given particle is active.
     */
    bool isActive(ParticleStorage::Particle *p);

    /**
     * Returns true if the given particle is fading out.
     */
    bool isFadingOut(ParticleStorage::Particle *p);

    /**
     * Forces the given particle to start fading out.
     */
    void setFadingOut(ParticleStorage::Particle *p);

    /**
     * Forces the given particle to be deleted immediatly.
     */
    void killParticle(ParticleStorage::Particle *p);

    /**
     * Returns an intensity for the given particle, based on its current
     * state. This intensity varies between 0 to 1 during fade in, stays equal
     * to 1 when the particle is active, and varies from 1 to 0 during fade out.
     */
    float getIntensity(ParticleStorage::Particle *p);

    /**
     * Updates the current time. We don't need to update the %particles
     * because we store their birth date instead of their age.
     */
    virtual void moveParticles(double dt);

    /**
     * Deletes the %particles that have completely faded out.
     */
    virtual void removeOldParticles();

protected:
    /**
     * Creates an uninitialized LifeCycleParticleLayer.
     */
    LifeCycleParticleLayer();

    /**
     * Initializes this LifeCycleParticleLayer. See #LifeCycleParticleLayer.
     */
    void init(float fadeInDelay, float activeDelay, float fadeOutDelay);

    /**
     * Initializes the birth date of the given particle to #time.
     */
    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<LifeCycleParticleLayer> p);

private:
    /**
     * The fade in delay of %particles, in microseconds. 0 means that
     * %particles are created directly in active state.
     */
    float fadeInDelay;

    /**
     * The active delay of %particles, in microseconds.
     */
    float activeDelay;

    /**
     * The fade out delay of %particles, in microseconds. 0 means that
     * %particles are deleted when the become inactive.
     */
    float fadeOutDelay;

    /**
     * The current time, in microseconds. This time is updated in
     * #moveParticles and used to set the birth date of particles in
     * #initParticle.
     */
    float time;
};

}

#endif
