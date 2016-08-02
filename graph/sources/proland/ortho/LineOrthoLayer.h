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

#ifndef _PROLAND_LINEORTHOLAYER_H_
#define _PROLAND_LINEORTHOLAYER_H_

#include "proland/graph/producer/GraphLayer.h"

namespace proland
{

/**
 * A GraphLayer that displays curves with lines of one pixel width.
 * @ingroup ortho
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class LineOrthoLayer : public GraphLayer
{
public:
    /**
     * Creates a new LineOrthoLayer.
     *
     * @param graphProducer the GraphProducer that produces the graphs to
     *      be drawn by this layer.
     * @param layerProgram the Program to be used to draw the graphs.
     * @param displayLevel the quadtree level at which the display of
     *      this layer must start.
     */
    LineOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel = 0);

    /**
     * Deletes this LineOrthoLayer.
     */
    virtual ~LineOrthoLayer();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

protected:
    /**
     * Creates an uninitialized LineOrthoLayer.
     */
    LineOrthoLayer();

    /**
     * Initializes this LineOrthoLayer. See #LineOrthoLayer.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel = 0);

    virtual void swap(ptr<LineOrthoLayer> p);

private:
    /**
     * The mesh used for drawing curves.
     */
    ptr<Mesh<vec2f, unsigned int> > mesh;

    ptr<Uniform3f> tileOffsetU;
};

}

#endif
