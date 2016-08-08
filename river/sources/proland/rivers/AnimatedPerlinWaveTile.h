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

#ifndef _PROLAND_ANIMATEDPERLINWAVETILE_H_
#define _PROLAND_ANIMATEDPERLINWAVETILE_H_

#include "proland/rivers/WaveTile.h"

namespace proland
{

/**
 * WaveTile are Texture used to advect rivers normal.
 * They can be updated through time, in order to change the waves profiles.
 * AnimatedPerlinWaveTile is a serie of #timeLoop Noise Textures displayed successively.
 * @ingroup rivers
 * @author Antoine Begault
 */
PROLAND_API class AnimatedPerlinWaveTile : public WaveTile
{
public:
    /**
     * Creates a new AnimatedPerlinWaveTile.
     * See WaveTile#WaveTile().
     */
    AnimatedPerlinWaveTile(std::string &name, int tileSize, int gridSize, float waveLength, int timeLoop);

    /**
     * Deletes an AnimatedPerlinWaveTile.
     */
    virtual ~AnimatedPerlinWaveTile();

    /**
     * See WaveTile#updateUniform().
     */
    virtual void updateUniform(ptr<Program> p);

protected:
    /**
     * 3D Noise generator. Taken from Qizhi's implementation.
     */
    struct Noise //3D noise
    {
    public:
       Noise (unsigned int seed=171717);

       void reinitialize(unsigned int seed);

       float operator()(float x, float y, float z) const;

       float operator()(float x, float y, float z, int wxy, int wz) const;

       float operator()(const vec3f &x) const
       {
           return (*this)(x[0], x[1], x[2]);
       }

    protected:
       static const unsigned int n=256;

       vec3f basis[n];

       int perm[n];

       unsigned int hash_index(int i, int j, int k, int wxy, int wz) const
       {
           return perm[(perm[(perm[i % wxy] + j) % wxy] + k % wz) % wxy];
       }

       unsigned int hash_index(int i, int j, int k) const
       {
           return perm[(perm[(perm[i % n] + j) % n] + k) % n];
       }
    };

    /**
     * Creates a new AnimatedPerlinWaveTile.
     */
    AnimatedPerlinWaveTile();

    /**
     * Initializes the fields of a AnimatedPerlinWaveTile.
     * See WaveTile#init().
     */
    void init(ptr<Texture2D> t, int size, int numLodLevels, int timeLoop, unsigned int seed);

    /**
     * Initializes the fields of a AnimatedPerlinWaveTile.
     * See WaveTile#init().
     */
    virtual void init(std::string &name, int tileSize, int gridSize, float waveLength, int timeLoop);

    virtual void swap(ptr<AnimatedPerlinWaveTile> t);

    /**
     * Contains the textures used to animate the wave profile.
     */
    std::vector<ptr<Texture2D> > tex;
};

}

#endif
