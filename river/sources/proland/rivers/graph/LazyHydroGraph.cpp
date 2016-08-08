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

#include "proland/rivers/graph/LazyHydroGraph.h"

#include "ork/core/Logger.h"
#include "proland/rivers/graph/LazyHydroCurve.h"
#include "proland/rivers/graph/HydroGraph.h"

namespace proland
{

LazyHydroGraph::LazyHydroGraph() : LazyGraph()
{
}

LazyHydroGraph::~LazyHydroGraph()
{
}

CurvePtr LazyHydroGraph::newCurve(CurvePtr parent, bool setParent)
{
    CurveId id = nextCurveId;
    nextCurveId.id++;
    CurvePtr c = new LazyHydroCurve(this, id);
    curves.insert(make_pair(id, c.get()));

    if (curveOffsets.find(id) == curveOffsets.end()) {
        curveOffsets.insert(make_pair(id, (long int) - 1));
    }

    curveCache->add(c.get(), true);
    return c;
}

CurvePtr LazyHydroGraph::newCurve(CurvePtr model, NodePtr start, NodePtr end)
{
    CurveId id = nextCurveId;
    nextCurveId.id++;
    CurvePtr c = new LazyHydroCurve(this, id, start->getId(), end->getId());
    start->addCurve(id);
    end->addCurve(id);
    if (model != NULL) {
        map<CurveId, long int>::iterator ci = curveOffsets.find(model->getId());
        curveOffsets.insert(make_pair(id, ci->second));
    } else {
        curveOffsets.insert(make_pair(id, (long int) -1));
    }
    curves.insert(make_pair(id, c.get()));

    curveCache->add(c.get(), true);
    return c;
}

void LazyHydroGraph::movePoint(CurvePtr c, int i, const vec2d &p, set<CurveId> &changedCurves)
{
    Graph::movePoint(c, i, p, changedCurves);
//    if (c->getType() == HydroCurve::BANK) {
//        CurvePtr h = c.cast<HydroCurve>()->getRiverPtr();
//        float w = h->getWidth();
//        float minDistance = 1e8;
//        float maxDistance = 0.f;
//        for (int j = 0; j < c->getSize(); j++) {
//            float tmp = 1e8;
//            vec2f point = c->getXY(j);
//            vec2f prev = h->getXY(0);
//            vec2f cur;
//            for (int k = 1; k < h->getSize(); k++) {
//                cur = h->getXY(k);
//                float d = seg2f(prev, cur).segmentDistSq(point);
//                if (d < tmp) {
//                    tmp = d;
//                }
//                prev = cur;
//            }
//            if (tmp > maxDistance) {
//                maxDistance = tmp;
//            }
//            if (tmp < minDistance) {
//                minDistance = tmp;
//            }
//        }
//        if (w * w < minDistance) {
//            h->setWidth(ceil(sqrt(minDistance)) * 2.0f);
//            changedCurves.insert(h->getId());
//        } else if (w * w < maxDistance) {
//            h->setWidth(ceil(sqrt(maxDistance)) * 2.0f);
//            changedCurves.insert(h->getId());
//        }
//        printf("%f %f:%f (%f)\n", w, minDistance, maxDistance, h->getWidth());
//    }
}

NodePtr LazyHydroGraph::addNode(CurvePtr c, int i, Graph::Changes &changed)
{
    CurveId riverId = c.cast<HydroCurve>()->getRiver();
    float potential = c.cast<HydroCurve>()->getPotential();
    NodePtr n = Graph::addNode(c, i, changed);
    for (int i = 0; i < 2; i++) {
        CurvePtr cc = n->getCurve(i);
        if (!(cc->getId() == c->getId())) {
            cc.cast<HydroCurve>()->setRiver(riverId);
            cc.cast<HydroCurve>()->setPotential(potential);
        }
    }
    return n;
}

Graph *LazyHydroGraph::createChild()
{
    return new HydroGraph();
}

CurvePtr LazyHydroGraph::loadCurve(long int offset, CurveId id)
{
    assert(fileReader != NULL);
    CurveId nullCid;
    nullCid.id = NULL_ID;
    long int oldOffset = fileReader->tellg();
    int size, type, start, end;
    float width, potential;
    CurveId river;

    fileReader->seekg(offset, ios::beg);
    //CurvePtr c = new LazyCurve(this, id);
    ptr<LazyHydroCurve> c = new LazyHydroCurve(this, id);
    size = fileReader->read<int>();
    width = fileReader->read<float>();
    type = fileReader->read<int>();
    if (nParamsCurves >= 5) {
        potential = fileReader->read<float>();
        river.id = fileReader->read<int>();
    } else {
        potential = -1.f;
        river.id = NULL_ID;
    }
    //printf("%d:  %d:%f:%d:%f:%d\n", id.id, size, width, type, potential, river.id);
    for (int j = 5; j < nParamsCurves; j++) {
        fileReader->read<float>();
    }
    start = fileReader->read<int>();
    for (int j = 1; j < nParamsCurveExtremities; j++) {
        fileReader->read<float>();
    }

    c->setWidth(width);
    c->setType(type);
    c->setPotential(potential);

    c->setRiver(river);

    NodeId nis;
    nis.id = start;
    c->loadVertex(nis);

    for (int j = 1; j < size - 1; j++) {
        float x, y;
        int isControl;
        x = fileReader->read<float>();
        y = fileReader->read<float>();
        isControl = fileReader->read<int>();

        c->loadVertex(x, y, -1, isControl == 1);

        for (int j = 3; j < nParamsCurvePoints; j++) {
            fileReader->read<float>();
        }
    }

    end = fileReader->read<int>();
    for (int j = 3; j < nParamsCurveExtremities; j++) {
        fileReader->read<float>();
    }

    nis.id = end;
    c->loadVertex(nis);

    c->computeCurvilinearCoordinates();

    AreaId aid;
    aid.id = fileReader->read<int>();
    c->loadArea(aid);
    aid.id = fileReader->read<int>();
    c->loadArea(aid);
    fileReader->read<int>(); //parent
    fileReader->seekg(oldOffset, ios::beg);
    return c;
}

void LazyHydroGraph::save(FileWriter *fileWriter, bool saveAreas)
{
    return HydroGraph::save(this, fileWriter, saveAreas);
}

void LazyHydroGraph::indexedSave(FileWriter *fileWriter, bool saveAreas)
{
    return HydroGraph::indexedSave(this, fileWriter, saveAreas);
}

void LazyHydroGraph::checkParams(int nodes, int curves, int areas, int curveExtremities, int curvePoints, int areaCurves, int subgraphs)
{
    if (nodes < 2 || curves < 5 || areas < 3 || curveExtremities < 1 || curvePoints < 3 || areaCurves < 2) {
        Graph::checkDefaultParams(nodes, curves, areas, curveExtremities, curvePoints, areaCurves, subgraphs);
        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->log("RIVER", "Can't load file : Graph is not a HydroGraph. It will be loaded as a basic Graph");
        }
        return;
    }
}

}
