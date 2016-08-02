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

#include "proland/graph/BasicCurvePart.h"

namespace proland
{

BasicCurvePart::BasicCurvePart(CurvePtr p, int start, int end) :
    CurvePart(), curve(p), orientation(0), start(start), end(end)
{
    assert(end > start);
}

BasicCurvePart::BasicCurvePart(CurvePtr p, int orientation, int start, int end) :
    CurvePart(), curve(p), orientation(orientation), start(start), end(end)
{
    assert(end > start);
    assert(canClip(0));
    assert(canClip(end - start));
}

CurveId BasicCurvePart::getId() const
{
    return curve->getId();
}

CurveId BasicCurvePart::getParentId() const
{
    return getCurve()->getParentId();
}

CurvePtr BasicCurvePart::getCurve() const
{
    return curve;
}

int BasicCurvePart::getCurveIndex(int i) const
{
    return orientation == 0 ? start + i : end - i;
}

int BasicCurvePart::getType() const
{
    return getCurve()->getType();
}

float BasicCurvePart::getWidth() const
{
    return getCurve()->getWidth();
}

int BasicCurvePart::getEnd() const
{
    return end - start;
}

vec2d BasicCurvePart::getXY(int i) const
{
    return getCurve()->getXY(orientation == 0 ? start + i : end - i);
}

bool BasicCurvePart::getIsControl(int i) const
{
    return getCurve()->getIsControl(orientation == 0 ? start + i : end - i);
}

float BasicCurvePart::getS(int i) const
{
    return getCurve()->getS(orientation == 0 ? start + i : end - i);
}

box2d BasicCurvePart::getBounds() const
{
    return getCurve()->getBounds();
}

bool BasicCurvePart::canClip(int i) const
{
    return !getCurve()->getIsControl(orientation == 0 ? start + i : end - i);
}

CurvePart *BasicCurvePart::clip(int start, int end) const
{
    assert(start >= 0 && end > start && end <= getEnd());
    if (orientation == 0) {
        return getCurve()->getOwner()->createCurvePart(getCurve(), 0, this->start + start, this->start + end);
    } else {
        return getCurve()->getOwner()->createCurvePart(getCurve(), 1, this->end - end, this->end - start);
    }
}

}
