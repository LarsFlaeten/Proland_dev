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

#ifndef _PROLAND_NOISE_H_
#define _PROLAND_NOISE_H_

#include "ork/math/pmath.h"

namespace proland
{

/**
 * @defgroup proland_math math
 * Provides utility math functions.
 * @ingroup proland
 */

/**
 * Returns a pseudo random integer in the range 0-2147483647.
 * @ingroup proland_math
 *
 * @param the seed used by this pseudo random generator. It is modified each
 *      time this function is called.
 * @return a peudo random integer in the range 0-2147483647.
 */
PROLAND_API inline long lrandom(long *seed)
{
    *seed = (*seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return *seed;
}

/**
 * Returns a pseudo random float number in the range 0-1.
 * @ingroup proland_math
 *
 * @param the seed used by this pseudo random generator. It is modified each
 *      time this function is called.
 * @return a pseudo random float number in the range 0-1.
 */
PROLAND_API inline float frandom(long *seed)
{
    long r = lrandom(seed) >> (31 - 24);
    return r / (float)(1 << 24);
}

/**
 * Returns a pseudo random float number with a Gaussian distribution.
 * @ingroup proland_math
 *
 * @param mean the mean of the Gaussian distribution.
 * @param stdDeviation the standard deviation of the Gaussian distribution
 *      (square root of its variance).
 * @param the seed used by this pseudo random generator. It is modified each
 *      time this function is called.
 * @return a pseudo random float number with the given Gaussian distribution.
 */
PROLAND_API inline float grandom(float mean, float stdDeviation, long *seed)
{
    float x1, x2, w, y1;
    static float y2;
    static int use_last = 0;

    if (use_last) {
        y1 = y2;
        use_last = 0;
    } else {
        do {
            x1 = 2.0f * frandom(seed) - 1.0f;
            x2 = 2.0f * frandom(seed) - 1.0f;
            w  = x1 * x1 + x2 * x2;
        } while (w >= 1.0f);
        w  = sqrt((-2.0f * log(w)) / w);
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }
    return mean + y1 * stdDeviation;
}

/**
 * Computes the classic 2D Perlin noise function.
 * @ingroup proland_math
 *
 * @param x the x coordinate of the point where the function must be evaluated.
 * @param y the y coordinate of the point where the function must be evaluated.
 * @param P an optional period to get a periodic noise function. The default
 *      value 0 means a non periodic function.
 * @return the classic 2D Perlin noise function evaluated at (x,y). This
 *      function has a main frequency of 1, and its value are between -1 and 1.
 */
PROLAND_API float cnoise(float x, float y, int P = 0);

/**
 * Computes the classic 3D Perlin noise function.
 * @ingroup proland_math
 *
 * @param x the x coordinate of the point where the function must be evaluated.
 * @param y the y coordinate of the point where the function must be evaluated.
 * @param z the z coordinate of the point where the function must be evaluated.
 * @param P an optional period to get a periodic noise function. The default
 *      value 0 means a non periodic function.
 * @return the classic 3D Perlin noise function evaluated at (x,y,z). This
 *      function has a main frequency of 1, and its value are between -1 and 1.
 */
PROLAND_API float cnoise(float x, float y, float z, int P = 0);

/**
 * Computes the 2D Perlin simplex noise function.
 * @ingroup proland_math
 *
 * @param x the x coordinate of the point where the function must be evaluated.
 * @param y the y coordinate of the point where the function must be evaluated.
 * @param P an optional period to get a periodic noise function. The default
 *      value 0 means a non periodic function.
 * @return the 2D Perlin simplex noise function evaluated at (x,y). This
 *      function has a main frequency of 1, and its value are between -1 and 1.
 */
PROLAND_API float snoise(float x, float y, int P = 0);

/**
 * Computes the 3D Perlin simplex noise function.
 * @ingroup proland_math
 *
 * @param x the x coordinate of the point where the function must be evaluated.
 * @param y the y coordinate of the point where the function must be evaluated.
 * @param z the z coordinate of the point where the function must be evaluated.
 * @param P an optional period to get a periodic noise function. The default
 *      value 0 means a non periodic function.
 * @return the 3D Perlin simplex noise function evaluated at (x,y,z). This
 *      function has a main frequency of 1, and its value are between -1 and 1.
 */
PROLAND_API float snoise(float x, float y, float z, int P = 0);

/**
 * Computes the 4D Perlin simplex noise function.
 * @ingroup proland_math
 *
 * @param x the x coordinate of the point where the function must be evaluated.
 * @param y the y coordinate of the point where the function must be evaluated.
 * @param z the z coordinate of the point where the function must be evaluated.
 * @param w the w coordinate of the point where the function must be evaluated.
 * @param P an optional period to get a periodic noise function. The default
 *      value 0 means a non periodic function.
 * @return the 4D Perlin simplex noise function evaluated at (x,y,z,w). This
 *      function has a main frequency of 1, and its value are between -1 and 1.
 */
PROLAND_API float snoise(float x, float y, float z, float w, int P = 0);

/**
 * Computes a 2D Fbm noise function in a 2D float array. This function is a sum
 * of several Perlin noise function with different frequencies and amplitudes.
 * @ingroup proland_math
 *
 * @param size the width and height of the array of values to be computed.
 * @param freq the pseudo frequency of the lower frequency noise function. The
 *      corresponding pseudo period in pixels is size/freq.
 * @param octaves the number of Perlin noise functions to add.
 * @param lacunarity the frequency factor between each noise function.
 * @param gain the amplitude factor between each noise function.
 * @return the computed size*size array of values. These values are normalized
 *      to stay in the range 0-1.
 */
PROLAND_API float *buildFbm4NoiseTexture2D(int size, int freq, int octaves, int lacunarity, float gain);

/**
 * Computes a 3D Fbm noise function in a 3D float array. This function is a sum
 * of several Perlin noise function with different frequencies and amplitudes.
 * @ingroup proland_math
 *
 * @param size the width, height and depth of the array of values to be computed.
 * @param freq the pseudo frequency of the lower frequency noise function. The
 *      corresponding pseudo period in pixels is size/freq.
 * @param octaves the number of Perlin noise functions to add.
 * @param lacunarity the frequency factor between each noise function.
 * @param gain the amplitude factor between each noise function.
 * @return the computed size*size*size array of values. These values are
 *      normalized to stay in the range 0-1.
 */
PROLAND_API float *buildFbm1NoiseTexture3D(int size, int freq, int octaves, int lacunarity, float gain);

}

#endif
