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

#include "proland/util/CylinderViewController.h"

namespace proland
{

CylinderViewController::CylinderViewController(ptr<SceneNode> node, double R) :
    TerrainViewController(node, R * 0.9), R(R)
{
}

CylinderViewController::~CylinderViewController()
{
}

double CylinderViewController::getHeight()
{
    return R - sqrt(position.y * position.y + position.z * position.z);
}

void CylinderViewController::move(vec3d &oldp, vec3d &p)
{
    double oldlon = atan2(oldp.z, oldp.y);
    double lon = atan2(p.z, p.y);
    x0 -= p.x - oldp.x;
    y0 -= lon - oldlon;
}

void CylinderViewController::update()
{
    double ca = cos(y0);
    double sa = sin(y0);
    vec3d po = vec3d(x0, sa * (R - groundHeight), -ca * (R - groundHeight));
    vec3d px = vec3d(1.0, 0.0, 0.0);
    vec3d py = vec3d(0.0, ca, sa);
    vec3d pz = vec3d(0.0, -sa, ca);

    double ct = cos(theta);
    double st = sin(theta);
    double cp = cos(phi);
    double sp = sin(phi);
    vec3d cx = px * cp + py * sp;
    vec3d cy = -px * sp*ct + py * cp*ct + pz * st;
    vec3d cz = px * sp*st - py * cp*st + pz * ct;
    position = po + cz * d * zoom;

    double l = sqrt(position.y * position.y + position.z * position.z);
    if (l > R - 1.0 - groundHeight) {
        position.y = position.y * (R - 1.0 - groundHeight) / l;
        position.z = position.z * (R - 1.0 - groundHeight) / l;
    }

    mat4d view(cx.x, cx.y, cx.z, 0.0,
            cy.x, cy.y, cy.z, 0.0,
            cz.x, cz.y, cz.z, 0.0,
            0.0, 0.0, 0.0, 1.0);

    view = view * mat4d::translate(-position);

    node->setLocalToParent(view.inverse());
}

}
