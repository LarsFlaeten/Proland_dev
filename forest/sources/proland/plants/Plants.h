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

#ifndef _PROLAND_PLANTS_H_
#define _PROLAND_PLANTS_H_

#include "ork/render/FrameBuffer.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup plants plants
 * Provides classes to render many 3D models on a %terrain with OpenGL instancing.
 * @ingroup proland
 */

/**
 * TODO.
 * @ingroup plants
 * @author Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class Plants : public Object
{
public:
    /**
     * The GLSL Shader that is able to determine if a seed is valid (see DrawPlantsTask). If
     * it is, it will determine its properties, such as the size of the plant, its type,
     * color etc... Eliminated seeds will contain (0, 0, 0, 0) values.
     */
    ptr<Program> selectProg;

    /**
     * The GLSL Shader used to render the plants shadows, based on the selected seeds.
     */
    ptr<Program> shadowProg;

    /**
     * The GLSL Shader used to render the plants, based on the selected seeds.
     */
    ptr<Program> renderProg;

    /**
     * Creates a new Plants.
     *
     * @param minLevel the first level to display the models from this Plants.
     * @param maxLevel the last level to display the models from this Plants.
     * @param maxDistance the furthest distance at which to display the models.
     * @param lodDistance the distance between each lod.
     */
    Plants(int minLevel, int maxLevel, int minDensity, int maxDensity, int tileCacheSize, float maxDistance);

    /**
     * Deletes this Plants.
     */
    ~Plants();

    /**
     * TODO.
     */
    int getMinLevel();

    /**
     * TODO.
     */
    int getMaxLevel();

    /**
     * TODO.
     */
    int getMinDensity();

    /**
     * TODO.
     */
    int getMaxDensity();

    /**
     * TODO.
     */
    int getTileCacheSize();

    /**
     * TODO.
     */
    float getMaxDistance();

    /**
     * TODO.
     */
    int getPatternCount();

    /**
     * TODO.
     */
    float getPoissonRadius();

    /**
     * Returns the i'th pattern.
     */
    ptr<MeshBuffers> getPattern(int index);

    /**
     * TODO.
     */
    void addPattern(ptr<MeshBuffers> pattern);

    /**
     * TODO.
     */
    void setMaxDistance(float maxDistance);

protected:
    /**
     * Creates a new Plants.
     */
    Plants();

    /**
     * Initializes a Plants fields.
     *
     * See #Plants.
     */
    void init(int minLevel, int maxLevel, int minDensity, int maxDensity, int tileCacheSize, float maxDistance);

    void swap(ptr<Plants> p);

private:
    int minLevel;

    int maxLevel;

    int minDensity;

    int maxDensity;

    int tileCacheSize;

    float maxDistance;

    std::vector< ptr<MeshBuffers> > patterns;
};

}

#endif
