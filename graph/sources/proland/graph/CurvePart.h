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

#ifndef _PROLAND_CURVEPART_H_
#define _PROLAND_CURVEPART_H_

#include "proland/graph/Curve.h"

namespace proland
{

/**
 * A part of a curve. This part is defined by a curve, and by two indexes that
 * give the start and end points of this curve part inside the whole curve.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class CurvePart
{
public:
    /**
     * Creates a new CurvePart.
     */
    CurvePart();

    /**
     * Deletes this CurvePart.
     */
    virtual ~CurvePart();

    /**
     * Returns the original curve's Id.
     */
    virtual CurveId getId() const;

    /**
     * Returns the original curve's parent.
     */
    virtual CurveId getParentId() const;

    /**
     * Returns the original curve's type.
     */
    virtual int getType() const;

    /**
     * Returns the original curve's width.
     */
    virtual float getWidth() const;

    /**
     * Returns the original curve.
     */
    virtual CurvePtr getCurve() const;

    /**
     * Returns the end index of this curve part inside the original curve.
     */
    virtual int getEnd() const = 0;

    /**
     * Returns the coordinates of a point at a given index.
     *
     * @param i the rank of the vertex to return.
     * @return the XY coords of the i'th vertex.
     */
    virtual vec2d getXY(int i) const = 0;

    /**
     * Returns the coordinates of a point at a given index starting from a given Point.
     *
     * @param start the starting position (start or end point).
     * @param offset the rank of the vertex to return.
     * @return the XY coords of the i'th vertex.
     */
    vec2d getXY(const vec2d &start, int offset) const;

    /**
     * Checks if a point is a control Point.
     *
     * @param i the rank of the vertex to check.
     * @return true if the i'th vertex is a control point.
     */
    virtual bool getIsControl(int i) const;

    /**
     * Returns the i'th point S coordinate.
     *
     * @param i the rank of the vertex to get the coordinates from.
     * @return the pseudo curvilinear coordinate of the i'th point.
     */
    virtual float getS(int i) const = 0;

    /**
     * Returns the curve part's bounding box
     */
    virtual box2d getBounds() const = 0;

    /**
     * Checks if a point is clipable.
     *
     * @param i the rank of the point to check.
     * @return true if the i'th point is clipable.
     */
    virtual bool canClip(int i) const;

    /**
     * Returns a sub-curvePart clipped from this one.
     *
     * @param start the starting index of the new curve inside the current one.
     * @param end the ending index of the new curve inside the current one.
     * @return a new CurvePart, containing the nodes in range [start - end] from this.
     */
    virtual CurvePart* clip(int start, int end) const = 0;

    /**
     * Clips this curve part with the given rectangle. This results in a set of
     * curve parts that are added to the given list. Note that the curve width
     * is NOT taken into account.
     *
     * @param clip the clip region.
     * @param result where the resulting curve parts must be stored.
     */
    void clip(const box2d &clip, vector<CurvePart*> &result) const;

    /**
     * Clips a set of curve parts.
     *
     * @param paths the curve parts that must be clipped.
     * @param clip the clip region.
     * @param result where the resulting curve parts must be stored.
     */
    static void clip(const vector<CurvePart*> &paths, const box2d &clip, vector<CurvePart*> &result);

    /**
     * Returns true if this equals a given Curve.
     *
     * @param c the curve to compare with.
     */
    bool equals(CurvePtr c) const;
};

}

#endif
