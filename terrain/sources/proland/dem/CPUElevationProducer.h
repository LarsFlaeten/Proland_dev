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

#ifndef _PROLAND_ELEVATION_PRODUCER_H_
#define _PROLAND_ELEVATION_PRODUCER_H_

#include "proland/producer/TileProducer.h"

namespace proland
{

/**
 * A TileProducer to create elevation tiles on CPU from CPU residual tiles.
 * @ingroup dem
 * @authors Antoine Begault, Eric Bruneton
 */
PROLAND_API class CPUElevationProducer : public TileProducer
{
public:
    /**
     * Creates a new CPUElevationProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a CPUTileStorage of float type.
     * @param residualTiles the %producer producing the residual tiles. This
     *      %producer should produce its tiles in a CPUTileStorage of float type.
     *      The size of the residual tiles (without borders) must be a multiple
     *      of the size of the elevation tiles (without borders).
     */
    CPUElevationProducer(ptr<TileCache> cache, ptr<TileProducer> residualTiles);

    /**
     * Deletes this CPUElevationProducer.
     */
    virtual ~CPUElevationProducer();

    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const;

    virtual void setRootQuadSize(float size);

    virtual int getBorder();

    virtual bool prefetchTile(int level, int tx, int ty);

    /**
     * Returns the %terrain altitude at a given point, at a given level.
     * The corresponding tile must be in cache before calling this method.
     *
     * @param producer a CPUElevationProducer or an equivalent (i.e. a %
     *      producer using an underlying CPUTileStorage of float type).
     * @param level level at which we want to get the altitude.
     * @param x physical x coordinate of the point to get (in meters from the %terrain center).
     * @param y physical y coordinate of the point to get (in meters from the %terrain center).
     */
    static float getHeight(ptr<TileProducer> producer, int level, float x, float y);

protected:
    /**
     * Creates an uninitialized CPUElevationProducer.
     */
    CPUElevationProducer();

    /**
     * Initializes this CPUElevationProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a CPUTileStorage of float type.
     * @param residualTiles the %producer producing the residual tiles. This
     *      %producer should produce its tiles in a CPUTileStorage of float type.
     *      The size of the residual tiles (without borders) must be a multiple
     *      of the size of the elevation tiles (without borders).
     */
    void init(ptr<TileCache> cache, ptr<TileProducer> residualTiles);

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual void beginCreateTile();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void endCreateTile();

    virtual void stopCreateTile(int level, int tx, int ty);

    virtual void swap(ptr<CPUElevationProducer> p);

private:
    /**
     * The %producer producing the residual tiles. This %producer should produce
     * its tiles in a CPUTileStorage of float type. The size of the residual tiles
     * (without borders) must be a multiple of the size of the elevation tiles
     * (without borders).
     */
    ptr<TileProducer> residualTiles;
};

}

#endif
