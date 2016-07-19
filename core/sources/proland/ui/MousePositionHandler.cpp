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

#include "proland/ui/MousePositionHandler.h"

#include "ork/scenegraph/ShowInfoTask.h"
#include "ork/resource/ResourceTemplate.h"

using namespace ork;

namespace proland
{

MousePositionHandler::MousePositionHandler() : EventHandler("MousePositionHandler")
{
}

MousePositionHandler::MousePositionHandler(map<ptr<SceneNode>, ptr<TerrainNode> > terrains, ptr<EventHandler> next) :
    EventHandler("MousePositionHandler")
{
    init(terrains, next);
}

void MousePositionHandler::init(map<ptr<SceneNode>, ptr<TerrainNode> > terrains, ptr<EventHandler> next)
{
    this->terrains = terrains;
    this->next = next;
    mousePosition = vec2i(0, 0);
    currentTerrain = -1;
    terrainPosition = vec3d(0.0, 0.0, 0.0);
    tile = vec3i(0, 0, 0);
}

MousePositionHandler::~MousePositionHandler()
{
    terrains.clear();
}

void MousePositionHandler::redisplay(double t, double dt)
{
    char cursor[90];
    sprintf(cursor, "Mouse coordinates: %d:%d:%f", mousePosition.x, mousePosition.y, mousePositionZ);
    ShowInfoTask::setInfo("Mouse1", cursor);
    if (currentTerrain != -1) {
        sprintf(cursor, "Terrain %d -> %10.3f:%10.3f:%10.3f (Tile:%d:%d:%d)", currentTerrain, terrainPosition.x, terrainPosition.y, terrainPosition.z, tile.x, tile.y, tile.z);
        ShowInfoTask::setInfo("Mouse2", cursor);
    }

    if (next != NULL) {
        next->redisplay(t, dt);
    }
}

void MousePositionHandler::reshape(int x, int y)
{
    if (next != NULL) {
        next->reshape(x, y);
    }
}

void MousePositionHandler::idle(bool damaged)
{
    if (next != NULL) {
        next->idle(damaged);
    }
}

bool MousePositionHandler::mouseClick(button b, state s, modifier m, int x, int y)
{
    return next != NULL && next->mouseClick(b, s, m, x, y);
}

ptr<TerrainQuad> MousePositionHandler::findTile(float x, float y, ptr<TerrainQuad> quad)
{
    if (quad->visible == SceneManager::INVISIBLE) {
        return NULL;
    }
    if (quad->isLeaf()) {
        return quad;
    } else {
        ptr<TerrainQuad> child = NULL;
        int c = -1;
        if (x > quad->ox + quad->l / 2) {
            if (y > quad->oy + quad->l / 2) {
                c = 3;
            } else {
                c = 1;
            }
        } else {
            if (y > quad->oy + quad->l / 2) {
                c = 2;
            } else {
                c = 0;
            }
        }
        child = findTile(x, y, quad->children[c]);

        return child == NULL ? quad : child;
    }
}

void MousePositionHandler::getWorldCoordinates(int x, int y)
{
    mousePosition = vec2i(x, y);
    float winx, winy, winz;
    ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
    vec4<GLint> vp = fb->getViewport();
    float width = (float) vp.z;
    float height = (float) vp.w;
    fb->readPixels(x, vp.w - y, 1, 1, DEPTH_COMPONENT, FLOAT, Buffer::Parameters(), CPUBuffer(&winz));
    mousePositionZ = winz;
    winx = (x * 2.0f) / width - 1.0f;
    winy = 1.0f - (y * 2.0f) / height;
    winz = 2.0f * winz - 1.0f;

    currentTerrain = 0;

    for (map<ptr<SceneNode>, ptr<TerrainNode> >::iterator it = terrains.begin(); it != terrains.end(); it++) {
        vec4d p = it->first->getLocalToScreen().inverse() * vec4d(winx, winy, winz, 1);
        if (isNaN(p.x + p.y +p.z +p.w)) {
            currentTerrain = -1;
            return;
        }
        box3d b = it->first->getLocalBounds();
        double px = p.x / p.w, py = p.y /p.w, pz = p.z / p.w;
        vec3d v = vec3d(px, py, pz);

        if (b.xmin > px || b.xmax < px || b.ymin > py || b.ymax < py || b.zmin > pz || b.zmax < pz) {
            currentTerrain++;
            continue;
        }
        terrainPosition = it->second->deform->deformedToLocal(v);
        ptr<TerrainQuad> quad = findTile(terrainPosition.x, terrainPosition.y, it->second->root);
        tile = vec3i(quad->level, quad->tx, quad->ty);
        return;
    }
    currentTerrain = -1;
}

bool MousePositionHandler::mouseMotion(int x, int y)
{
    getWorldCoordinates(x, y);
    return next != NULL && next->mouseMotion(x, y);
}

bool MousePositionHandler::mousePassiveMotion(int x, int y)
{
    getWorldCoordinates(x, y);
    return next != NULL && next->mousePassiveMotion(x, y);
}

bool MousePositionHandler::mouseWheel(wheel b, modifier m, int x, int y)
{
    return next != NULL && next->mouseWheel(b, m, x, y);
}

bool MousePositionHandler::keyTyped(unsigned char c, modifier m, int x, int y)
{
    return next != NULL && next->keyTyped(c, m, x, y);
}

bool MousePositionHandler::keyReleased(unsigned char c, modifier m, int x, int y)
{
    return next != NULL && next->keyReleased(c, m, x, y);
}

bool MousePositionHandler::specialKey(key k, modifier m, int x, int y)
{
    return next != NULL && next->specialKey(k, m, x, y);
}

bool MousePositionHandler::specialKeyReleased(key k, modifier m, int x, int y)
{
    return next != NULL && next->specialKeyReleased(k, m, x, y);
}

void MousePositionHandler::swap(ptr<MousePositionHandler> o)
{
    std::swap(terrains, o->terrains);
    std::swap(mousePosition, o->mousePosition);
    std::swap(currentTerrain, o->currentTerrain);
    std::swap(terrainPosition, o->terrainPosition);
    std::swap(next, o->next);
}

class MousePositionHandlerResource : public ResourceTemplate<100, MousePositionHandler>
{
public:

    MousePositionHandlerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<100, MousePositionHandler> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,terrains,next,");
        map<ptr<SceneNode>, ptr<TerrainNode> > terrains;
        ptr<EventHandler> next = manager->loadResource(getParameter(desc, e, "next")).cast<EventHandler>();

        string names = getParameter(desc, e, "terrains") + ",";
        string::size_type start = 0;
        string::size_type index;
        while ((index = names.find(',', start)) != string::npos) {
            string name = names.substr(start, index - start);
            ptr<SceneNode> n = manager->loadResource(name).cast<SceneNode>();

            terrains.insert(make_pair(n, n->getField("terrain").cast<TerrainNode>()));
            start = index + 1;
        }

        init(terrains, next);
    }
};

extern const char mousePositionHandler[] = "mousePositionHandler";

static ResourceFactory::Type<mousePositionHandler, MousePositionHandlerResource> MousePositionHandlerType;

}
