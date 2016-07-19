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

#ifndef _PROLAND_PARTICLE_LAYER_H_
#define _PROLAND_PARTICLE_LAYER_H_

#include "proland/particles/ParticleStorage.h"
#include "proland/producer/TileProducer.h"

namespace proland
{

class ParticleProducer;

/**
 * An abstract layer for a ParticleProducer.
 * @ingroup particles
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class ParticleLayer : public Object
{
public:
    /**
     * Creates a new ParticleLayer.
     *
     * @param type the layer's type.
     * @param particleSize the size in bytes of the layer specific data that
     *      must be stored for each particle.
     */
    ParticleLayer(const char *type, int particleSize);

    /**
     * Deletes this ParticleLayer.
     */
    virtual ~ParticleLayer();

    /**
     * Returns the ParticleProducer to which this ParticleLayer belongs.
     */
    ParticleProducer *getOwner();

    /**
     * Returns true if this layer is enabled. If it is not the case, its
     * #moveParticles, #removeOldParticles and #addNewParticles will not
     * be called by the ParticleProducer.
     */
    bool isEnabled();

    /**
     * Enables or disables this ParticleLayer. The #moveParticles,
     * #removeOldParticles and #addNewParticles methods are called only
     * on enabled layers.
     *
     * @param enable true to enable this layer, false otherwise.
     */
    void setIsEnabled(bool enable);

    /**
     * Returns the size in bytes of the layer specific data that must be
     * stored for each particle.
     */
    virtual int getParticleSize();

    /**
     * Returns a pointer to the layer specific data of the given particle.
     *
     * @param p a particle.
     * @return a pointer to the data that is specific to this layer for the
     *      given particle.
     */
    inline void *getParticleData(ParticleStorage::Particle *p)
    {
        return (void*) (((unsigned char*) p) + offset);
    }

    /**
     * Returns a pointer to the Particle corresponding to the given layer
     * specific data.
     *
     * @param p a pointer to the data that is specific to this layer for a
     *      given particle.
     * @return the corresponding Particle.
     */
    inline ParticleStorage::Particle *getParticle(void *p)
    {
        return (ParticleStorage::Particle*) (((unsigned char*) p) - offset);
    }

    /**
     * Returns the tile producers used by this ParticleLayer.
     *
     * @param[out] producers the tile producers used by this ParticleLayer.
     */
    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const
    {
    }

    /**
     * Moves the existing %particles.
     * The default implementation of this method does nothing.
     *
     * @param dt the elapsed time since the last call to this method, in
     *      microseconds.
     */
    virtual void moveParticles(double dt);

    /**
     * Removes old %particles.
     * The default implementation of this method does nothing.
     */
    virtual void removeOldParticles();

    /**
     * Adds new %particles.
     * The default implementation of this method does nothing.
     */
    virtual void addNewParticles();

protected:
    /**
     * Initializes this ParticleLayer.
     */
    void init(int particleSize);

    /**
     * Initializes this ParticleLayer. If this layer needs reference to other
     * layers it can initialize them in this method (using the template method
     * ParticleProducer#getLayer()). The default implementation of this method
     * does nothing.
     */
    virtual void initialize();

    /**
     * Initializes the data that is specific to this layer in the given
     * particle. The default implementation of this method does nothing.
     */
    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<ParticleLayer> p);

private:
    /**
     * The ParticleProducer to which this ParticleLayer belongs.
     */
    ParticleProducer *owner;

    /**
     * The size in bytes of the layer specific data that must be stored for
     * each particle.
     */
    int size;

    /**
     * The offset of the data that is specific to this layer in the global
     * particle data.
     */
    int offset;

    /**
     * True if this layer is enabled.
     */
    bool enabled;

    friend class ParticleProducer;
};

}

#endif
