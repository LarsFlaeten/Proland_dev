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

#ifndef _PROLAND_SCREEN_PARTICLE_LAYER_H_
#define _PROLAND_SCREEN_PARTICLE_LAYER_H_

#include "ork/math/box2.h"
#include "ork/render/FrameBuffer.h"
#include "ork/scenegraph/SceneManager.h"

#include "proland/particles/LifeCycleParticleLayer.h"
#include "proland/particles/WorldParticleLayer.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup screen screen
 * Provides a particle layer to force %particles to stay in the viewport.
 * @ingroup particles
 */

class ParticleGrid;

class RangeList;

/**
 * A ParticleLayer to force %particles to stay in the viewport, with a uniform
 * density. This layer requires %particles initially managed in world space,
 * using a WorldParticleLayer. It then forces these %particles to stay in the
 * framebuffer viewport. For that it deletes %particles that project outside the
 * viewport, and creates new %particles in this viewport to maintain a constant
 * density of %particles. In fact particles are not really deleted, but forced
 * to fading out. This requires a LifeCycleParticleLayer.
 * @ingroup screen
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class ScreenParticleLayer : public ParticleLayer
{
public:
    /**
     * The reason why a particle is fading out.
     */
    enum status {
        AGE, ///< the particle reached its maximum age
        OUTSIDE_VIEWPORT, ///< the particle projected outside the viewport
        POISSON_DISK ///< the particle was too close to other %particles
    };

    /**
     * Layer specific particle data for managing %particles in screen space.
     */
    struct ScreenParticle
    {
        /**
         * The current particle position in screen space, in pixels.
         */
        vec2f screenPos;

        /**
         * If this particle is fading out, why.
         */
        status reason;
    };

    /**
     * Creates a new ScreenParticleLayer.
     * @param radius the radius of each particle, in pixels. This radius is
     *     used as a Poisson-disk radius for maintaining a Poisson distribution
     *     of %particles in screen space.
     * @param offscreenDepthBuffer the offscreen buffer that contains depth.
     *     This texture is used to avoid copying the depth buffer at each frame.
     */
    ScreenParticleLayer(float radius, ptr<Texture2D> offscreenDepthBuffer);

    /**
     * Deletes this LifeCycleParticleLayer.
     */
    virtual ~ScreenParticleLayer();

    /**
     * Returns the Poisson-disk radius of each particle, in pixels.
     */
    float getParticleRadius() const;

    /**
     * Sets the Poisson-disk radius of each particle, in pixels.
     */
    void setParticleRadius(float radius);

    /**
     * Returns the screen space specific data of the given particle.
     *
     * @param p a particle.
     */
    inline ScreenParticle *getScreenParticle(ParticleStorage::Particle *p)
    {
        return (ScreenParticle*) getParticleData(p);
    }

    /**
     * Sets the scene manager used to set the world to screen transformation.
     */
    void setSceneManager(SceneManager *manager);

    virtual void moveParticles(double dt);

    virtual void removeOldParticles();

    virtual void addNewParticles();

    /**
     * Returns the neighbors of the given screen particle.
     *
     * @param s a screen particle.
     * @param[out] n the number of neighbors of s.
     * @return the neighbors of s.
     */
    ScreenParticle** getNeighbors(ScreenParticle *s, int &n);

protected:
    /**
     * Creates an uninitialized ScreenParticleLayer.
     */
    ScreenParticleLayer();

    /**
     * Initializes this ScreenParticleLayer. See #ScreenParticleLayer.
     */
    void init(float radius, ptr<Texture2D> offscreenDepthBuffer);

    virtual void initialize();

    virtual void initParticle(ParticleStorage::Particle *p);

    virtual void swap(ptr<ScreenParticleLayer> p);

private:
    /**
     * The scene manager, used to get the world to screen transformation.
     */
    SceneManager *scene;

    /**
     * The Poisson-disk radius of each particle, in pixels.
     */
    float radius;

    /**
     * The current bounds of the viewport, in pixels. Particles are forced to
     * stay in this viewport by this particle layer.
     */
    box2f bounds;

    // -----------------------------------------------------------------------
    // objects needed create new particles with a Poisson-disk distribution

    /**
     * A ParticleGrid to store %particles according to their screen position.
     * This grid is necessary to quickly find the neighbors of %particles,
     * which are themselves needed to maintain a Poisson-disk distribution.
     */
    ParticleGrid *grid;

    /**
     * Data structure used to find where to create new %particles, so that they
     * form a Poisson-disk distribution.
     */
    RangeList *ranges;

    // -----------------------------------------------------------------------
    // objects needed to read the depths of newly created particles

    /**
     * The world to screen transformation of the last frame.
     * Used to determine if the camera moved at this frame.
     */
    mat4d lastWorldToScreen;

    /**
     * The viewport of the last frame.
     * Used to determine if the camera moved at this frame.
     */
    vec4i lastViewport;

    /**
     * True if the camera is not moving, and if the depth buffer has been
     * read into #depthArray.
     */
    bool depthBufferRead;

    /**
     * The size of the #depthArray array.
     */
    int depthArraySize;

    /**
     * Array used to get the depth of the %particles created at this frame or
     * the depth of all the pixels of the depth buffer.
     */
    float *depthArray;

    /**
     * Framebuffer in which to execute the #packer program. This
     * framebuffer uses as color attachment a texture containing the
     * depths read by #packer, which are then read back in #depthArray.
     */
    ptr<FrameBuffer> frameBuffer;

    /**
     * Mesh used to retrieve depths with the #packer program. This
     * point mesh contains one point per particle. Each point contains the
     * screen coordinates of a particle, and the index of this particle.
     */
    ptr< Mesh<vec3f, unsigned int> > mesh;

    /**
     * Program used to get the depths of a set of %particles.
     */
    static static_ptr<Program> packer;

    /**
     * Sampler uniform to access the #depthTexture texture.
     */
    ptr<UniformSampler> depthTextureU;

    /**
     * Viewport width and height, plus width of the #framebuffer viewport.
     */
    ptr<Uniform3f> sizeU;

    /**
     * A copy of the depth buffer in the form of a texture. This texture
     * is needed by the #packer program to access the depth of arbitrary
     * pixels in the depth buffer.
     */
    static static_ptr<Texture2D> depthBuffer;

    /**
     * True if the user provides an offscreen depthbuffer. This allows to
     * not copy the depthbuffer before reading from it.
     */
    bool useOffscreenDepthBuffer;

    // -----------------------------------------------------------------------
    // required layers

    /**
     * The layer managing the %particles in world space.
     */
    WorldParticleLayer *worldLayer;

    /**
     * The layer managing the %particles life cycle.
     */
    LifeCycleParticleLayer *lifeCycleLayer;

    // -----------------------------------------------------------------------
    // private methods

    /**
     * Updates #ranges based on the neighbors of the given particle.
     */
    void findNeighborRanges(ScreenParticle *p);

    /**
     * Creates a new particle at the given position, and adds it to #grid.
     *
     * @param pos a position in screen space, in pixels.
     * @return the created particle, or NULL if the maximum capacity of the
     *      %particles storage was reached.
     */
    ScreenParticle *newScreenParticle(const vec2f &pos);

    /**
     * Reads the depths of the given %particles from the depth buffer.
     *
     * @param particles a list of %particles.
     */
    void getParticleDepths(const std::vector<ScreenParticle*> &particles);
};

}

#endif
