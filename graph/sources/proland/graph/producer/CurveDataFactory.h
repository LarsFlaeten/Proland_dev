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

#ifndef _PROLAND_CURVEDATAFACTORY_H_
#define _PROLAND_CURVEDATAFACTORY_H_

#include "proland/producer/TileProducer.h"
#include "proland/graph/GraphListener.h"
#include "proland/graph/producer/CurveData.h"

namespace proland
{

/**
 * CurveData Factory. Handles creation, access and destruction of CurveDatas, and allows to instantiate different types of CurveData.
 * @ingroup producer
 * @author Antoine Begault
 */
PROLAND_API class CurveDataFactory : public GraphListener
{
public:
    /**
     * Creates a new CurveDataFactory.
     */
    CurveDataFactory();

    /**
     * Creates a new CurveDataFactory.
     *
     * @param producer the GraphProducer used to generate Curves and their flattened version.
     */
    CurveDataFactory(ptr<TileProducer> producer);

    /**
     * Deletes this CurveDataFactory.
     */
    virtual ~CurveDataFactory();

    /**
     * Returns a new CurveData, whose type depends on the class that implements CurveDataFactory.
     *
     * @param id the CurveId of the described Curve.
     * @param flattenCurve flattened version of the described Curve.
     */
    virtual CurveData *newCurveData(CurveId id, CurvePtr flattenCurve);

    /**
     * See GraphListener#graphChanged().
     */
    virtual void graphChanged();

    /**
     * Returns the CurveData corresponding to the given Curve.
     * Handles a count of references.
     *
     * @param c a Curve.
     */
    CurveData *getCurveData(CurvePtr c);

    /**
     * Releases the CurveData corresponding to the given CurveId. The reference count is decreased, and if it is equal to zero,  the CurveData is deleted.
     * It will then call GraphProducer#putFlattenCurve().
     *
     * @param id the CurveId of an existing CurveData.
     */
    void putCurveData(CurveId id);

    /**
     * Returns the CurveData corresponding to the given Curve. Doesn't change the reference count.
     *
     * @param c a Curve.
     */
    CurveData *findCurveData(CurvePtr c);

    /**
     * Releases every used CurveData corresponding to a given tile.
     *
     * @param level level of the Tile.
     * @param x x coordinate of the Tile.
     * @param y y coordinate of the Tile.
     */
    void releaseCurveData(int level, int tx, int ty);

    /**
     * Adds a list of Curves used by a tile of this factory. They will require a call to #releaseCurveData.
     *
     * @param level level of the Tile.
     * @param x x coordinate of the Tile.
     * @param y y coordinate of the Tile.
     * @param curveDatas list of Ids corresponding to the used CurveData.
     */
    void addUsedCurveDatas(int level, int tx, int ty, set<CurveId> curveDatas);

    /**
     * Deletes every CurveData.
     */
    void clear();

    bool hasCurveData(CurveId id) {
        return curveDatas.find(id) != curveDatas.end();
    }

protected:
    /**
     * Initializes CurveDataFactory fields.
     *
     * @param producer the GraphProducer used to generate Curves and their flattened version.
     */
    void init(ptr<TileProducer> producer);

    virtual void swap(CurveDataFactory* factory);

    /**
     * The GraphProducer used to generate Curves and their flattened version.
     */
    ptr<TileProducer> producer;

    /**
     * List of every CurveData and their associated CurveId.
     */
    map<CurveId, CurveData*> curveDatas;

    /**
     * Reference count for each CurveData.
     */
    map<CurveData*, int> usedDataCount;

    /**
     * Maps every tile to the CurveDatas it uses.
     */
    map<TileCache::Tile::Id, set<CurveId> > usedDatas;
};

}

#endif
