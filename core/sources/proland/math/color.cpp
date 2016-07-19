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

#include "proland/math/color.h"

using namespace std;

namespace proland
{

vec3f rgb2hsl(const vec3f &rgb)
{
    float vmin = min(rgb.x, min(rgb.y, rgb.z));
    float vmax = max(rgb.x, max(rgb.y, rgb.z));
    float dmax = vmax - vmin;
    float h, s, l;
    l = (vmax + vmin) / 2;
    if (dmax == 0) {
        h = 0;
        s = 0;
    } else {
        if (l < 0.5) {
            s = dmax / (vmax + vmin);
        } else {
            s = dmax / (2 - vmax - vmin);
        }
        float d[3];
        d[0] = (((vmax - rgb.x) / 6) + (dmax / 2)) / dmax;
        d[1] = (((vmax - rgb.y) / 6) + (dmax / 2)) / dmax;
        d[2] = (((vmax - rgb.z) / 6) + (dmax / 2)) / dmax;
        if (rgb.x == vmax) {
            h = d[2] - d[1];
        } else if (rgb.y == vmax) {
            h = (1.0f / 3) + d[0] - d[2];
        } else /*if (rgb.z == vmax)*/ {
            h = (2.0f / 3) + d[1] - d[0];
        }
        if (h < 0) {
            h += 1;
        }
        if (h > 1) {
            h -= 1;
        }
    }
    return vec3f(h, s, l);
}

static float h2rgb(float v1, float v2, float vH)
{
    float r;
    if (vH < 0) vH += 1;
    if (vH > 1) vH -= 1;
    if (vH < 1.0 / 6) {
        r = v1 + (v2 - v1) * 6 * vH;
    } else if (vH < 1.0 / 2) {
        r = v2;
    } else if (vH < 2.0 / 3) {
        r = v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6;
    } else {
        r = v1;
    }
    return r;
}

vec3f hsl2rgb(const vec3f &hsl)
{
    float s = hsl.y;
    float l = hsl.z;
    if (s == 0) {
        return vec3f(l, l, l);
    } else {
       float v1, v2;
       if (l < 0.5) {
           v2 = l * (1 + s);
       } else {
           v2 = l + s - s * l;
       }
       v1 = 2 * l - v2;
       float r = h2rgb(v1, v2, hsl.x + 1.0f / 3);
       float g = h2rgb(v1, v2, hsl.x);
       float b = h2rgb(v1, v2, hsl.x - 1.0f / 3);
       return vec3f(r, g, b);
    }
}

mat3f dcolor(const vec3f &rgb, const vec3f &amp)
{
    vec3f hsl = rgb2hsl(rgb);
    vec3f RGB;
    mat3f m;
    RGB = hsl2rgb(hsl + vec3f(0.01f, 0, 0));
    m.setColumn(0, (RGB - rgb) / 0.01f * amp.x);
    RGB = hsl2rgb(hsl + vec3f(0, 0.01f, 0));
    m.setColumn(1, (RGB - rgb) / 0.01f * amp.y);
    RGB = hsl2rgb(hsl + vec3f(0, 0, 0.01f));
    m.setColumn(2, (RGB - rgb) / 0.01f * amp.z);
    return m;
}

}
