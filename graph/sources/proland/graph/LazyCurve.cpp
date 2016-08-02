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

#include "proland/graph/LazyCurve.h"
#include "proland/graph/LazyGraph.h"

namespace proland
{

LazyCurve::LazyCurve(Graph* owner, CurveId id) :
    Curve(owner)
{
    this->id = id;
    this->startId.id = NULL_ID;
    this->endId.id = NULL_ID;
    this->parentId.id = NULL_ID;
}

LazyCurve::LazyCurve(Graph* owner, CurveId id, NodeId s, NodeId e) :
    Curve(owner), startId(s), endId(e)
{
    this->id = id;
    this->parentId.id = NULL_ID;
    this->s0 = 0;
    this->s1 = 1;
}

LazyCurve::~LazyCurve()
{
    if (owner != NULL) {
        dynamic_cast<LazyGraph*>(owner)->deleteCurve(id);
    }
}

CurveId LazyCurve::getId() const
{
    return id;
}

CurvePtr LazyCurve::getParent() const
{
    return NULL;
}

NodePtr LazyCurve::getStart() const
{
    if (startId.id == NULL_ID) {
        start = NULL;
    } else if (start == NULL) {
        start = owner->getNode(startId);
    }
    return start;
}

NodePtr LazyCurve::getEnd() const
{
    if (endId.id == NULL_ID) {
        end = NULL;
    } else if (end == NULL) {
        end = owner->getNode(endId);
    }
    return end;
}

void LazyCurve::clear()
{
    Curve::clear();
    startId.id = NULL_ID;
    endId.id = NULL_ID;
}

void LazyCurve::doRelease()
{
    if (owner != NULL) {
        start = NULL;
        end = NULL;
        dynamic_cast<LazyGraph*>(owner)->releaseCurve(id);
    } else {
        delete this;
    }
}

void LazyCurve::setParentId(CurveId id)
{
    this->parentId = id;
}

void LazyCurve::addVertex(NodeId id, bool isEnd)
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

void LazyCurve::loadVertex(float x, float y, float s, bool isControl)
{
    Curve::addVertex(x, y, s, isControl);
}

void LazyCurve::loadVertex(NodeId id, bool isEnd)
{
    if ((start == NULL && startId.id == NULL_ID)  || isEnd == 0) {
        start = NULL;
        startId = id;
    } else {
        end = NULL;
        endId = id;
    }
}

void LazyCurve::addVertex(double x, double y, float s, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(x, y, s, isControl);
}

void LazyCurve::addVertex(vec2d pt, int rank, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(pt, rank, isControl);
}

void LazyCurve::addVertex(const vec2d &p, float s, float l, bool isControl)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(p, s, l, isControl);
}

void LazyCurve::addVertex(const Vertex &pt)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertex(pt);
}

void LazyCurve::addVertices(const vector<vec2d> &v)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addVertices(v);
}


void LazyCurve::removeVertex(int i)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::removeVertex(i);
}

void LazyCurve::setIsControl(int i, bool c)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setIsControl(i, c);
}

void LazyCurve::setS(int i, float s)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setS(i, s);
}

void LazyCurve::setXY(int i, const vec2d &p)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setXY(i, p);
}

void LazyCurve::setWidth(float width)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setWidth(width);
}

void LazyCurve::setType(int type)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::setType(type);
}

void LazyCurve::addArea(AreaId a)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::addArea(a);
}

void LazyCurve::removeArea(AreaId a)
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::removeArea(a);
}

void LazyCurve::loadArea(AreaId a)
{
    Curve::addArea(a);
}

void LazyCurve::invert()
{
    dynamic_cast<LazyGraph*>(owner)->getCurveCache()->add(this, true);
    Curve::invert();
    NodeId tmp = startId;
    startId = endId;
    endId = tmp;
}

}

