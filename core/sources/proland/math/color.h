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

#ifndef _PROLAND_COLOR_H_
#define _PROLAND_COLOR_H_

#include "ork/math/vec3.h"
#include "ork/math/mat3.h"

using namespace ork;

namespace proland
{

/**
 * Converts a color from rgb space to hsl space. Color coordinates in [0,1]
 * @ingroup proland_math
 */
PROLAND_API vec3f rgb2hsl(const vec3f &rgb);

/**
 * Converts a color from hsl space to rgb space. Color coordinates in [0,1]
 * @ingroup proland_math
 */
PROLAND_API vec3f hsl2rgb(const vec3f &hsl);

/**
 * Computes the derivates of an rgb color with respect to hsl coordinates.
 * @ingroup proland_math
 *
 * @param rgb a color in rgb space.
 * @param amp scaling factors for each partial derivative.
 * @return a matrix whose columns correspond to the partial derivatives drgb/dh,
 *      drgb/ds and drgb/dl, scaled by the factors amp.x, amp.y and amp.z.
 */
PROLAND_API mat3f dcolor(const vec3f &rgb, const vec3f &amp = vec3f(1/12.0f, 1/3.0f, 1/6.0f));

}

#endif
