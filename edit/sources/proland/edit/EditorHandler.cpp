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

#include "proland/edit/EditorHandler.h"

#include <algorithm>

#include <ork/render/FrameBuffer.h>

using namespace ork;

namespace proland
{

Editor::Editor(string name, string group, bool active)
{
    this->name = name;
    this->group = group;
    this->active = active;
}

Editor::~Editor()
{
}

bool Editor::isActive() {
    return active;
}

void Editor::setActive(bool active)
{
    this->active = active;
}

string Editor::getName()
{
    return name;
}

void Editor::setName(string name)
{
    this->name = name;
}

string Editor::getGroup()
{
    return group;
}

void Editor::setGroup(string group)
{
    this->group= group;
}

void Editor::swap(Editor *e)
{
    std::swap(name, e->name);
    std::swap(group, e->group);
    std::swap(active, e->active);
}

EditorHandler::EditorHandler(float relativeRadius) :
    EventHandler("EditorHandler"), relativeRadius(relativeRadius), update(false), paint(false), newStrokes(0), depthBuffer(NULL)
{
}

bool EditorHandler::hasEditors()
{
    return !editors.empty();
}

int EditorHandler::getEditorCount()
{
    return (int) editors.size();
}

Editor* EditorHandler::getEditor(int index)
{
    return editors[index];
}

void EditorHandler::addEditor(Editor *editor)
{
    vec4f v = editor->getBrushColor();
    brushColor[0] = v.x;
    brushColor[1] = v.y;
    brushColor[2] = v.z;
    brushColor[3] = v.w;

    editors.push_back(editor);
}

void EditorHandler::removeEditor(Editor *editor)
{
    vector<Editor*>::iterator i = find(editors.begin(), editors.end(), editor);
    if (i != editors.end()) {
        editors.erase(i);
    }
}

void EditorHandler::setActive(bool active)
{
    if (!active) {
        if (paint) {
            if (depthBuffer != NULL) {
                delete[] depthBuffer;
                depthBuffer = NULL;
            }
            strokes.clear();
            paint = false;
            for (unsigned int i = 0; i < editors.size(); ++i) {
                if (editors[i]->isActive()) {
                    editors[i]->update();
                }
            }
        }
        for (unsigned int i = 0; i < editors.size(); ++i) {
            editors[i]->setPencil(vec4f(0.0f ,0.0f, 0.0f, -1.0f), vec4f(brushColor[0], brushColor[1], brushColor[2], brushColor[3]), false);
        }
    }
}

void EditorHandler::setActive(string group, bool active)
{
    for (unsigned int i = 0; i < editors.size(); ++i) {
        if (editors[i]->getGroup().compare(group) == 0) {
            editors[i]->setActive(active);
        }
    }
}

bool EditorHandler::isActive(string group)
{
    for (unsigned int i = 0; i < editors.size(); ++i) {
        if (editors[i]->getGroup().compare(group) == 0 && editors[i]->isActive() == false) {
            return false;
        }
    }
    return true;
}

void EditorHandler::reset()
{
    for (unsigned int i = 0; i < editors.size(); ++i) {
        if (editors[i]->isActive()) {
            editors[i]->reset();
        }
    }
}

void EditorHandler::redisplay(double t, double dt)
{
    if (editors.empty()) {
        return;
    }

    if (update) {
        for (unsigned int i = 0; i < editors.size(); ++i) {
            if (editors[i]->isActive()) {
                editors[i]->update();
            }
        }
        update = false;
    }
    // mouse in world space
    vec3d p = getPosition(lastPos.x, lastPos.y);

    // camera in local space, altitude in local space
    float altitude = -1.0f;
    for (unsigned int i = 0; i < editors.size(); ++i) {
        if (editors[i]->isActive()) {
            mat4d screenToLocal = editors[i]->getTerrain()->getLocalToScreen().inverse();
            vec4d c = screenToLocal * vec4d(0.0, 0.0, 1.0, 0.0);
            double cx = c.x / c.w;
            double cy = c.y / c.w;
            double cz = c.z / c.w;
            if (isFinite(cx) && isFinite(cy)) {
                vec3d dv = editors[i]->getTerrainNode()->deform->deformedToLocal(vec3d(cx, cy, cz));
                if (isFinite(dv.z)) {
                    altitude = dv.z;
                    break;
                }
            }
        }
    }

    // field of view angle
    vec4d frustum[6];
    ptr<SceneManager> manager = editors[0]->getTerrain()->getOwner();
    SceneManager::getFrustumPlanes(manager->getCameraToScreen(), frustum);
    vec3d left = frustum[0].xyz().normalize();
    vec3d right = frustum[1].xyz().normalize();
    float fov = (float) safe_acos(-left.dotproduct(right));

    // pencil radius
    radius = altitude * tan(fov / 2.0) * relativeRadius;

    for (unsigned int i = 0; i < editors.size(); ++i) {
        if (editors[i]->isActive()) {
            editors[i]->setPencil(vec4f(p.x, p.y, p.z, radius), vec4f(brushColor[0], brushColor[1], brushColor[2], brushColor[3]), paint);
        }
    }
    if (newStrokes > 0) {
        for (unsigned int i = 0; i < editors.size(); ++i) {
            if (editors[i]->isActive()) {
                editors[i]->edit(strokes);
            }
        }
    }
    newStrokes = 0;
}

bool EditorHandler::mouseClick(button b, state s, modifier m, int x, int y)
{
    if (editors.empty()) {
        return false;
    }
    if (b == LEFT_BUTTON && (m & SHIFT) != 0 && s == DOWN) {
        ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
        vec4<GLint> vp = fb->getViewport();
        depthBuffer = new float[vp.z * vp.w];
        fb->readPixels(vp.x, vp.y, vp.z, vp.w, DEPTH_COMPONENT, FLOAT, Buffer::Parameters(), CPUBuffer(depthBuffer));

        paint = true;
        vec3d p = getPosition(x, y);
        strokes.clear();
        strokes.push_back(vec4d(p.x, p.y, p.z, radius));
        newStrokes++;
        return true;
    }
    if (b == LEFT_BUTTON && s == UP && paint) {
        update = true;
        if (depthBuffer != NULL) {
            delete[] depthBuffer;
            depthBuffer = NULL;
        }
        strokes.clear();
        paint = false;
    }
    return false;
}

bool EditorHandler::mouseWheel(wheel b, modifier m, int x, int y)
{
    if (editors.empty()) {
        return false;
    }
    if (b == EventHandler::WHEEL_UP && (m & EventHandler::SHIFT) != 0) {
        relativeRadius /= 1.1f;
        return true;
    }
    if (b == EventHandler::WHEEL_DOWN && (m & EventHandler::SHIFT) != 0) {
        relativeRadius *= 1.1f;
        return true;
    }
    return false;
}

bool EditorHandler::mouseMotion(int x, int y)
{
    if (editors.empty()) {
        return false;
    }
    lastPos = vec2i(x, y);
    if (paint) {
        vec4d l = strokes[strokes.size() - 1];
        vec3d p = getPosition(x, y);
        if ((p - vec3d(l.x, l.y, l.z)).length() > radius * 0.2) {
            strokes.push_back(vec4d(p.x, p.y, p.z, radius));
            newStrokes++;
        }
        return true;
    } else {
        redisplay(0.0, 0.0);
    }
    return false;
}

bool EditorHandler::mousePassiveMotion(int x, int y)
{
    if (editors.empty()) {
        return false;
    }
    lastPos = vec2i(x, y);
    return false;
}

vec3d EditorHandler::getPosition(int x, int y)
{
    float winx, winy, winz;

    ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
    vec4<GLint> vp = fb->getViewport();
    float width = (float) vp.z;
    float height = (float) vp.w;
    if (depthBuffer == NULL) {
        fb->readPixels(x, vp.w - y, 1, 1, DEPTH_COMPONENT, FLOAT, Buffer::Parameters(), CPUBuffer(&winz));
    } else {
        winz = depthBuffer[x + (vp.w - y) * vp.z];
    }

    winx = (x * 2.0f) / width - 1.0f;
    winy = 1.0f - (y * 2.0f) / height;
    winz = 2.0f * winz - 1.0f;

    ptr<SceneManager> manager = editors[0]->getTerrain()->getOwner();
    mat4d screenToWorld = manager->getWorldToScreen().inverse();
    vec4d p = screenToWorld * vec4d(winx, winy, winz, 1);

    double px = p.x / p.w;
    double py = p.y / p.w;
    double pz = p.z / p.w;
    return vec3d(px, py, pz);
}

}
