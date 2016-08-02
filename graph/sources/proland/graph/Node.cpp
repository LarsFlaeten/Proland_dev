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

#include "proland/graph/Node.h"

#include "proland/graph/Curve.h"

namespace proland
{


Node::Node(Graph* owner, double x, double y) :
    Object("Node"), owner(owner), pos(x, y)
{
}

Node::~Node()
{
    curves.clear();
}

NodeId Node::getId() const
{
    NodeId i;
    i.ref = (Node*) this;
    return i;
}

int Node::getCurveCount() const
{
    return curves.size();
}

int Node::getCurveCount(const set<CurveId> &includedCurves) const
{
    int total = 0;
    for (int i = 0; i < getCurveCount(); ++i) {
        if (includedCurves.find(curves[i]) != includedCurves.end()) {
            ++total;
        }
    }
    return total;
}

CurvePtr Node::getCurve(int i) const
{
    return owner->getCurve(curves[i]);
}

CurvePtr Node::getCurve(const NodePtr n) const
{
    CurvePtr result = NULL;
    for (int i = 0; i < getCurveCount(); ++i) {
        CurvePtr c = getCurve(i);
        if (c->getOpposite((Node*) this) == n) {
            assert(result == NULL);
            result = c;
        }
    }
    return result;
}

NodePtr Node::getOpposite(const NodePtr n) const
{
    assert(getCurveCount() == 2);
    CurvePtr c0 = getCurve(0);
    CurvePtr c1 = getCurve(1);
    NodePtr c0a = c0->getStart();
    NodePtr c0b = c0->getEnd();
    NodePtr c1a = c1->getStart();
    NodePtr c1b = c1->getEnd();
    if (c0a != this && c0a != n) {
        return c0a;
    } else if (c0b != this && c0b != n) {
        return c0b;
    } else if (c1a != this && c1a != n) {
        return c1a;
    } else if (c1b != this && c1b != n) {
        return c1b;
    }
    return NULL;
}

void Node::setPos(const vec2d &p)
{
    pos = p;
    for (int i = 0; i < getCurveCount(); ++i) {
        getCurve(i)->resetBounds();
    }
}

void Node::addCurve(CurveId c)
{
    if (find(curves.begin(), curves.end(), c) == curves.end()) {
        curves.push_back(c);
    }
}

void Node::removeCurve(CurveId c)
{
    vector<CurveId>::iterator i = find(curves.begin(), curves.end(), c);
    if (i != curves.end()) {
        curves.erase(i);
    }
}

void Node::setOwner(Graph* owner)
{
    this->owner = owner;
}

}
