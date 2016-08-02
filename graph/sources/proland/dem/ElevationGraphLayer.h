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

#ifndef _PROLAND_ELEVATIONGRAPHLAYER_H_
#define _PROLAND_ELEVATIONGRAPHLAYER_H_

#include "proland/graph/producer/GraphLayer.h"
#include "proland/graph/producer/CurveDataFactory.h"
#include "proland/dem/ElevationCurveData.h"

namespace proland
{

/**
 * An abstract GraphLayer to modify the elevations of an ElevationProducer,
 * by drawing a Graph. The elevations of the Graph vertices are given
 * by ElevationCurveData objects, created by this class (which also
 * implements CurveDataFactory). The raw %terrain elevations (used to
 * compute elevation profiles for curves in ElevationCurveData) are
 * computed on CPU with the help of a CPUElevationProducer (or an equivalent
 * - see CPUElevationProducer#getHeight) referenced by this layer.
 * @ingroup dem
 * @authors Antoine Begault, Eric Bruneton
 */
PROLAND_API class ElevationGraphLayer : public GraphLayer, public CurveDataFactory
{
public:
    /**
     * Creates a new ElevationGraphLayer.
     *
     * @param name the layer type.
     */
    ElevationGraphLayer(const char *name);

    /**
     * Creates a new ElevationGraphLayer.
     *
     * @param name the layer type.
     * @param graphProducer the GraphProducer that produces the Graph tiles that
     *      this layer must draw.
     * @param layerProgram the Program to be used to draw the graphs in this layer.
     * @param elevations the %producer used to compute raw %terrain elevations,
     *      themselves used to compute ElevationCurveData objects.
     * @param displayLevel the tile level to start display. Tiles whole level is
     *      less than displayLevel are not drawn by this Layer, and graphProducer is not
     *      asked to produce the corresponding %graph tiles.
     * @param quality enable or not the quality mode (better display).
     * @param storeGraphTiles forces the graph tiles produced by graphProducer
     *      to stay in cache as long as the tiles produced by this producer
     *      (i.e. the TileProducer to which this Layer belongs) are in use.
     *      When used with the storeParents option in graphProducer, this option
     *      ensures that the graph tiles corresponding to the tiles of this
     *      producer that are in use, and all their ancestors, are in cache.
     *      Boths options are <b>required</b> when an EditGraphOrthoLayer is used
     *      (to locate Node from x,y coordinates efficiently, and to take
     *      advantage of the incremental clipUpdate methods). Otherwise they
     *      are not mandatory.
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    ElevationGraphLayer(const char *name, ptr<GraphProducer> graphProducer,
        ptr<Program> layerProgram, ptr<TileProducer> elevations, int displayLevel = 0,
        bool quality = true, bool storeGraphTiles = false, bool deform = false);

    /**
     * Deletes this ElevationGraphLayer.
     */
    virtual ~ElevationGraphLayer();

    virtual void getReferencedProducers(vector< ptr<TileProducer> > &producers) const;

    virtual void startCreateTile(int level, int tx, int ty, unsigned int deadline,
        ptr<Task> task, ptr<TaskGraph> result);

    virtual void stopCreateTile(int level, int tx, int ty);

    /**
     * Draws the altitude profile of a curve. The curve is drawn with its total
     * footprint width, which includes not only the curve itself, but the also
     * the area where it may modifies the terrain elevations (e.g. this includes
     * roadsides or riverbanks for roads and %rivers).
     *
     * @param p the Curve to be drawn.
     * @param data the ElevationCurveData containing the elevation profile of p.
     *       This object is normally created by this CurveDataFactory.
     * @param width the total footprint width of the curve.
     * @param nwidth the total footprint width of the curve, divided by its real
     *      width.
     * @param stepLength the maximum distance between two OpenGL vertices on the
     *      tesselation used to draw the curve p. If the vertices of p are too far
     *      apart, intermediate vertices are introduced to respect this maximum distance.
     * @param caps true to draw caps at the extremities of the curve.
     * @param fb the FrameBuffer into which the curve must be drawn.
     * @param prog a program used to draw the curve.
     * @param mesh a vec4 Mesh (containing vertex positions and UV attributes).
     *      The position is set to the vertex x,y coordinates. The u attribute
     *      contains the vertex z coordinate, and the v attribute the signed relative
     *      distance to the curve axis (i.e., plus or minus nwidth).
     */
    void drawCurveAltitude(const vec3d &tileCoords, CurvePtr p, ElevationCurveData *data, float width, float nwidth,
        float stepLength, bool caps, ptr<FrameBuffer> fb, ptr<Program> prog, Mesh<vec4f, unsigned int> &mesh,
        vec2d *nx = NULL, vec2d *ny = NULL, vec2d *lx = NULL, vec2d *ly = NULL);

    virtual void swap(ptr<ElevationGraphLayer> p);

protected:
    /**
     * Initializes this ElevationGraphLayer.
     *
     * @param graphProducer the GraphProducer that produces the Graph tiles that
     *      this layer must draw.
     * @param layerProgram the Program to be used to draw the graphs in this layer.
     * @param elevations the %producer used to compute raw %terrain elevations,
     *      themselves used to compute ElevationCurveData objects.
     * @param displayLevel the tile level to start display. Tiles whole level is
     *      less than displayLevel are not drawn by this Layer, and graphProducer is not
     *      asked to produce the corresponding %graph tiles.
     * @param quality enable or not the quality mode (better display).
     * @param storeGraphTiles forces the graph tiles produced by graphProducer
     *      to stay in cache as long as the tiles produced by this producer
     *      (i.e. the TileProducer to which this Layer belongs) are in use.
     *      When used with the storeParents option in graphProducer, this option
     *      ensures that the graph tiles corresponding to the tiles of this
     *      producer that are in use, and all their ancestors, are in cache.
     *      Boths options are <b>required</b> when an EditGraphOrthoLayer is used
     *      (to locate Node from x,y coordinates efficiently, and to take
     *      advantage of the incremental clipUpdate methods). Otherwise they
     *      are not mandatory.
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        ptr<TileProducer> elevations, int displayLevel = 0, bool quality = true,
        bool storeGraphTiles = false, bool deform = false);

    /**
     * The %producer used to compute raw %terrain elevations, themselves used
     * to compute ElevationCurveData objects.
     */
    ptr<TileProducer> elevations;
};

}

#endif
