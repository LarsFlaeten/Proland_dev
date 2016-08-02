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

#ifndef _PROLAND_ORTHOMARGIN_H_
#define _PROLAND_ORTHOMARGIN_H_

#include "proland/graph/Margin.h"

namespace proland
{

/**
 * A Margin to clip a Graph for an OrthoGPUProducer layer. This margin
 * enlarges the clip region so that it also includes the ortho tile borders.
 * @ingroup ortho
 * @author Antoine Begault
 */
PROLAND_API class OrthoMargin : public Margin
{
public:
    /**
     * Creates an uninitialized OrthoMargin.
     */
    OrthoMargin();

    /**
     * Creates a new OrthoMargin.
     *
     * @param samplesPerTile number of pixels per ortho tile (without borders).
     * @param borderFactor size of the tile borders in percentage of tile size.
     * @param widthFactor width of drawn curves in percentage of curve width.
     */
    OrthoMargin(int samplesPerTile, float borderFactor, float widthFactor);

    /**
     * Deletes this OrthoMargin.
     */
    virtual ~OrthoMargin();

    virtual double getMargin(double clipSize);

    virtual double getMargin(double clipSize, CurvePtr p);

protected:
    /**
     * Number of pixels per elevation tile, without borders.
     */
    int samplesPerTile;

    /**
     * Size of the tile borders in percentage of tile size.
     */
    float borderFactor;

    /**
     * Width of drawn curves in percentage of curve width.
     */
    float widthFactor;
};

}

#endif
