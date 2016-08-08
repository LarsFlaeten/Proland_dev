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

#ifndef _PROLAND_WAVETILE_H_
#define _PROLAND_WAVETILE_H_

#include "ork/core/Object.h"
#include "ork/render/Program.h"
#include "ork/render/Texture2D.h"

using namespace ork;

namespace proland
{

/**
 * WaveTile are Texture used to advect rivers normal.
 * They can be updated through time, in order to change the waves profiles.
 * @ingroup rivers
 * @author Antoine Begault
 */
PROLAND_API class WaveTile : public Object
{
public:
    /**
     * Creates a new WaveTile.
     *
     * @param tex the Texture2D that contains the wave profiles.
     * @param gridSize size of the texture.
     * @param tileSize size of a tile.
     * @param waveLength size of a wave.
     * @param timeLoop number of frames of a wave cycle.
     */
    WaveTile(std::string &name, ptr<Texture2D> tex, int gridSize, int tileSize, float waveLength, int timeLoop);

    /**
     * Deletes this WaveTile.
     */
    virtual ~WaveTile();

    /**
     * Updates the Texture depending on the time spent between the two images.
     */
    virtual void timeStep(float dt);

    /**
     * Updates the GLSL uniforms.
     */
    virtual void updateUniform(ptr<Program> p);

    virtual void checkUniforms(ptr<Program> p);

    void setWaveLength(float length);

    float getWaveLength() const;

protected:
    /**
     * Creates a new WaveTile.
     */
    WaveTile();

    virtual void swap(ptr<WaveTile> t);
    /**
     * initializes the fields of a WaveTile.
     *
     * @param tex the Texture2D that contains the wave profiles.
     * @param gridSize size of the texture.
     * @param tileSize size of a tile.
     * @param waveLength size of a wave.
     * @param timeLoop number of frames of a wave cycle.
     */
    virtual void init(std::string &name, ptr<Texture2D> tex, int gridSize, int tileSize, float waveLength, int timeLoop);

    /**
     * Current wave tile's name.
     */
    std::string name;

    /**
     * The Texture2D that contains the wave profiles.
     */
    ptr<Texture2D> tex;

    /**
     * Size of the texture.
     */
    int gridSize;

    /**
     * Size of a tile.
     */
    int tileSize;

    /**
     * Current Time in the time cycle.
     */
    float time;

    /**
     * Number of frames of a wave cycle.
     */
    int timeLoop;

    /**
     * Size of a wave.
     */
    float waveLength;

    /**
     * Last updated program.
     * Used to avoid querying again every uniforms.
     */
    ptr<Program> lastProgram;

    /**
     * Current Time in the time cycle.
     */
    ptr<Uniform1f> timeU;

    /**
     * Number of frames of a wave cycle.
     */
    ptr<Uniform1f> timeLoopU;

    /**
     * Size of a wave.
     */
    ptr<Uniform1f> lengthU;

    /**
     * Size of a wave pattern inside texture.
     */
    ptr<Uniform1f> patternTexSizeU;

    /**
     * The sampler containing the texture.
     */
    ptr<UniformSampler> patternTexU;
};

}

#endif
