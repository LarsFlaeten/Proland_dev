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

#include "proland/graph/producer/CurveDataFactory.h"

#include "proland/graph/producer/GraphProducer.h"

namespace proland
{

CurveDataFactory::CurveDataFactory()
{
}

CurveDataFactory::CurveDataFactory(ptr<TileProducer> producer)
{
    init(producer);
}

void CurveDataFactory::init(ptr<TileProducer> producer)
{
    assert(producer != NULL);
    this->producer = producer;
    producer.cast<GraphProducer>()->getRoot()->addListener(this);
}

CurveDataFactory::~CurveDataFactory()
{
    assert(producer != NULL);
    producer.cast<GraphProducer>()->getRoot()->removeListener(this);
    producer = NULL;
    clear();
}

CurveData *CurveDataFactory::newCurveData(CurveId id, CurvePtr flattenCurve)
{
    return new CurveData(id, flattenCurve);
}

void CurveDataFactory::graphChanged()
{
    assert(producer != NULL);
    Graph::Changes changes = producer.cast<GraphProducer>()->getRoot()->changes;
    set<CurveId>::const_iterator end = changes.addedCurves.end();
    set<CurveId>::const_iterator i = changes.addedCurves.begin();
    while (i != end) {
        CurveId id = (*i);
        map<CurveId, CurveData*>::iterator j = curveDatas.find(id);
        if (j != curveDatas.end()) {
            CurveData *d = j->second;
            CurvePtr c = producer.cast<GraphProducer>()->getRoot()->getCurve(id);
            CurveData *data = newCurveData(id, producer.cast<GraphProducer>()->getFlattenCurve(c));
            usedDataCount[data] = usedDataCount[d];
            usedDataCount.erase(usedDataCount.find(d));
            curveDatas.erase(j);
            delete d;
            curveDatas.insert(pair<CurveId, CurveData*>(id, data));
        }
        i++;
    }

    end = changes.removedCurves.end();
    i = changes.removedCurves.begin();
    while (i != end) {
        CurveId id = *i++;
        if (changes.addedCurves.find(id) == changes.addedCurves.end()) {
            map<CurveId, CurveData*>::iterator j = curveDatas.find(id);
            if (j != curveDatas.end()) {
                CurveData *d = j->second;
                usedDataCount.erase(usedDataCount.find(d));
                curveDatas.erase(j);
                delete d;
            }
        }
    }
}

void CurveDataFactory::clear()
{
    for(map<CurveId, CurveData*>::iterator i = curveDatas.begin(); i != curveDatas.end(); i++) {
        delete i->second;
    }
    curveDatas.clear();
    usedDataCount.clear();
    usedDatas.clear();
}

CurveData *CurveDataFactory::getCurveData(CurvePtr c)
{
    assert(producer != NULL);
    CurvePtr pc = c->getAncestor();
    CurveId id = pc->getId();
    map<CurveId, CurveData*>::iterator i = curveDatas.find(id);
    if (i == curveDatas.end()) {
        CurveData *data = newCurveData(id, producer.cast<GraphProducer>()->getFlattenCurve(pc));
        curveDatas.insert(pair<CurveId, CurveData*>(id, data));
        usedDataCount.insert(pair<CurveData*, int>(data, 1));
        return data;
    } else {
        usedDataCount[i->second] = usedDataCount[i->second] + 1;
        return i->second;
    }
}

void CurveDataFactory::putCurveData(CurveId id)
{
    assert(producer != NULL);
    map<CurveId, CurveData*>::iterator i = curveDatas.find(id);
    if (i == curveDatas.end()) {
        //printf("couldn't put curveData for curve : %d\n", id.id);
        return;
    }

    usedDataCount[i->second] = usedDataCount[i->second] - 1;
    if (usedDataCount[i->second] == 0) {
        usedDataCount.erase(i->second);
        delete i->second;
        producer.cast<GraphProducer>()->putFlattenCurve(id);
        curveDatas.erase(id);
    }
}

CurveData *CurveDataFactory::findCurveData(CurvePtr c)
{
    CurvePtr pc = c->getAncestor();
    CurveId id = pc->getId();
    map<CurveId, CurveData*>::iterator i = curveDatas.find(id);
    if (i == curveDatas.end()) {
        printf("couldn't find curveData for curve : %d\n", id.id);
    }
    assert(i != curveDatas.end());
    return i->second;
}

void CurveDataFactory::addUsedCurveDatas(int level, int tx, int ty, set<CurveId> curveDatas)
{
    TileCache::Tile::Id id = TileCache::Tile::getId(level, tx, ty);
    usedDatas.insert(make_pair(id, curveDatas));
}

void CurveDataFactory::releaseCurveData(int level, int tx, int ty)
{
    TileCache::Tile::Id id = TileCache::Tile::getId(level, tx, ty);
    map<TileCache::Tile::Id, set<CurveId> >::iterator curveIds = usedDatas.find(id);
    if (curveIds != usedDatas.end()) {
        for(set<CurveId>::iterator i = curveIds->second.begin(); i != curveIds->second.end(); i++) {
            putCurveData(*i);
        }
        curveIds->second.clear();
        usedDatas.erase(curveIds);
    }
}

void CurveDataFactory::swap(CurveDataFactory* factory)
{
    std::swap(usedDatas, factory->usedDatas);
    std::swap(producer, factory->producer);
    std::swap(curveDatas, factory->curveDatas);
    std::swap(usedDataCount, factory->usedDataCount);
}

}
