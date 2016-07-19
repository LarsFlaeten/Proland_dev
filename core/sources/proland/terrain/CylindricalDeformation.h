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

#ifndef _PROLAND_CYLINDRICAL_DEFORMATION_H_
#define _PROLAND_CYLINDRICAL_DEFORMATION_H_

#include "proland/terrain/Deformation.h"

namespace proland
{

/**
 * A Deformation of space transforming planes to cylinders. This deformation
 * transforms the plane z=0 into a cylinder of radius R. The
 * deformation of p=(x,y,z) in local space is q=(x, r.sin(a), r.cos(a)),
 * where r=R-z and a=y/R.
 * @ingroup terrain
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class CylindricalDeformation : public Deformation
{
public:
    /**
     * The radius of the cylinder into which the plane z=0 must be deformed.
     */
    const float R;

    /**
     * Creates a new CylindricalDeformation.
     *
     * @param R the radius of the cylinder into which the plane z=0 must be
     *      deformed.
     */
    CylindricalDeformation(float R);

    /**
     * Deletes this CylindricalDeformation.
     */
    virtual ~CylindricalDeformation();

    virtual vec3d localToDeformed(const vec3d &localPt) const;

    virtual mat4d localToDeformedDifferential(const vec3d &localPt, bool clamp = false) const;

    virtual vec3d deformedToLocal(const vec3d &worldPt) const;

    virtual box2f deformedToLocalBounds(const vec3d &deformedCenter, double deformedRadius) const;

    virtual mat4d deformedToTangentFrame(const vec3d &deformedPt) const;

    virtual void setUniforms(ptr<SceneNode> context, ptr<TerrainNode> n, ptr<Program> prog) const;

    virtual SceneManager::visibility getVisibility(const TerrainNode *t, const box3d &localBox) const;

private:
    mutable ptr<UniformMatrix4f> localToWorldU;

    mutable ptr<Uniform1f> radiusU;

    static SceneManager::visibility getVisibility(const vec4d &clip, const vec3d b[4], float f);
};

}

#endif
