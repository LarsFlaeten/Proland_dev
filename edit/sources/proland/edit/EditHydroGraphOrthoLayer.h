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

#ifndef _PROLAND_EDITHYDROGRAPHORTHOLAYER_H_
#define _PROLAND_EDITHYDROGRAPHORTHOLAYER_H_

#include "proland/edit/EditGraphOrthoLayer.h"
#include "proland/rivers/graph/HydroGraph.h"

namespace proland
{

/**
 * An OrthoGPUProducer layer to %edit Graph objects.
 * See EditGraphOrthoLayer.
 * This subclass handles HydroGraph, and allows to dynamically create rivers and quickly designing their banks.
 * @ingroup edit
 * @authors Antoine Begault, Eric Bruneton, Guillaume Piolat
 */
PROLAND_API class EditHydroGraphOrthoLayer : public EditGraphOrthoLayer
{
public:
    /**
     * The EventHandler associated to EditGraphOrthoLayer.
     * new Functionnality: adding a river to a given Curve. Just drag and drop.
     * Displays more info on Curve than the basic TweakGraphLayer (HydroCurve related info).
     */
    class EditHydroGraphHandler : public EditGraphOrthoLayer::EditGraphHandler
    {
    public:

        /**
         * Creates a new TweakHydroGraphLayer.
         *
         * @param e the associated EditGraphOrthoLayer on which the updates will occur.
         * @param r the RessourceManager used to load terrain Info.
         * @param t the name of the terrain from which we want to load info.
         */
        EditHydroGraphHandler(EditGraphOrthoLayer *e, ptr<ResourceManager> r, string t)
            : EditGraphOrthoLayer::EditGraphHandler(e, r, t)
        {
        }

        /**
         * See EventHandler#mouseClick.
         */
        virtual bool mouseClick(button b, state s, modifier m, int x, int y);

        /**
         * See EventHandler#mouseMotion.
         */
        virtual bool mouseMotion(int x, int y);
    };

    /**
     * Creates a new EditHydroGraphOrthoLayer.
     *
     * @param graphs a vector of GraphProducers that will be handled by this layer.
     * @param layerProgram the GLSL Program to be used to draw the graphs in this
     *      Layer.
     * @param displayLevel the tile level to start display. Tiles whole level is less than displayLevel are not drawn by this Layer,
     *          and graphProducer is not asked to produce the corresponding graph tiles.
     * @param tolerance tolerance parameter for screen selection of points.
     * @param softEdition true to only call the update() method once the user release the mouse when editing.
     *          Usefull for avoiding costly updates when editing.
     * @param softEditionDelay minimum amount of time between two updates if softEdition is false.
     * @param deform whether we apply a spherical deformation on the layer or not.
     * @param terrain name of the terrain on which this EditGraphOrthoLayer is located. only required if the terrainNode has been deformed or moved.
     * @param manager the ResourceManager used to create the scene.
     */
    EditHydroGraphOrthoLayer(const vector< ptr<GraphProducer> > &graphs, ptr<Program> layerProg, int displayLevel = 0, float tolerance = 8.0f / 192.0f, bool softEdition = true, double softEditionDelay = 100000.0, bool deform = false, string terrain = "", ptr<ResourceManager> manager = NULL) :
        EditGraphOrthoLayer(graphs, layerProg, displayLevel, tolerance, softEdition, softEditionDelay, deform, terrain, manager)
    {
    }

    /**
     * Deletes this EditHydroGraphOrthoLayer.
     */
    virtual ~EditHydroGraphOrthoLayer()
    {
    }

    /**
     * Adds a curve from selected point to given coords. If given coords doesn't corresponds to a given node, it creates it.
     * Also returns the new selection for interface functions.
     *
     * @param x X coord of second node.
     * @param y Y coord of second node.
     * @param tolerance tolerance parameter for node searching.
     * @param g the graph to modify.
     * @param[out] curve will contain the selected curve, NULL if none.
     * @param[out] point will contain the selected point's rank in 'curve', -1 if none.
     * @param[out] changes will contain the list of changes that occured in g.
     * @return true if changes occured.
     */
    virtual bool addCurve(double x, double y, float tolerance, GraphPtr g, CurvePtr &curve, int &point, Graph::Changes &changes);

    /**
     * Adds a Curve between two new nodes.
     * Also returns the new selection for interface functions.
     *
     * @param x1 X coord of first node.
     * @param y1 Y coord of first node.
     * @param x2 X coord of second node.
     * @param y2 Y coord of second node.
     * @param g the graph to modify.
     * @param[out] curve will contain the selected curve, NULL if none.
     * @param[out] point will contain the selected point's rank in 'curve', -1 if none.
     * @param[out] changes will contain the list of changes that occured in g.
     * @return true if changes occured.
     */
    virtual bool addCurve(double x1, double y1, double x2, double y2, float tolerance, GraphPtr g, CurvePtr &curve, int &segment, int &point, Graph::Changes &changes);

protected:
    /**
     * Creates a new EditHydroGraphOrthoLayer.
     */
    EditHydroGraphOrthoLayer() : EditGraphOrthoLayer()
    {
    }

    /**
     * Initializes EditGraphOrthoLayer fields.
     *
     * @param graphs a vector of GraphProducers that will be handled by this layer.
     * @param layerProgram the GLSL Program to be used to draw the graphs in this
     *      Layer.
     * @param displayLevel the tile level to start display. Tiles whole level is less than displayLevel are not drawn by this Layer,
     *          and graphProducer is not asked to produce the corresponding graph tiles.
     * @param tolerance tolerance parameter for screen selection of points.
     * @param softEdition true to only call the update() method once the user release the mouse when editing.
     *          Usefull for avoiding costly updates when editing.
     * @param softEditionDelay minimum amount of time between two updates if softEdition is false.
     * @param deform whether we apply a spherical deformation on the layer or not.
     * @param terrain name of the terrain on which this EditGraphOrthoLayer is located. only required if the terrainNode has been deformed or moved.
     * @param manager the ResourceManager used to create the scene.
     */
    virtual void init(const vector< ptr<GraphProducer> > &graphs, ptr<Program> layerProg, int displayLevel = 0, float tolerance = 8.0f / 192.0f, bool softEdition = true, double softEditionDelay = 100000.0, bool deform = false, string terrain = "", ptr<ResourceManager> manager = NULL);

    /**
     * Iterative method for finding a River axis which is suitable for a given bank segment.
     * This one searches the appropriate graph to search into, and calls #findRiver(GraphPtr g,...).
     *
     * @param t the tile to search into.
     * @param x1 x coordinate of the 1st point to search.
     * @param y1 y coordinate of the 1st point to search.
     * @param x2 x coordinate of the 2nd point to search.
     * @param y2 y coordinate of the 2nd point to search.
     * @param[out] orientationC2R true if the axis is on the left of the bank.
     * @param[out] orientationC2R true if the bank is on the left of the axis.
     * @return the river if something was found.
     */
    CurvePtr findRiver(TileCache::Tile *t, double x1, double y1, double x2, double y2, int &orientationC2R, int &orientationR2C);

    /**
     * Iterative method for finding a River axis which is suitable for a given bank segment.
     *
     * @param p the graph to search into.
     * @param x1 x coordinate of the 1st point to search.
     * @param y1 y coordinate of the 1st point to search.
     * @param x2 x coordinate of the 2nd point to search.
     * @param y2 y coordinate of the 2nd point to search.
     * @param[out] orientationC2R true if the axis is on the left of the bank.
     * @param[out] orientationC2R true if the bank is on the left of the axis.
     * @return the river if something was found.
     */
    CurvePtr findRiver(GraphPtr p, double x1, double y1, double x2, double y2, int &orientationC2R, int &orientationR2C);

    void swap(ptr<EditHydroGraphOrthoLayer> p);

    /**
     * AddingRiver state. True if we are adding a river axis.
     * This options allows to manually add a river axis to a given Bank.
     */
    bool addingRiver;

    friend class TweakHydroGraphLayer;
};

}

#endif
