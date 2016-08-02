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

#ifndef _PROLAND_WATERORTHOLAYER_H_
#define _PROLAND_WATERORTHOLAYER_H_

#include "proland/graph/producer/GraphLayer.h"

namespace proland
{

/**
 * An OrthoGPUProducer layer to draw static %rivers and lakes.
 * @ingroup ortho
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class WaterOrthoLayer : public GraphLayer
{
public:
    /**
     * Predefined type for rivers.
     * Used for drawing and managing rivers.
     */
    enum riverType
    {
        BORDER = -2, //!< River Banks. Curves with this type have as ancestor the curve representing the center of the river they belong to.
        OBSTACLE = -1, //!< Floating obstacle or islands.
        RIVER = 0, //!< Basic river.
        ISLAND = 1, //!< Represents islands when area1 != NULL.
        LAKE = 2 //!< Lake.
    };

    /**
     * Creates a new WaterOrthoLayer.
     *
     * @param graphProducer the GraphProducer that produces the graphs to
     *      be drawn by this layer.
     * @param layerProgram the Program to be used to draw the graphs.
     * @param displayLevel the quadtree level at which the display of
     *      this layer must start.
     * @param quality enable or not the quality mode (better display).
     * @param color the color of water (default 0:0:0).
     * @param deform true if the produced tiles will be mapped on a spherical
     *      %terrain.
     */
    WaterOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        int displayLevel = 0, bool quality = true, vec4f color = vec4f(0, 0, 0, 0),
        bool deform = false);

    /**
     * Deletes this WaterOrthoLayer.
     */
    virtual ~WaterOrthoLayer();

    virtual void setTileSize(int tileSize, int tileBorder, float rootQuadSize);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

protected:
    /**
     * Creates an uninitialized WaterOrthoLayer.
     */
    WaterOrthoLayer();

    /**
     * Initializes this WaterOrthoLayer. See #WaterOrthoLayer.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        int displayLevel = 0, bool quality = true, vec4f color = vec4f(0, 0, 0, 0),
        bool deform = true);

    virtual void swap(ptr<WaterOrthoLayer> p);

private:
    /**
     * Water color.
     */
    vec4f color;

    /**
     * The mesh used for drawing curves.
     */
    ptr< Mesh<vec2f, unsigned int> > mesh;

    /**
     * The tesselator used for drawing areas.
     */
    ptr<Tesselator> tess;

    ptr<Uniform3f> tileOffsetU;

    ptr<Uniform4f> colorU;
};

}

#endif
