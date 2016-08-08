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

#ifndef _PROLAND_HYDROCURVE_H_
#define _PROLAND_HYDROCURVE_H_

#include "proland/graph/Curve.h"

namespace proland
{

/**
 * A Curve with additional, river specific data.
 * An HydroCurve can be of 2 different types:
 * - A River, which is the same as a Curve
 * - A Bank, which contains links to Rivers and potential values.
 * @ingroup rivergraph
 * @author Antoine Begault
 */
PROLAND_API class HydroCurve : public Curve
{
public:
    /**
     * HydroCurve Type.
     */
    enum HYDRO_CURVE_TYPE
    {
        AXIS,
        CLOSING_SEGMENT,
        BANK
    };

    /**
     * Creates a new HydroCurve.
     *
     * @param owner the graph containing this curve.
     */
    HydroCurve(Graph *owner);

    /**
     * Creates a new HydroCurve, with parameters copied from another Curve.
     *
     * @param owner the graph containing this curve.
     * @param c the copied curve, from which this curve takes its data.
     * @param s the start node.
     * @param e the end node.
     */
    HydroCurve(Graph *owner, CurvePtr c, NodePtr s, NodePtr e);

    /**
     * Deletes this HydroCurve.
     */
    virtual ~HydroCurve();

    /**
     * Returns the width of this curve.
     */
    float getWidth() const;

    /**
     * Returns this Curve's potential.
     * Returns -1 if this Curve is a river axis.
     */
    float getPotential() const;

    /**
     * Sets this curve's potential.
     */
    void setPotential(float potential);

    /**
     * Returns the id of the river axis associated to this HydroCurve.
     * NULL_ID if this Curve is a river axis.
     */
    CurveId getRiver() const;

    /**
     * Returns the river axis associated to this HydroCurve.
     * NULL if this Curve is a river.
     */
    CurvePtr getRiverPtr() const;

    /**
     * Sets the river axis associated to this HydroCurve.
     *
     * @param river the Curve's Id.
     */
    void setRiver(CurveId river);

    /**
     * Display method. For debug only.
     */
    virtual void print() const;

protected:
    /**
     * River axis associated to this HydroCurve.
     */
    CurveId river;

    /**
     * Value used to determine the flow between two banks.
     */
    float potential;

    friend class HydroGraph;
};

}

#endif
