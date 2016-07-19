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

#include "proland/util/TerrainViewController.h"

#include "ork/render/FrameBuffer.h"
#include "proland/terrain/TerrainNode.h"

using namespace std;
using namespace ork;

namespace proland
{

TerrainViewController::TerrainViewController(ptr<SceneNode> node, double d0) :
    Object("TerrainViewController"),
    fov(80.0), x0(0.0), y0(0.0), theta(0.0), phi(0.0), d(d0), zoom(1.0),
    node(node), groundHeight(0.0)
{
}

TerrainViewController::~TerrainViewController()
{
}

ptr<SceneNode> TerrainViewController::getNode()
{
    return node;
}

void TerrainViewController::setNode(ptr<SceneNode> node)
{
    this->node = node;
}

float TerrainViewController::getGroundHeight()
{
    return groundHeight;
}

void TerrainViewController::setGroundHeight(float groundHeight)
{
    this->groundHeight = groundHeight;
}

double TerrainViewController::getHeight()
{
    return position.z;
}

void TerrainViewController::move(vec3d &oldp, vec3d &p)
{
    x0 -= p.x - oldp.x;
    y0 -= p.y - oldp.y;
}

void TerrainViewController::moveForward(double distance)
{
    x0 -= sin(phi) * distance;
    y0 += cos(phi) * distance;
}

void TerrainViewController::turn(double angle)
{
    double l = d * sin(theta);
    x0 -= (sin(phi) * (cos(angle) - 1.0) + cos(phi) * sin(angle)) * l;
    y0 += (cos(phi) * (cos(angle) - 1.0) - sin(phi) * sin(angle)) * l;
    phi += angle;
}

double TerrainViewController::interpolate(double sx0, double sy0, double stheta, double sphi, double sd,
            double dx0, double dy0, double dtheta, double dphi, double dd, double t)
{
    // TODO interpolation
    x0 = dx0;
    y0 = dy0;
    theta = dtheta;
    phi = dphi;
    d = dd;
    return 1.0;
}

void TerrainViewController::interpolatePos(double sx0, double sy0, double dx0, double dy0, double t, double &x0, double &y0)
{
    x0 = sx0 * (1.0 - t) + dx0 * t;
    y0 = sy0 * (1.0 - t) + dy0 * t;
}

void TerrainViewController::interpolateDirection(double slon, double slat, double elon, double elat, double t, double &lon, double &lat)
{
    vec3d s = vec3d(cos(slon) * cos(slat), sin(slon) * cos(slat), sin(slat));
    vec3d e = vec3d(cos(elon) * cos(elat), sin(elon) * cos(elat), sin(elat));
    vec3d v = (s * (1.0 - t) + e * t).normalize();
    lat = safe_asin(v.z);
    lon = atan2(v.y, v.x);
}

void TerrainViewController::update()
{
    vec3d po = vec3d(x0, y0, groundHeight);
    vec3d px = vec3d(1.0, 0.0, 0.0);
    vec3d py = vec3d(0.0, 1.0, 0.0);
    vec3d pz = vec3d(0.0, 0.0, 1.0);

    double ct = cos(theta);
    double st = sin(theta);
    double cp = cos(phi);
    double sp = sin(phi);
    vec3d cx = px * cp + py * sp;
    vec3d cy = -px * sp*ct + py * cp*ct + pz * st;
    vec3d cz = px * sp*st - py * cp*st + pz * ct;
    position = po + cz * d * zoom;

    if (position.z < groundHeight + 1.0) {
        position.z = groundHeight + 1.0;
    }

    mat4d view(cx.x, cx.y, cx.z, 0.0,
            cy.x, cy.y, cy.z, 0.0,
            cz.x, cz.y, cz.z, 0.0,
            0.0, 0.0, 0.0, 1.0);

    view = view * mat4d::translate(-position);

    node->setLocalToParent(view.inverse());
}

void TerrainViewController::setProjection(float znear, float zfar, vec4f viewport)
{
    vec4i vp = SceneManager::getCurrentFrameBuffer()->getViewport();
    float width = (float) vp.z;
    float height = (float) vp.w;
    float vfov = degrees(2 * atan(height / width * tan(radians(fov / 2))));

    float h = (float) (getHeight() - TerrainNode::groundHeightAtCamera);
    if (znear == 0.0f) {
        znear = 0.1f * h;
    }
    if (zfar == 0.0f) {
        zfar = 1e6f * h;
    }

    if (zoom > 1.0) {
        vfov = degrees(2 * atan(height / width * tan(radians(fov / 2)) / zoom));
        znear = d * zoom * max(1.0 - 10.0 * tan(radians(fov / 2)) / zoom, 0.1);
        zfar = d * zoom * min(1.0 + 10.0 * tan(radians(fov / 2)) / zoom, 10.0);
    }

    mat4d clip = mat4d::orthoProjection(viewport.y, viewport.x, viewport.w, viewport.z, 1.0f, -1.0f);
    mat4d cameraToScreen = mat4d::perspectiveProjection(vfov, width / height, znear, zfar);
    node->getOwner()->setCameraToScreen(clip * cameraToScreen);
}

}
