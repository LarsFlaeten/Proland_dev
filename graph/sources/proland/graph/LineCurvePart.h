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

#ifndef _PROLAND_LINECURVEPART_H_
#define _PROLAND_LINECURVEPART_H_

#include "proland/graph/CurvePart.h"

namespace proland
{

/**
 * A part of a curve. This part is defined by 2 points (start and end points).
 * This is used for completing a clipped area when one or more of its curves
 * are out of the clipping box.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class LineCurvePart : public CurvePart
{
public:
    /**
     * Creates a new LineCurvePart.
     *
     * @param start Coordinates of the starting point.
     * @param end Coordinates of the end point.
     */
    LineCurvePart(vec2d start, vec2d end);

    /**
     * Returns the end index of this curve part inside the original curve.
     */
    virtual int getEnd() const;

    /**
     * Returns the coordinates of a point at a given index.
     *
     * @param i the rank of the Node to get the coordinates from.
     */
    virtual vec2d getXY(int i) const;

    /**
     * Returns true if the i'th point is a Control Point.
     *
     * @param i the rank of the Node to get the boolean from.
     */
    virtual bool getIsControl(int i) const;

    /**
     * Returns the i'th point S coordinate.
     *
     * @param i the rank of the Node to get the coordinate from.
     */
    virtual float getS(int i) const;

    /**
     * Returns the curve part's bounding box.
     */
    virtual box2d getBounds() const;

    /**
     * Returns a sub-curvePart clipped from this one.
     *
     * @param start starting point.
     * @param end end point.
     * @return a new CurvePart, result of the clipping of this CurvePart.
     */
    virtual CurvePart *clip(int start, int end) const;

private:
    /**
     * The start point that defines this CurvePart.
     */
    vec2d start;

    /**
     * The start point that defines this CurvePart.
     */
    vec2d end;
};

}

#endif
