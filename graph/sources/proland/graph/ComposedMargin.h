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

#ifndef _PROLAND_COMPOSEDMARGIN_H_
#define _PROLAND_COMPOSEDMARGIN_H_

#include "proland/graph/Margin.h"

namespace proland
{

/**
 * A Margin is used to determine what to clip in a graph.
 * It enables to take into account the width of a curve when clipping a box.
 * For example, when drawing a road that would go along a border, the curve would be in only one tile,
 * but considering the curve's width, the drawing would be cut on the tile border. That is why we need margins.
 * This class enables to check multiple Margins at the same time.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class ComposedMargin : public Margin
{
public:
    /**
     * Creates a new ComposedMargin.
     */
    ComposedMargin();

    /**
     * Deletes this ComposedMargin.
     */
    virtual ~ComposedMargin();

    /**
     * Returns the maximum margin from contained margins (max borderSize).
     *
     * @param clipSize size of the clipping box.
     */
    virtual double getMargin(double clipSize);

    /**
     * Returns the maximum margin from contained margins.
     *
     * @param clipSize size of the clipping box.
     * @param p the curve to get the margin from.
     */
    virtual double getMargin(double clipSize, CurvePtr p);

    /**
     * Returns the maximum margin from contained margins.
     *
     * @param clipSize size of the clipping box.
     * @param a the area to get the margin from.
     */
    virtual double getMargin(double clipSize, AreaPtr a);

    /**
     * Adds a margin.
     *
     * @param m a margin.
     */
    void addMargin(Margin *m);

    /**
     * Removes a margin.
     *
     * param m a margin.
     */
    void removeMargin(Margin *m);

private:
    /**
     * list of margins.
     */
    vector<Margin*> margins;
};

}

#endif
