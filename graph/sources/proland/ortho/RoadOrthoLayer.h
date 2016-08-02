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

#ifndef _PROLAND_ROADORTHOLAYER_H_
#define _PROLAND_ROADORTHOLAYER_H_

#include "proland/graph/producer/GraphLayer.h"
#include "proland/graph/producer/CurveDataFactory.h"

namespace proland
{

/**
 * An OrthoGPUProducer layer to draw roads.
 * @ingroup ortho
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class RoadOrthoLayer : public GraphLayer, public CurveDataFactory
{
public:
    /**
     * Predefined type for roads.
     * Used for drawing and managing roads.
     */
    enum roadType
    {
        BASIC = 0,
        ROAD = 0, //!< Basic road.
        UNKNOWN = 1, //!< Undefined.
        BRIDGE = 2 //!< Bridge linking 2 roads, passing on top of another.
    };

    /**
     * Creates a new RoadOrthoLayer.
     *
     * @param graphProducer the GraphProducer that produces the graphs to
     *      be drawn by this layer.
     * @param layerProgram the Program to be used to draw the graphs.
     * @param displayLevel the quadtree level at which the display of
     *      this layer must start.
     * @param quality enable or not the quality mode (better display).
     * @param color roads color (default 0:0:0).
     * @param dirt small dirt paths color.
     * @param border roads border color.
     * @param border_width roads border width.
     * @param inner_border_width roads inner border width.
     * @param deform true if the produced tiles will be mapped on a spherical
     *      %terrain.
     */
    RoadOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        int displayLevel = 0, bool quality = true, vec4f color = vec4f(0, 0, 0, 0),
        vec4f dirt = vec4f(0, 0, 0, 0), vec4f border = vec4f(1, 1, 1, 0),
        float border_width = 1.2f, float inner_border_width = 2.0f,
        bool deform = false);

    /**
     * Deletes this RoadOrthoLayer.
     */
    virtual ~RoadOrthoLayer();

    virtual void setTileSize(int tileSize, int tileBorder, float rootQuadSize);

    virtual void startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> result);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void stopCreateTile(int level, int tx, int ty);

protected:
    /**
     * Creates an uninitialized RoadOrthoLayer.
     */
    RoadOrthoLayer();

    /**
     * Initializes this RoadOrthoLayer. See #RoadOrthoLayer.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        int displayLevel = 0, bool quality = true, vec4f color = vec4f(0, 0, 0, 0),
        vec4f dist = vec4f(0, 0, 0, 0), vec4f border = vec4f(1, 1, 1, 0),
        float border_width = 1.2f, float inner_border_width = 2.0f,
        bool deform = false);

    virtual void swap(ptr<RoadOrthoLayer> p);

private:
    /**
     * Roads border width.
     */
    float border_width;

    /**
     * Roads inner border width.
     */
    float inner_border_width;

    /**
     * Roads color (default 0:0:0).
     */
    vec4f color;

    /**
     * Small dirt paths color.
     */
    vec4f dirt;

    /**
     * Roads border color.
     */
    vec4f border;

    /**
     * The mesh used for drawing curves.
     * Contains XY coordinates of each vertex.
     */
    ptr< Mesh<vec2f, unsigned int> > mesh;

    /**
     * The mesh used for drawing curves.
     * Contains XY coordinates + UV parameters for each vertex.
     */
    ptr< Mesh<vec4f, unsigned int> > meshuv;

    /**
     * Draws a road end white stripe between 2 points.
     *
     * @param fb a FrameBuffer.
     * @param p Coordinates of the first point.
     * @param n Coordinates of the second point.
     * @param w The line width.
     * @param scale The scale, based on the current display level.
     */
    void drawRoadEnd(const vec3d &tileCoords, ptr<FrameBuffer> fb, const vec2d &p, const vec2d &n, double length, float w, float scale, vec2d *nx = NULL, vec2d *ny = NULL, vec2d *lx = NULL, vec2d *ly = NULL);

    /**
     * Returns the length of a curve to be drawn without stripes.
     *
     * @param p a curve.
     * @param data the CurveData containing data about Curve p.
     * @param start if true, will compute from the starting node. Else, from the end node.
     */
    float getLengthWithoutStripes(CurvePtr p, CurveData * data, bool start);

    /**
     * Returns the longest flat length of curves around a node.
     *
     * @param p a Node.
     * @param q a position.
     * @param path a Curve.
     * @return the longest flat length around node p.
     */
    float getFlatLength(NodePtr p, vec2d q, CurvePtr path);

    ptr<Uniform3f> tileOffsetU;

    ptr<Uniform4f> colorU;

    ptr<Uniform2f> blendSizeU;

    ptr<Uniform3f> stripeSizeU;
};

}

#endif
