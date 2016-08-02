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

#include "proland/graph/LazyArea.h"

namespace proland
{

LazyArea::LazyArea(Graph* owner, AreaId id) :
    Area(owner)
{
    parentId.id = NULL_ID;
    this->id = id;
}

LazyArea::~LazyArea()
{
    if (owner != NULL) {
        dynamic_cast<LazyGraph*>(owner)->deleteArea(id);
    }
}

AreaId LazyArea::getId() const
{
    return id;
}

AreaPtr LazyArea::getParent() const
{
    return NULL;
    //if (parent == NULL) {
    //    parent = owner->getArea(parentId);
    //}
    //return parent;
}

CurvePtr LazyArea::getCurve(int i) const
{
    if (curves[i].first == NULL) {
        curves[i] = pair<CurvePtr, int>(owner->getCurve(curveIds[i].first), curveIds[i].second);
    }
    return curves[i].first;
}

CurvePtr LazyArea::getCurve(int i, int& orientation) const
{
    if (curves[i].first == NULL) {
        curves[i] = pair<CurvePtr, int>(owner->getCurve(curveIds[i].first), curveIds[i].second);
    }
    orientation = curveIds[i].second;
    return curves[i].first;
}

int LazyArea::getCurveCount() const
{
    return curveIds.size();
}

void LazyArea::setOrientation(int i, int orientation)
{
    dynamic_cast<LazyGraph*>(owner)->getAreaCache()->add(this, true);
    curveIds[i].second = orientation;
    //curves[i].second = orientation;
}

void LazyArea::invertCurve(CurveId cid)
{
    dynamic_cast<LazyGraph*>(owner)->getAreaCache()->add(this, true);
    vector< pair<CurveId, int> >::iterator it = curveIds.begin();
    while (it != curveIds.end()) {
        if (it->first == cid) {
            break;
        }
        it++;
    }
    assert(it != curveIds.end());
    it->second = it->second == 0;
    Area::invertCurve(cid);
}

void LazyArea::loadCurve(CurveId id, int orientation)
{
    curveIds.push_back(make_pair<CurveId, int>(id, orientation));
    curves.push_back(make_pair<CurvePtr, int>(NULL, 0));
}

void LazyArea::addCurve(CurveId id, int orientation)
{
    dynamic_cast<LazyGraph*>(owner)->getAreaCache()->add(this, true);
    curveIds.push_back(make_pair<CurveId, int>(id, orientation));
    curves.push_back(make_pair<CurvePtr, int>((CurvePtr) NULL, 0));
}

void LazyArea::switchCurves(int curve1, int curve2)
{
    dynamic_cast<LazyGraph*>(owner)->getAreaCache()->add(this, true);
    pair<CurveId, int> cq = curveIds[curve1];
    curveIds[curve1] = curveIds[curve2];
    curveIds[curve2] = cq;
    if (curves[curve1].first != NULL || curves[curve2].first != NULL) {
        pair<CurvePtr, int> cq = curves[curve1];
        curves[curve1] = curves[curve2];
        curves[curve2] = cq;
    }
}

void LazyArea::removeCurve(int index)
{
    dynamic_cast<LazyGraph*>(owner)->getAreaCache()->add(this, true);
    curves.erase(curves.begin() + index);
    curveIds.erase(curveIds.begin() + index);
}

void LazyArea::doRelease()
{
    if (owner != NULL) {
        dynamic_cast<LazyGraph*>(owner)->releaseArea(id);
        for (int i = 0; i < (int) curves.size(); i++) {
            curves[i] = pair<CurvePtr, int>((CurvePtr)NULL, 0);
        }
    } else {
        delete this;
    }
}

void LazyArea::setParentId(AreaId id)
{
    this->parentId = id;
}

}
