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

#ifndef _PROLAND_LAZYHYDROGRAPH_H
#define _PROLAND_LAZYHYDROGRAPH_H

#include "proland/graph/LazyGraph.h"

namespace proland
{

/**
 * A HydroGraph with lazy loading behavior.
 * See graph::LazyGraph and rivers::HydroGraph.
 * @ingroup rivergraph
 * @author Antoine Begault
 */
PROLAND_API class LazyHydroGraph : public LazyGraph
{
public:
    /**
     * Creates a new LazyHydroGraph.
     */
    LazyHydroGraph();

    /**
     * Deletes this LazyHydroGraph.
     */
    virtual ~LazyHydroGraph();

    /**
     * Checks if the provided params count are correct for this graph.
     * A correct amount of parameters may depend on the graph it is used by.
     * For Graph, the amounts must be respectively at least 2, 4, 3, 3, 5, 2, 0.
     * Should be overwritten for graphs containing more data.
     */
    virtual void checkParams(int nodes, int curves, int areas, int curveExtremities, int curvePoints, int areaCurves, int subgraphs);

    /**
     * Adds a curve to this graph.
     *
     * @param parent the parent curve (NULL if none).
     * @param setParent if true, the new curve's parent will be set to the 'parent' parameter.
     * @return the new curve.
     */
    virtual CurvePtr newCurve(CurvePtr parent, bool setParent);

    /**
     * Adds a curve to this graph.
     *
     * @param model a model curve : the new curve will have the same vertices.
     * @param start the start node.
     * @param end the end node.
     * @return the new curve.
     */
    virtual CurvePtr newCurve(CurvePtr model, NodePtr start, NodePtr end);

    /**
     * Returns a new HydroGraph.
     */
    virtual Graph *createChild();

    /**
     * Saves this graph from a basic file.
     *
     * @param fileWriter the FileWriter used to save the file.
     * @param saveAreas if true, will save the subgraphs.
     */
    virtual void save(FileWriter *fileWriter, bool saveAreas = true);

    /**
     * Saves this graph from an indexed file.
     * Indexed files are used to load LazyGraphs faster. It contains the indexes of each element in the file.
     *
     * @param fileWriter the FileWriter used to save the file
     * @param saveAreas if true, will save the subgraphs
     */
    virtual void indexedSave(FileWriter *fileWriter, bool saveAreas = true);

    virtual void movePoint(CurvePtr c, int i, const vec2d &p, set<CurveId> &changedCurves);

    virtual NodePtr addNode(CurvePtr c, int i, Graph::Changes &changed);

protected:
    /**
     * Loads the Curve corresponding to the given Id.
     * The Curve description will be fetched via #fileReader at the offset given as parameter.
     *
     * @param offset the offset of this Curve in the file.
     * @param id the id of this Curve.
     * @return the loaded Curve.
     */
    virtual CurvePtr loadCurve(long int offset, CurveId id);

    friend class LazyHydroCurve;
};

}

#endif
