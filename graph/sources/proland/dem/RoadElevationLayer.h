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

#ifndef _PROLAND_ROADELEVATIONLAYER_H_
#define _PROLAND_ROADELEVATIONLAYER_H_

#include "proland/dem/ElevationGraphLayer.h"
#include "proland/dem/ElevationMargin.h"
#include "proland/dem/ElevationCurveData.h"

namespace proland
{

/**
 * An ElevationGraphLayer for road graphs.
 * @ingroup dem
 * @authors Antoine Begault, Eric Bruneton, Guillaume Piolat
 */
PROLAND_API class RoadElevationLayer : public ElevationGraphLayer
{
public:
    /**
     * An ElevationCurveData for road elevation profiles.
     */
    class RoadElevationCurveData : public ElevationCurveData
    {
    public:
        /**
         * Predefined types for roads.
         * Used for drawing and managing roads.
         */
        enum roadType
        {
            BASIC = 0,
            ROAD = 0, //!< Basic road.
            UNKNOWN = 1, //!< Undefined.
            BRIDGE = 2//!< Bridge linking 2 roads, passing on top of another.
        };

        /**
         * Creates a new RoadElevationCurveData.
         *
         * @param id the id of the curve for which we need to store the data.
         * @param flattenCurve the flattened version of the curve for which we need
         *      to store the data.
         * @param elevations the %producer used to compute raw %terrain elevations,
         *      themselves used to compute the elevation profile.
         */
        RoadElevationCurveData(CurveId id, CurvePtr flattenCurve, ptr<TileProducer> elevations);

        /**
         * Deletes this RoadElevationCurveData.
         */
        ~RoadElevationCurveData();

        virtual float getStartHeight();

        virtual float getEndHeight();

        virtual float getAltitude(float s);

        virtual void getUsedTiles(set<TileCache::Tile::Id> &tiles, float rootSampleLength);

        void getBridgesz();

    protected:
        /**
         * Computes the cap length at a given extremity.
         *
         * @param p the Node from which to compute the cap length.
         * @param q a point determining the direction of the cap length.
         * @param path the Curve to compute the cap length from. Must contain p & q.
         */
        virtual float getCapLength(NodePtr p, vec2d q);

        /**
         * True if the starting point of the road is connected to a bridge
         */
        bool startBridge;

        /**
         * True if the ending point of the road is connected to a bridge
         */
        bool endBridge;

        /**
         * Elevation at the starting point of the road, if connected to a bridge.
         */
        float startBridgez;

        /**
         * Elevation at the ending point of the road, if connected to a bridge.
         */
        float endBridgez;

        /**
         * True if extremities were checked.
         */
        bool initBridges;
    };

    /**
     * An ElevationMargin for roads. This margin takes into account the
     * total footprint width of roads, larger than their real widths
     * (see ElevationGraphLayer#drawCurveAltitude).
     */
    class RoadElevationMargin : public ElevationMargin
    {
    public:
        /**
         * Creates a new RoadElevationMargin.
         *
         * @param samplesPerTile number of pixels per elevation tile (without borders).
         * @param borderFactor size of the border in percentage of tile size.
         */
        RoadElevationMargin(int samplesPerTile, float borderFactor);

        /**
         * Deletes this RoadElevationMargin.
         */
        ~RoadElevationMargin();

        virtual double getMargin(double clipSize, CurvePtr p);
    };

    /**
     * Creates a new RoadElevationLayer.
     *
     * @param graphProducer the GraphProducer that produces the Graph tiles that
     *      this layer must draw.
     * @param layerProgram the Program to be used to draw the roads in this layer.
     * @param elevations the %producer used to compute raw %terrain elevations,
     *      themselves used to compute ElevationCurveData objects.
     * @param displayLevel the tile level to start display. Tiles whole level is
     *      less than displayLevel are not drawn by this Layer, and graphProducer is not
     *      asked to produce the corresponding %graph tiles.
     * @param quality enable or not the quality mode (better display).
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    RoadElevationLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        ptr<TileProducer> elevations, int displayLevel = 0,
        bool quality = true, bool deform = false);

    /**
     * Deletes this RoadOrthoLayer.
     */
    virtual ~RoadElevationLayer();

    virtual void setTileSize(int tileSize, int tileBorder, float rootQuadSize);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual CurveData *newCurveData(CurveId id, CurvePtr flattenCurve);

protected:
    /**
     * Creates an uninitialized RoadOrthoLayer.
     */
    RoadElevationLayer();

    /**
     * Initializes this RoadElevationLayer.
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
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        ptr<TileProducer> elevations, int displayLevel = 0,
        bool quality = true, bool deform = false);

    virtual void swap(ptr<RoadElevationLayer> p);

private:
    /**
     * The mesh used for drawing road elevation profiles. Contains vertex positions
     * and UV attributes. The position is set to the vertex x,y coordinates. The u
     * attribute contains the vertex z coordinate, and the v attribute the signed
     * distance to the road axis (divided by the road width).
     */
    ptr< Mesh<vec4f, unsigned int> > meshuv;

    ptr<Uniform3f> tileOffsetU;
};

}
#endif
