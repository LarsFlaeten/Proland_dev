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

#ifndef _PROLAND_BASICCURVEPART_H_
#define _PROLAND_BASICCURVEPART_H_

#include "proland/graph/CurvePart.h"

namespace proland
{

/**
 * A part of a curve. This part is defined by a curve, and by two indexes that
 * give the start and end points of this curve part inside the whole curve.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class BasicCurvePart : public CurvePart
{
public:
    /**
     * Creates a new CurvePart.
     *
     * @param p the whole curve.
     * @param start start of the interval in p to which this part corresponds.
     * @param end end of the interval in p to which this part corresponds.
     */
    BasicCurvePart(CurvePtr p, int start, int end);

    /**
     * Creates a new CurvePart.
     *
     * @param p the whole curve.
     * @param orientation the curve part orientation (see #orientation).
     * @param start start of the interval in p to which this part corresponds.
     * @param end end of the interval in p to which this part corresponds.
     */
    BasicCurvePart(CurvePtr p, int orientation, int start, int end);

    /**
     * Returns the original curve Id.
     */
    virtual CurveId getId() const;

    /**
     * Returns the original curve parentId.
     */
    virtual CurveId getParentId() const;

    /**
     * Returns the original curve.
     */
    CurvePtr getCurve() const;

    /**
     * Returns the corresponding index of i in the original curve.
     *
     * @param i rank of the point
     */
    int getCurveIndex(int i) const;

    /**
     * Returns the original curve type.
     */
    virtual int getType() const;

    /**
     * Returns the original curve width.
     */
    virtual float getWidth() const;

    /**
     * Returns the length if this curvepart.
     */
    virtual int getEnd() const;

    /**
     * Returns the XY coords of the i'th point from this curve.
     *
     * @param i rank of the point to get the coords from.
     */
    virtual vec2d getXY(int i) const;

    /**
     * Returns the value isControl of the i'th point from this curve.
     *
     * @param i rank of the point to get the boolean from.
     */
    virtual bool getIsControl(int i) const;

    /**
     * Returns the S coords of the i'th point from this curve.
     *
     * @param i rank of the point to get the coord from.
     */
    virtual float getS(int i) const;

    /**
     * Returns the bounds of this curvePart.
     */
    virtual box2d getBounds() const;

    /**
     * Returns false if the i'th point is a controlPoint, else true.
     *
     * @param i rank of the point.
     */
    virtual bool canClip(int i) const;

    /**
     * Clips this curvePart.
     *
     * @param start starting point.
     * @param end end point.
     * @return a new CurvePart, result of the clipping of this CurvePart.
     */
    virtual CurvePart *clip(int start, int end) const;

protected:
    /**
     * The original curve.
     */
    CurvePtr curve;

    /**
     * The orientation of this curve part. 0 means that the curve's start
     * and end are given by #start and #end respectively. 1 means that the
     * curve's start and end are given by #end and #start respectively (note
     * that #end is always greater than #start).
     */
    int orientation;

    /**
     * Start of the interval, inside #path, to which this curve part
     * corresponds (inclusive).
     */
    int start;

    /**
     * End of the interval, inside #path, to which this curve part
     * corresponds (inclusive).
     */
    int end;
};

}

#endif
