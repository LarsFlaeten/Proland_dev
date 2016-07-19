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

#include "proland/terrain/TerrainQuad.h"

#include <algorithm>

#include "proland/terrain/TerrainNode.h"

using namespace std;

namespace proland
{

TerrainQuad::TerrainQuad(TerrainNode *owner, const TerrainQuad *parent,
    int tx, int ty, double ox, double oy, double l, float zmin, float zmax) :
    Object("TerrainQuad"), parent(parent), level(parent == NULL ? 0 : parent->level + 1), tx(tx), ty(ty),
    ox(ox), oy(oy), l(l), zmin(zmin), zmax(zmax), occluded(false), drawable(true), owner(owner)
{
}

TerrainQuad::~TerrainQuad()
{
}

TerrainNode *TerrainQuad::getOwner()
{
    return owner;
}

bool TerrainQuad::isLeaf() const
{
    return children[0] == NULL;
}

int TerrainQuad::getSize() const
{
    int s = 1;
    if (isLeaf()) {
        return s;
    } else {
        return s + children[0]->getSize() + children[1]->getSize() +
            children[2]->getSize() + children[3]->getSize();
    }
}

int TerrainQuad::getDepth() const
{
    if (isLeaf()) {
        return level;
    } else {
        return max(max(children[0]->getDepth(), children[1]->getDepth()),
            max(children[2]->getDepth(), children[3]->getDepth()));
    }
}

void TerrainQuad::update()
{
    SceneManager::visibility v = parent == NULL ? SceneManager::PARTIALLY_VISIBLE : parent->visible;
    if (v == SceneManager::PARTIALLY_VISIBLE) {
        box3d localBox(ox, ox + l, oy, oy + l, zmin, zmax);
        visible = owner->getVisibility(localBox);
    } else {
        visible = v;
    }

    // here we reuse the occlusion test from the previous frame:
    // if the quad was found unoccluded in the previous frame, we suppose it is
    // still unoccluded at this frame. If it was found occluded, we perform
    // an occlusion test to check if it is still occluded.
    if (visible != SceneManager::INVISIBLE && occluded) {
        occluded = owner->isOccluded(box3d(ox, ox + l, oy, oy + l, zmin, zmax));
        if (occluded) {
            visible = SceneManager::INVISIBLE;
        }
    }

    double ground = TerrainNode::groundHeightAtCamera;
    float dist = owner->getCameraDist(box3d(ox, ox + l, oy, oy + l, min(0.0, ground), max(0.0, ground)));

    if ((owner->splitInvisibleQuads || visible != SceneManager::INVISIBLE) && dist < l * owner->getSplitDistance() && level < owner->maxLevel) {
        if (isLeaf()) {
            subdivide();
        }

        int order[4];
        double ox = owner->getLocalCamera().x;
        double oy = owner->getLocalCamera().y;
        double cx = this->ox + l / 2.0;
        double cy = this->oy + l / 2.0;
        if (oy < cy) {
            if (ox < cx) {
                order[0] = 0;
                order[1] = 1;
                order[2] = 2;
                order[3] = 3;
            } else {
                order[0] = 1;
                order[1] = 0;
                order[2] = 3;
                order[3] = 2;
            }
        } else {
            if (ox < cx) {
                order[0] = 2;
                order[1] = 0;
                order[2] = 3;
                order[3] = 1;
            } else {
                order[0] = 3;
                order[1] = 1;
                order[2] = 2;
                order[3] = 0;
            }
        }

        children[order[0]]->update();
        children[order[1]]->update();
        children[order[2]]->update();
        children[order[3]]->update();

        // we compute a more precise occlusion for the next frame (see above),
        // by combining the occlusion status of the child nodes
        occluded = children[0]->occluded && children[1]->occluded && children[2]->occluded && children[3]->occluded;
    } else {
        if (visible != SceneManager::INVISIBLE) {
            // we add the bounding box of this quad to the occluders list
            occluded = owner->addOccluder(box3d(ox, ox + l, oy, oy + l, zmin, zmax));
            if (occluded) {
                visible = SceneManager::INVISIBLE;
            }
        }
        if (!isLeaf()) {
            children[0] = NULL;
            children[1] = NULL;
            children[2] = NULL;
            children[3] = NULL;
        }
    }
}

void TerrainQuad::subdivide()
{
    float hl = (float) l / 2.0f;
    children[0] = new TerrainQuad(owner, this, 2 * tx, 2 * ty, ox, oy, hl, zmin, zmax);
    children[1] = new TerrainQuad(owner, this, 2 * tx + 1, 2 * ty, ox + hl, oy, hl, zmin, zmax);
    children[2] = new TerrainQuad(owner, this, 2 * tx, 2 * ty + 1, ox, oy + hl, hl, zmin, zmax);
    children[3] = new TerrainQuad(owner, this, 2 * tx + 1, 2 * ty + 1, ox + hl, oy + hl, hl, zmin, zmax);
}

}
