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

#include "proland/graph/CurvePart.h"

#include "proland/math/geometry.h"

namespace proland
{

CurvePart::CurvePart()
{
}

CurvePart::~CurvePart()
{
}

CurveId CurvePart::getId() const
{
    CurveId i;
    i.id = NULL_ID;
    return i;
}

CurveId CurvePart::getParentId() const
{
    CurveId i;
    i.id = NULL_ID;
    return i;
}

int CurvePart::getType() const
{
    return 0;
}

float CurvePart::getWidth() const
{
    return -1;
}

CurvePtr CurvePart::getCurve() const
{
    return NULL;
}

vec2d CurvePart::getXY(const vec2d &start, int offset) const
{
    assert(start == getXY(0) || start == getXY(getEnd()));
    return getXY(start == getXY(0) ? offset : getEnd() - offset);
}

bool CurvePart::getIsControl(int i) const
{
    return false;
}

bool CurvePart::canClip(int i) const
{
    return true;
}

void CurvePart::clip(const box2d &clip, vector<CurvePart*> &result) const
{
    int start = -1;
    int cur = 0;
    while (cur < getEnd()) {
        int c = cur;
        vec2d p0 = getXY(cur);
        vec2d p1 = getXY(++cur);
        vec2d p2;
        vec2d p3;
        bool intersect;
        assert(canClip(cur - 1));
        if (!canClip(cur)) {
            p2 = getXY(++cur);
            if (!canClip(cur)) {
                p3 = getXY(++cur);
                assert(canClip(cur));
                intersect = clipCubic(clip, p0, p1, p2, p3);
            } else {
                intersect = clipQuad(clip, p0, p1, p2);
            }
        } else {
            intersect = clipSegment(clip, p0, p1);
        }

        if (intersect) {
            if (start == -1) {
                start = c;
            }
        } else {
            if (start != -1) {
                result.push_back(this->clip(start, c));
            }
            start = -1;
        }
    }
    if (start != -1) {
        result.push_back(this->clip(start, getEnd()));
    }
}

void CurvePart::clip(const vector<CurvePart*> &paths, const box2d &clip, vector<CurvePart*> &result)
{
    for (int i = 0; i < (int) paths.size(); ++i) {
        paths[i]->clip(clip, result);
    }
}

bool CurvePart::equals(CurvePtr c) const
{
    int n = getEnd();
    if (n != c->getSize() - 1) {
        return false;
    }
    for (int i = 0; i <= n; ++i) {
        if (getXY(i) != c->getXY(i) || getIsControl(i) != c->getIsControl(i)) {
            for (int j = 0; j <= n; ++j) {
                if (getXY(n - j) != c->getXY(j) || getIsControl(n - j) != c->getIsControl(j)) {
                    return false;
                }
            }
            return true;
        }
    }
    return true;
}

}
