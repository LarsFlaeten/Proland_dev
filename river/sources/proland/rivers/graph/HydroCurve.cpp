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

#include "proland/rivers/graph/HydroCurve.h"

#include "proland/math/seg2.h"
#include "proland/rivers/graph/HydroGraph.h"

using namespace ork;

namespace proland
{

HydroCurve::HydroCurve(Graph *owner) : Curve(owner), potential(-1.f)
{
    river.id = NULL_ID;
}

HydroCurve::HydroCurve(Graph *owner, CurvePtr c, NodePtr s, NodePtr e) :
    Curve(owner, c, s ,e)
{
    if (c != NULL) {
        ptr<HydroCurve> h = c.cast<HydroCurve>();
        assert(h != NULL);
        setRiver(h->getRiver());
        potential = h->getPotential();
    } else {
        river.id = NULL_ID;
        potential = -1.f;
    }
}

HydroCurve::~HydroCurve()
{
}

float HydroCurve::getWidth() const
{
    if (river.id != NULL_ID) {
        return getRiverPtr()->getWidth();
    }

    return width;
}

float HydroCurve::getPotential() const
{
    return potential;
}

void HydroCurve::setPotential(float potential)
{
    this->potential = potential;
}

CurveId HydroCurve::getRiver() const
{
    return river;
}

CurvePtr HydroCurve::getRiverPtr() const
{
    return getOwner()->getAncestor()->getCurve(river);
}

void HydroCurve::setRiver(CurveId river)
{
    this->river = river;
    if (river.id != NULL_ID) {
        this->width = getRiverPtr()->getWidth();
    }
}

void HydroCurve::print() const
{
    printf("%d-> %d %f %d %f %d\n", getId().id, getSize(), getWidth(), getType(), getPotential(), getRiver().id);
    vec2d v;
    for (int i = 0; i < getSize(); i++) {
        v = getXY(i);
        printf("%f %f %d %f %f\n", v.x, v.y, getIsControl(i), getS(i), getL(i));
    }
}

}
