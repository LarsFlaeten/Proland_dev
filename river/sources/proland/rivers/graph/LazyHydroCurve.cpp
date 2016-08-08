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

#include "proland/rivers/graph/LazyHydroCurve.h"

#include "proland/rivers/graph/LazyHydroGraph.h"

namespace proland
{

LazyHydroCurve::LazyHydroCurve(Graph* owner, CurveId id) :
    HydroCurve(owner)
{
    this->id = id;
    this->startId.id = NULL_ID;
    this->endId.id = NULL_ID;
    this->parentId.id = NULL_ID;
}

LazyHydroCurve::LazyHydroCurve(Graph* owner, CurveId id, NodeId s, NodeId e) :
    HydroCurve(owner), startId(s), endId(e)
{
    this->id = id;
    this->parentId.id = NULL_ID;
    this->s0 = 0;
    this->s1 = 1;
}

LazyHydroCurve::~LazyHydroCurve()
{
    if (owner != NULL) {
        dynamic_cast<LazyGraph*>(owner)->deleteCurve(id);
    }
}

CurveId LazyHydroCurve::getId() const
{
    return id;
}

CurvePtr LazyHydroCurve::getParent() const
{
    return NULL;
}

NodePtr LazyHydroCurve::getStart() const
{
    if (startId.id == NULL_ID) {
        start = NULL;
    } else if (start == NULL) {
        start = owner->getNode(startId);
    }
    return start;
}

NodePtr LazyHydroCurve::getEnd() const
{
    if (endId.id == NULL_ID) {
        end = NULL;
    } else if (end == NULL) {
        end = owner->getNode(endId);
    }
    return end;
}

void LazyHydroCurve::clear()
{
    Curve::clear();
    startId.id = NULL_ID;
    endId.id = NULL_ID;
}

void LazyHydroCurve::doRelease()
{
    if (owner != NULL) {
        start = NULL;
        end = NULL;
        dynamic_cast<LazyGraph*>(owner)->releaseCurve(id);
    } else {
        delete this;
    }
}

void LazyHydroCurve::setParentId(CurveId id)
{
    this->parentId = id;
}

void LazyHydroCurve::addVertex(NodeId id, bool isEnd)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    if ((start == NULL && startId.id == NULL_ID)  || isEnd == 0) {
        start = NULL;
        startId = id;
    } else {
        end = NULL;
        endId = id;
    }
}

void LazyHydroCurve::loadVertex(float x, float y, float s, bool isControl)
{
    Curve::addVertex(x, y, s, isControl);
}

void LazyHydroCurve::loadVertex(NodeId id, bool isEnd)
{
    if ((start == NULL && startId.id == NULL_ID)  || isEnd == 0) {
        start = NULL;
        startId = id;
    } else {
        end = NULL;
        endId = id;
    }
}

void LazyHydroCurve::addVertex(float x, float y, float s, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(x, y, s, isControl);
}

void LazyHydroCurve::addVertex(vec2d pt, int rank, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(pt, rank, isControl);
}

void LazyHydroCurve::addVertex(const vec2d &p, float s, float l, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(p, s, l, isControl);
}

void LazyHydroCurve::addVertex(const Vertex &pt)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(pt);
}

void LazyHydroCurve::addVertices(const vector<vec2d> &v)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertices(v);
}


void LazyHydroCurve::removeVertex(int i)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::removeVertex(i);
}

void LazyHydroCurve::setIsControl(int i, bool c)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setIsControl(i, c);
}

void LazyHydroCurve::setS(int i, float s)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setS(i, s);
}

void LazyHydroCurve::setXY(int i, const vec2d &p)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setXY(i, p);
}

void LazyHydroCurve::setWidth(float width)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setWidth(width);
}

void LazyHydroCurve::setType(int type)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setType(type);
}

void LazyHydroCurve::addArea(AreaId a)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addArea(a);
}

void LazyHydroCurve::removeArea(AreaId a)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::removeArea(a);
}

void LazyHydroCurve::loadArea(AreaId a)
{
    Curve::addArea(a);
}

void LazyHydroCurve::invert()
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::invert();
    NodeId tmp = startId;
    startId = endId;
    endId = tmp;
}

}
