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

#include "proland/terrain/CylindricalDeformation.h"

#include "ork/render/Program.h"
#include "proland/terrain/TerrainNode.h"

namespace proland
{

CylindricalDeformation::CylindricalDeformation(float R) :
    Deformation(), R(R), localToWorldU(NULL), radiusU(NULL)
{
}

CylindricalDeformation::~CylindricalDeformation()
{
}

vec3d CylindricalDeformation::localToDeformed(const vec3d &localPt) const
{
    float alpha = (float) localPt.y / R;
    float r = R - (float) localPt.z;
    return vec3d(localPt.x, r * sin(alpha), -r * cos(alpha));
}

mat4d CylindricalDeformation::localToDeformedDifferential(const vec3d &localPt, bool clamp) const
{
    float alpha = (float) localPt.y / R;
    return mat4d(1.0, 0.0, 0.0, localPt.x,
        0.0, cos(alpha), -sin(alpha), R * sin(alpha),
        0.0, sin(alpha), cos(alpha), - R * cos(alpha),
        0.0, 0.0, 0.0, 1.0);
}

vec3d CylindricalDeformation::deformedToLocal(const vec3d &deformedPt) const
{
    float y = R * atan2((float) deformedPt.y, (float) -deformedPt.z);
    float z = R - sqrt((float) (deformedPt.y * deformedPt.y + deformedPt.z * deformedPt.z));
    return vec3d(deformedPt.x, y, z);
}

box2f CylindricalDeformation::deformedToLocalBounds(const vec3d &deformedCenter, double deformedRadius) const
{
    assert(false); // TODO
    return box2f();
}

mat4d CylindricalDeformation::deformedToTangentFrame(const vec3d &deformedPt) const
{
    vec3d Uz = vec3d(0.0, -deformedPt.y, -deformedPt.z).normalize();
    vec3d Ux = vec3d::UNIT_X;
    vec3d Uy = Uz.crossProduct(Ux);
    vec3d O = vec3d(deformedPt.x, -Uz.y * R, -Uz.z * R);
    return mat4d(Ux.x, Ux.y, Ux.z, -O.dotproduct(Ux),
        Uy.x, Uy.y, Uy.z, -O.dotproduct(Uy),
        Uz.x, Uz.y, Uz.z, -O.dotproduct(Uz),
        0.0, 0.0, 0.0, 1.0);
}

void CylindricalDeformation::setUniforms(ptr<SceneNode> context, ptr<TerrainNode> n, ptr<Program> prog) const
{
    if (lastNodeProg != prog) {
        localToWorldU = prog->getUniformMatrix4f("deformation.localToWorld");
        radiusU = prog->getUniform1f("deformation.radius");
    }

    if (localToWorldU != NULL) {
        mat4d ltow = context->getLocalToWorld();
        localToWorldU->setMatrix(ltow.cast<float>());
    }

    Deformation::setUniforms(context, n, prog);

    if (radiusU != NULL) {
        radiusU->set(R);
    }
}

SceneManager::visibility CylindricalDeformation::getVisibility(const TerrainNode *t, const box3d &localBox) const
{
    vec3d deformedBox[4];
    deformedBox[0] = localToDeformed(vec3d(localBox.xmin, localBox.ymin, localBox.zmax));
    deformedBox[1] = localToDeformed(vec3d(localBox.xmax, localBox.ymin, localBox.zmax));
    deformedBox[2] = localToDeformed(vec3d(localBox.xmax, localBox.ymax, localBox.zmax));
    deformedBox[3] = localToDeformed(vec3d(localBox.xmin, localBox.ymax, localBox.zmax));
    double dy = localBox.ymax - localBox.ymin;
    float f = (float) ((R - localBox.zmin) / ((R - localBox.zmax) * cos(dy / (2.0 * R))));

    const vec4d *deformedFrustumPlanes = t->getDeformedFrustumPlanes();
    SceneManager::visibility v0 = getVisibility(deformedFrustumPlanes[0], deformedBox, f);
    if (v0 == SceneManager::INVISIBLE) {
        return SceneManager::INVISIBLE;
    }
    SceneManager::visibility v1 = getVisibility(deformedFrustumPlanes[1], deformedBox, f);
    if (v1 == SceneManager::INVISIBLE) {
        return SceneManager::INVISIBLE;
    }
    SceneManager::visibility v2 = getVisibility(deformedFrustumPlanes[2], deformedBox, f);
    if (v2 == SceneManager::INVISIBLE) {
        return SceneManager::INVISIBLE;
    }
    SceneManager::visibility v3 = getVisibility(deformedFrustumPlanes[3], deformedBox, f);
    if (v3 == SceneManager::INVISIBLE) {
        return SceneManager::INVISIBLE;
    }
    SceneManager::visibility v4 = getVisibility(deformedFrustumPlanes[4], deformedBox, f);
    if (v4 == SceneManager::INVISIBLE) {
        return SceneManager::INVISIBLE;
    }
    if (v0 == SceneManager::FULLY_VISIBLE && v1 == SceneManager::FULLY_VISIBLE &&
        v2 == SceneManager::FULLY_VISIBLE && v3 == SceneManager::FULLY_VISIBLE &&
        v4 == SceneManager::FULLY_VISIBLE)
    {
        return SceneManager::FULLY_VISIBLE;
    }
    return SceneManager::PARTIALLY_VISIBLE;
}

SceneManager::visibility CylindricalDeformation::getVisibility(const vec4d &clip, const vec3d b[4], float f)
{
    double c1 = b[0].x * clip.x + clip.w;
    double c2 = b[1].x * clip.x + clip.w;
    double c3 = b[2].x * clip.x + clip.w;
    double c4 = b[3].x * clip.x + clip.w;
    double o1 = b[0].y * clip.y + b[0].z * clip.z;
    double o2 = b[1].y * clip.y + b[1].z * clip.z;
    double o3 = b[2].y * clip.y + b[2].z * clip.z;
    double o4 = b[3].y * clip.y + b[3].z * clip.z;
    double p1 = o1 + c1;
    double p2 = o2 + c2;
    double p3 = o3 + c3;
    double p4 = o4 + c4;
    double p5 = o1 * f + c1;
    double p6 = o2 * f + c2;
    double p7 = o3 * f + c3;
    double p8 = o4 * f + c4;
    if (p1 <= 0 && p2 <= 0 && p3 <= 0 && p4 <= 0 && p5 <= 0 && p6 <= 0 && p7 <= 0 && p8 <= 0) {
        return SceneManager::INVISIBLE;
    }
    if (p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0 && p6 > 0 && p7 > 0 && p8 > 0) {
        return SceneManager::FULLY_VISIBLE;
    }
    return SceneManager::PARTIALLY_VISIBLE;
}

}
