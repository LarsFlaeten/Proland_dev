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

#include "proland/graph/producer/CurveData.h"

#include "proland/producer/CPUTileStorage.h"
#include "proland/graph/producer/GraphLayer.h"

namespace proland
{

#define UNINITIALIZED -1e9


CurveData::CurveData(CurveId id, CurvePtr flattenCurve) :
    id(id), flattenCurve(flattenCurve)
{
    length = flattenCurve->computeCurvilinearLength();

    if (flattenCurve->getStart()->getCurveCount() > 1) {
        NodePtr n = flattenCurve->getStart();
        startCapLength = getCapLength(n, flattenCurve->getXY(n, 1));
    } else {
        startCapLength = 0.f;
    }
    if (flattenCurve->getEnd()->getCurveCount() > 1) {
        NodePtr n = flattenCurve->getEnd();
        endCapLength = getCapLength(n, flattenCurve->getXY(n, 1));
    } else {
        endCapLength = 0.f;
    }
}

CurveData::~CurveData()
{
    flattenCurve = NULL;
}


CurveId CurveData::getId()
{
    return id;
}

float CurveData::getCapLength(NodePtr p, vec2d q)
{
    vec2d o = p->getPos();
    float capLength = 0;
    for (int i = 0; i < p->getCurveCount(); ++i) {
        CurvePtr ipath = p->getCurve(i);
        if (!(ipath->getId() == getId())) {
            vec2d r = ipath->getXY(p, 1);
            if (abs(angle(q - o, r - o) - M_PI) < 0.01) {
                continue;
            }
            float pw = 2 * flattenCurve->getWidth();
            float ipw = 2 * ipath->getWidth();
            vec2d corner = proland::corner(o, q, r, (double) pw, (double) ipw);
            float dot = (q - o).dot(corner - o);
            capLength = max((double) capLength, dot / (o - q).length());
        }
    }
    return ceil(capLength);
}

float CurveData::getStartCapLength()
{
    return startCapLength / 2;
}

float CurveData::getEndCapLength()
{
    return endCapLength / 2;
}

float CurveData::getCurvilinearLength()
{
    return length;
}

float CurveData::getCurvilinearLength(float s, vec2d *p, vec2d *n)
{
    return flattenCurve->getCurvilinearLength(s, p, n);
}

float CurveData::getCurvilinearCoordinate(float l, vec2d *p, vec2d *n)
{
    return flattenCurve->getCurvilinearCoordinate(l, p, n);
}

void CurveData::getUsedTiles(set<TileCache::Tile::Id> &tiles, float rootSampleLength)
{
    return;
}

float CurveData::getS(int rank)
{
    return flattenCurve->getS(rank);
}

CurvePtr CurveData::getFlattenCurve()
{
    return flattenCurve;
}

}
