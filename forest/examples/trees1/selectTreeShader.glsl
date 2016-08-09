/*
 * Proland: a procedural landscape rendering library.
 * Copyright (c) 2008-2011 INRIA
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Proland is distributed under a dual-license scheme.
 * You can obtain a specific license from Inria: proland-licensing@inria.fr.
 */

/*
 * Authors: Eric Bruneton, Antoine Begault, Guillaume Piolat.
 */

//#extension GL_EXT_gpu_shader4 : enable

float rand(vec2 co){
        return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float x){
        return fract(sin(x*(12.9898+78.233)) * 43758.5453);
}

#ifdef _VERTEX_

layout(location=0) in vec3 vertex;
out vec3 pt;

void main()
{
    pt = vertex;
}

#endif

#ifdef _GEOMETRY_

#include "textureTile.glsl"

uniform vec3 tileOffset;
uniform samplerTile lccSampler;
uniform samplerTile elevationSampler;
uniform samplerTile fragmentNormalSampler;

layout(points) in;
layout(points,max_vertices=1) out;
in vec3 pt[];
out vec3 pos;
out vec2 p_n;
out vec4 p_colorsize;
out vec4 p_seeds;


void main()
{
    if (textureTile(lccSampler, pt[0].xy).r > 0.25) {
        float z = textureTile(elevationSampler, pt[0].xy).z - 0.35;
        vec2 n = textureTile(fragmentNormalSampler, pt[0].xy).xy;
        pos = vec3(pt[0].xy * tileOffset.z + tileOffset.xy, z);
        //vec4 seeds = unpackUnorm4x8(floatBitsToUint(pt[0].z));
        // we replace above by a (quite) random function
        vec4 seeds;
        seeds.r = rand(pt[0].z);
        seeds.g = rand(seeds.r);
        seeds.b = rand(seeds.g);
        seeds.a = rand(seeds.b);
        vec3 color = vec3(1.0) + (seeds.rgb - vec3(0.5)) * vec3(0.45, 0.45, 0.75);
        float size = mix(0.8, 1.2, seeds.w) * 0.5;
        float seed = dot(seeds, vec4(0.25));
        //params = vec3(uintBitsToFloat(packUnorm2x16(n)),
        //            uintBitsToFloat(packUnorm4x8(vec4(color, size))),
        //            uintBitsToFloat(packUnorm4x8(vec4(seed, 0.0, 0.0, 0.0))));
        p_n = n;
        p_colorsize = vec4(color, size);
        p_seeds = vec4(seed, 0.0, 0.0, 0.0);
        EmitVertex();
        EndPrimitive();
    }
}

#endif
