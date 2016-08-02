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

#include "proland/graph/BasicGraph.h"

namespace proland
{

BasicGraph::BasicGraph() :
    Graph()
{
}

bool BasicGraph::Cmp::operator()(const vec2d &u, const vec2d &v) const
{
    return u.x == v.x ? u.y < v.y : u.x < v.x;
}

BasicGraph::~BasicGraph()
{
    clean();
    clear();
}

void BasicGraph::clear()
{
    areas.clear();
    curves.clear();
    nodes.clear();
}

NodePtr BasicGraph::newNode(const vec2d &p)
{
    NodePtr n = new Node(this, p.x, p.y);
    if (mapping != NULL) {
        mapping->insert(make_pair(p, n.get()));
    }
    nodes.insert(make_pair(n->getId(), n));
    return n;
}

CurvePtr BasicGraph::newCurve(CurvePtr parent, bool setParent)
{
    CurvePtr c = new Curve(this);
    if (setParent) {
        c->setParent(parent);
        curves.insert(make_pair(c->getParentId(), c));
    } else {
        curves.insert(make_pair(c->getId(), c));
    }
    return c;
}

CurvePtr BasicGraph::newCurve(CurvePtr model, NodePtr start, NodePtr end)
{
    CurvePtr c = new Curve(this, model, start, end);
    start->addCurve(c->getId());
    end->addCurve(c->getId());
    curves.insert(make_pair(c->getId(), c));
    return c;
}

AreaPtr BasicGraph::newArea(AreaPtr parent, bool setParent)
{
    AreaPtr a = new Area(this);
    if (setParent) {
        a->setParent(parent);
        areas.insert(make_pair(parent->getId(), a));
    } else {
        areas.insert(make_pair(a->getId(), a));
    }
    return a;
}

AreaPtr BasicGraph::getChildArea(AreaId parentId)
{
    map<AreaId, AreaPtr>::const_iterator i = areas.find(parentId);
    if (i != areas.end()) {
        return i->second;
    } else {
        //assert(false);
        return NULL;
    }
}

void BasicGraph::removeNode(NodeId id)
{
    NodePtr n = getNode(id);
    if (n != NULL) {
        assert(n->getCurveCount() == 0);
        if (mapping != NULL) {
            map<vec2d, Node*, Cmp>::iterator i = mapping->find(n->getPos());
            if (i != mapping->end())
            {
                mapping->erase(i);
            }
        }

        n = NULL;
        map<NodeId, NodePtr>::iterator i = nodes.find(id);
        if (i != nodes.end())
        {
            nodes.erase(i);
        }
    }
}

// TODO : Handle the removedCurves && removedAreas lists
// Maybe transform those lists to ptr lists
void BasicGraph::removeCurve(CurveId id)
{
    CurvePtr c = getCurve(id);
    if (c != NULL) {
        assert(c->area1.id == NULL_ID && c->area2.id == NULL_ID);
        NodePtr start = c->getStart();
        NodePtr end = c->getEnd();
        start->removeCurve(id);
        if (start != end) {
            end->removeCurve(id);
            if (start->getCurveCount() == 0) {
                NodeId sid = start->getId();
                start = NULL;
                removeNode(sid);
            }
        } else {
            start = NULL;
        }
        if (end->getCurveCount() == 0) {
            NodeId eid = end->getId();
            end = NULL;
            removeNode(eid);
        }

        typedef multimap<CurveId, CurvePtr>::iterator MI;
        pair<MI, MI> range = c->getParent() == NULL ? curves.equal_range(c->getId()) : curves.equal_range(c->getParentId());
        MI i = range.first;
        MI n = range.second;

        while (i != n) {
            if (i->second->getId() == id) {
                curves.erase(i);
                break;
            }
            ++i;
        }

        removedCurves.push_back(c);
    }
}
void BasicGraph::removeArea(AreaId id)
{
    AreaPtr a = getArea(id);
    if (a != NULL) {
        while (a->getCurveCount()) {
            int orientation;
            a->getCurve(0, orientation)->removeArea(id);
            a->removeCurve(0);
        }
        map<AreaId, AreaPtr>::iterator i = areas.find(a->getParent() == NULL ? a->getId() : a->getParentId());
        assert(i != areas.end());
        removedAreas.push_back(i->second);
        areas.erase(i);
    }
}

void BasicGraph::clean()
{
    removedCurves.clear();
    removedAreas.clear();
}

void BasicGraph::load(const string &file, bool loadSubgraphs)
{
    clear();

    bool isIndexed = false;
    FileReader *fileReader = new FileReader(file, isIndexed);

    if (isIndexed) {
        loadIndexed(fileReader, loadSubgraphs);
    } else {
        load(fileReader, loadSubgraphs);
    }

    delete fileReader;
}

void BasicGraph::loadIndexed(FileReader *fileReader, bool loadSubgraphs)
{
    assert(fileReader != 0);
    int nodeCount;
    int curveCount;
    int areaCount;
    int subgraphCount;

    nParamsNodes = fileReader->read<int>();
    nParamsCurves = fileReader->read<int>();
    nParamsAreas = fileReader->read<int>();
    nParamsCurveExtremities = fileReader->read<int>();
    nParamsCurvePoints = fileReader->read<int>();
    nParamsAreaCurves = fileReader->read<int>();
    nParamsSubgraphs = fileReader->read<int>();

    checkParams(nParamsNodes, nParamsCurves, nParamsAreas, nParamsCurveExtremities, nParamsCurvePoints, nParamsAreaCurves, nParamsSubgraphs);
    long int offset = fileReader->read<long int>();
    long int begin = fileReader->tellg();

    fileReader->seekg(offset, ios::beg);

    nodeCount = fileReader->read<int>();
    curveCount = fileReader->read<int>();
    areaCount = fileReader->read<int>();
    subgraphCount = fileReader->read<int>();

    fileReader->seekg(begin, ios::beg);

    vector<NodePtr> nodesTMP(nodeCount);
    for (int i = 0; i < nodeCount; i++) {
        float x, y;
        x = fileReader->read<float>();
        y = fileReader->read<float>();
        nodesTMP[i] = newNode(vec2d(x, y));
        for (int j = 2; j < nParamsNodes; j++) {
            fileReader->read<float>();
        }
        int size = fileReader->read<int>();
        for (int j = 0; j < size; j++) {
            fileReader->read<int>();
        }
    }

    vector<CurvePtr> curvesTMP(curveCount);
    for (int i = 0; i < curveCount; i++) {
        int size;
        float width;
        int type;
        size = fileReader->read<int>();
        width = fileReader->read<float>();
        type = fileReader->read<int>();
        for (int j = 3; j < nParamsCurves; j++) {
            fileReader->read<float>();
        }

        int start = fileReader->read<int>();
        for (int j = 1; j < nParamsCurveExtremities; j++) {
            fileReader->read<float>();
        }
        vector<Vertex> v;
        for (int j = 1; j < size - 1; j++) {
            float x, y;
            int isControl;
            x = fileReader->read<float>();
            y = fileReader->read<float>();
            isControl = fileReader->read<int>();
            for (int k = 3; k < nParamsCurvePoints; k++) {
                fileReader->read<float>();
            }
            v.push_back(Vertex(x, y, -1, isControl == 1));
        }
        int end = fileReader->read<int>();
        for (int j = 1; j < nParamsCurveExtremities; j++) {
            fileReader->read<float>();
        }
        fileReader->read<int>();
        fileReader->read<int>();
        CurveId parentId;
        parentId.id= (unsigned int) fileReader->read<int>();

        CurvePtr c;
        if (getParent() != NULL) {
            c = newCurve(getParent()->getCurve(parentId), parentId.id != NULL_ID);
        } else {
            c = newCurve(NULL, false);
        }
        c->width = width;
        c->type = type;
        c->addVertex(nodesTMP[start]->getId());
        c->addVertex(nodesTMP[end]->getId());
        nodesTMP[start]->addCurve(c->getId());
        nodesTMP[end]->addCurve(c->getId());
        for (vector<Vertex>::iterator j = v.begin(); j != v.end(); j++) {
            c->addVertex(*j);
        }

        c->computeCurvilinearCoordinates();
        curvesTMP[i] = c;
    }
    vector<AreaPtr> areasTMP(areaCount);
    for (int i = 0; i < areaCount; i++) {
        int size;
        int info;
        int subgraph;
        size = fileReader->read<int>();
        info = fileReader->read<int>();
        subgraph = fileReader->read<int>();
        for (int j = 3; j < nParamsAreas; j++) {
            fileReader->read<float>();
        }

        vector<pair<int, int> > v;
        for (int j = 0; j < size; j++) {
            int index;
            int orientation;
            index = fileReader->read<int>();
            orientation = fileReader->read<int>();

            for (int k = 2; k < nParamsAreaCurves; k++) {
                fileReader->read<float>();
            }
            v.push_back(make_pair(index, orientation));
        }
        AreaId parentId;
        for (int j = 0; j < nParamsSubgraphs; j++) {
            fileReader->read<float>();
        }
        parentId.id= (unsigned int) fileReader->read<int>();

        AreaPtr a;
        if (getParent() != NULL) {
            a = newArea(getParent()->getArea(parentId), parentId.id != NULL_ID);
        } else {
            a = newArea(NULL, false);
        }
        a->info = info;
        a->subgraph = loadSubgraphs ? (subgraph == 1 ? createChild() : NULL) : NULL;
        for (vector<pair<int, int> >::iterator j = v.begin(); j != v.end(); j++) {
            a->addCurve(curvesTMP[j->first]->getId(), j->second);
            curvesTMP[j->first]->addArea(a->getId());
        }

        areasTMP[i] = a;
    }

    for (int i = 0; i < areaCount; ++i) {
        AreaPtr a = areasTMP[i];
        if (a->subgraph != NULL) {
            a->subgraph->load(fileReader, loadSubgraphs);
        }
    }
}

void BasicGraph::load(FileReader *fileReader, bool loadSubgraphs)
{
    int nodeCount;
    int curveCount;
    int areaCount;
    nParamsNodes = fileReader->read<int>();
    nParamsCurves = fileReader->read<int>();
    nParamsAreas = fileReader->read<int>();
    nParamsCurveExtremities = fileReader->read<int>();
    nParamsCurvePoints = fileReader->read<int>();
    nParamsAreaCurves = fileReader->read<int>();
    nParamsSubgraphs = fileReader->read<int>();

    checkParams(nParamsNodes, nParamsCurves, nParamsAreas, nParamsCurveExtremities, nParamsCurvePoints, nParamsAreaCurves, nParamsSubgraphs);

    nodeCount = fileReader->read<int>();
    vector<NodePtr> nodesTMP(nodeCount);
    for (int i = 0; i < nodeCount; i++) {
        float x, y;
        x = fileReader->read<float>();
        y = fileReader->read<float>();
        for (int j = 2; j < nParamsNodes; j++) {
            fileReader->read<float>();
        }
        int s = fileReader->read<int>();
        for (int j = 0; j < s; j++) {
            fileReader->read<int>();
        }
        nodesTMP[i] = newNode(vec2d(x, y));
    }
    curveCount = fileReader->read<int>();
    vector<CurvePtr> curvesTMP(curveCount);
    for (int i = 0; i < curveCount; i++) {
        int size;
        float width;
        int type;
        size = fileReader->read<int>();
        width = fileReader->read<float>();
        type = fileReader->read<int>();
        for (int j = 3; j < nParamsCurves; j++) {
            fileReader->read<float>();
        }

        int start = fileReader->read<int>();
        for (int j = 1; j < nParamsCurveExtremities; j++) {
            fileReader->read<float>();
        }
        vector<Vertex> v;
        for (int j = 1; j < size - 1; j++) {
            float x, y;
            int isControl;
            x = fileReader->read<float>();
            y = fileReader->read<float>();
            isControl = fileReader->read<int>();
            v.push_back(Vertex(x, y, -1, isControl == 1));
            for (int k = 3; k < nParamsCurvePoints; k++) {
                fileReader->read<float>();
            }
        }
        int end = fileReader->read<int>();
        for (int j = 1; j < nParamsCurveExtremities; j++) {
            fileReader->read<float>();
        }
        fileReader->read<int>();
        fileReader->read<int>();
        CurveId parentId;
        parentId.id= (unsigned int) fileReader->read<int>();

        CurvePtr c;
        if (getParent() != NULL) {
            c = newCurve(getParent()->getCurve(parentId), parentId.id != NULL_ID);
        } else {
            c = newCurve(NULL, false);
        }
        c->width = width;
        c->type = type;
        c->addVertex(nodesTMP[start]->getId());
        c->addVertex(nodesTMP[end]->getId());
        nodesTMP[start]->addCurve(c->getId());
        nodesTMP[end]->addCurve(c->getId());
        for (vector<Vertex>::iterator j = v.begin(); j != v.end(); j++) {
            c->addVertex(*j);
        }

        c->computeCurvilinearCoordinates();
        curvesTMP[i] = c;
    }
    areaCount = fileReader->read<int>();
    vector<AreaPtr> areasTMP(areaCount);
    for (int i = 0; i < areaCount; i++) {
        int size;
        int info;
        int subgraph;

        size = fileReader->read<int>();
        info = fileReader->read<int>();
        subgraph = fileReader->read<int>();
        for (int j = 3; j < nParamsAreas; j++) {
            fileReader->read<float>();
        }
        vector<pair<int, int> > v;
        for (int j = 0; j < size; j++) {
            int index;
            int orientation;
            index = fileReader->read<int>();
            orientation = fileReader->read<int>();
            for(int k = 2; k < nParamsAreaCurves; k++) {
                fileReader->read<float>();
            }
            v.push_back(make_pair(index, orientation));
        }
        for(int j = 0; j < nParamsSubgraphs; j++) {
            fileReader->read<float>();
        }
        AreaId parentId;
        parentId.id= (unsigned int) fileReader->read<int>();

        AreaPtr a;
        if (getParent() != NULL) {
            a = newArea(getParent()->getArea(parentId), parentId.id != NULL_ID);
        } else {
            a = newArea(NULL, false);
        }
        a->info = info;
        a->subgraph = loadSubgraphs ? (subgraph == 1 ? createChild() : NULL) : NULL;
        for (vector<pair<int, int> >::iterator j = v.begin(); j != v.end(); j++) {
            a->addCurve(curvesTMP[j->first]->getId(), j->second);
            curvesTMP[j->first]->addArea(a->getId());
        }

        areasTMP[i] = a;
    }
    for (int i = 0; i < areaCount; ++i) {
        AreaPtr a = areasTMP[i];
        if (a->subgraph != NULL) {
            a->subgraph->load(fileReader, loadSubgraphs);
        }
    }
}

}
