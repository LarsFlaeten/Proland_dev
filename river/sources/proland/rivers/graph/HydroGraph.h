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

#ifndef _PROLAND_HYDROGRAPH_H_
#define _PROLAND_HYDROGRAPH_H_

#include "proland/graph/BasicGraph.h"
#include "proland/rivers/graph/HydroCurve.h"

namespace proland
{

/**
 * @defgroup rivergraph graph
 * Provides graphs with river related information.
 * @ingroup rivers
 */

/**
 * A Graph with additional, river specific data. This data consists of:
 * - Rivers: Basically, they are regular Curves.
 * - Banks : Corresponds to a part of the boundaries of a given River.
 * Important to improve performances. Also contains a potential Value.
 * Areas are treated just like regular areas.
 * @ingroup rivergraph
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class HydroGraph : public BasicGraph
{
public:
    /**
     * Creates a new HydroGraph.
     */
    HydroGraph();

    /**
     * Deletes this HydroGraph.
     */
    virtual ~HydroGraph();

    virtual void load(FileReader * fileReader, bool loadSubgraphs = true);

    virtual void loadIndexed(FileReader * fileReader, bool loadSubgraphs = true);

    virtual void checkParams(int nodes, int curves, int areas, int curveExtremities, int curvePoints, int areaCurves, int subgraphs);

    virtual CurvePtr newCurve(CurvePtr parent, bool setParent);

    virtual CurvePtr newCurve(CurvePtr model, NodePtr start, NodePtr end);

    virtual CurvePtr addCurvePart(CurvePart &cp, set<CurveId> *addedCurves, bool setParent = true);

    virtual Graph *createChild();

    virtual void save(FileWriter *fileWriter, bool saveAreas = true);

    virtual void indexedSave(FileWriter *fileWriter, bool saveAreas = true);

    /**
     * Saves a graph from a basic file.
     *
     * @param graph the graph to save
     * @param fileWriter the FileWriter used to save the file.
     * @param saveAreas if true, will save the subgraphs.
     */
    static void save(Graph *graph, FileWriter *fileWriter, bool saveAreas = true);

    /**
     * Saves a graph from an indexed file.
     * Indexed files are used to load LazyGraphs faster. It contains the indexes of each element in the file.
     *
     * @param graph the graph to save
     * @param fileWriter the FileWriter used to save the file
     * @param saveAreas if true, will save the subgraphs
     */
    static void indexedSave(Graph *graph, FileWriter *fileWriter, bool saveAreas = true);

    virtual void movePoint(CurvePtr c, int i, const vec2d &p, set<CurveId> &changedCurves);

    virtual NodePtr addNode(CurvePtr c, int i, Graph::Changes &changed);

    virtual void print(bool detailed);
};

}

#endif
