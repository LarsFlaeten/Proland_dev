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

#ifndef _PROLAND_HYDROFLOWPRODUCER_H
#define _PROLAND_HYDROFLOWPRODUCER_H

#include "proland/graph/producer/GraphProducer.h"
#include "proland/graph/producer/CurveDataFactory.h"

namespace proland
{

#define BASEWIDTH(width, scale) ((width) + 2.0f * sqrt(2.0f)/(scale))

#define TOTALWIDTH(basewidth) ((basewidth) * 3.0f)

/**
 * Produces the required HydroFlowData for a given tile at a given level.
 * Uses Graphs from a GraphProducer to create curves contained in a HydroFlowData. It then stores them as a simplified version in another graph.
 * The simplified version only contains the <i>visible</i> curves (larger than 1 pixel) from a Graph.
 * @ingroup rivers
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class HydroFlowProducer : public TileProducer, public CurveDataFactory
{
public:
    /**
     * A Margin adapted for river graphs.
     */
    class RiverMargin : public Margin
    {
    public:
        /**
         * Creates a new Margin.
         */
        RiverMargin(int samplesPerTile, float borderFactor);

        virtual double getMargin(double clipSize);

        virtual double getMargin(double clipSize, CurvePtr p);

    private:
        /**
         * Border factor.
         */
        float borderFactor;

        /**
         * Number of samples per Tile.
         */
        int samplesPerTile;
    };

    /**
     * Creates a new HydroFlowProducer.
     *
     * @param graphs the GraphProducer used to create Graphs.
     * @param cache the tile cache that stores the tiles produced by this
     *      %producer.
     * @param displayTileSize size of a displayed tile. Determines which
     *      curves will be added to the HydroFlowData (only those whose width
     *      is larger than 1 pixel will be taken into account).
     * @param slipParameter determines slip conditions.
     * @param searchRadiusFactor determines the radius of a DistCell coverage.
     * @param potentialDelta radius used for potential computation.
     * @param minLevel minimum level to start creating tiles.
     */
    HydroFlowProducer(ptr<GraphProducer> graphs,
        ptr<TileCache> cache, int displayTileSize, float slipParameter,
        float searchRadiusFactor, float potentialDelta, int minLevel);

    /**
     * Deletes this HydroFlowProducer.
     */
    virtual ~HydroFlowProducer();

    /**
     * Returns the GraphProducer used by this HydroFlowProducer.
     */
    ptr<GraphProducer> getGraphProducer();

    /**
     * Returns the displayed tile size. This is equivalent to the size displayed in ortho layer.
     */
    int getTileSize();

    /**
     * Returns the slip parameter.
     */
    float getSlipParameter();

    /**
     * Changes the slip parameter.
     */
    void setSlipParameter(float slip);

    /**
     * Returns the potential delta parameter.
     */
    float getPotentialDelta();

    /**
     * Changes the potential delta parameter.
     */
    void setPotentialDelta(float delta);

    virtual float getRootQuadSize();

    virtual void setRootQuadSize(float size);

    virtual int getBorder();

    virtual TileCache::Tile* getTile(int level, int tx, int ty, unsigned int deadline);

    virtual void putTile(TileCache::Tile *t);

    virtual void getReferencedProducers(vector< ptr<TileProducer> > &producers) const;

    virtual void graphChanged();

    vec3d getTileCoords(int level, int tx, int ty);

    inline float getTileBorder()
    {
        return getBorder();
    }

    /**
     * Adds a list of tiles used by each tile of this layer. They will
     * require a call to TileProducer#put() when the task has been done.
     *
     * @param level the level of the tile.
     * @param tx x coordinate of the tile.
     * @param ty y coordinate of the tile.
     * @param producer the Producer from which the Tiles were produced.
     * @param tiles the used tiles that will need to be released.
     */
    void addUsedTiles(int level, int tx, int ty, TileProducer *producer,
            set<TileCache::Tile*> tiles);

protected:
    /**
     * Creates a new HydroFlowProducer.
     */
    HydroFlowProducer();

    /**
     * Initializes HydroFlowProducer fields.
     *
     * See #HydroFlowProducer.
     */
    void init(ptr<GraphProducer> graphs,
        ptr<TileCache> cache, int displayTileSize, float slipParameter,
        float searchRadiusFactor, float potentialDelta, int minLevel);

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void stopCreateTile(int level, int tx, int ty);

    virtual void swap(ptr<HydroFlowProducer> p);

private:
    /**
     * The GraphProducer used to create Graphs.
     */
    ptr<GraphProducer> graphs;

    /**
     * Size of a displayed tile. Determines which curves will be added
     * to the HydroFlowData (only those whose width is larger than 1
     * pixel will be taken into account).
     */
    int displayTileSize;

    /**
     * Determines slip conditions.
     */
    float slipParameter;

    /**
    * Determines the radius of a DistCell coverage.
    */
    float searchRadiusFactor;

    /**
     * Radius used for potential computation.
     */
    float potentialDelta;

    /**
     * The tiles currently in use. These tiles cannot be evicted from the cache
     * and from the TileStorage, until they become unused. Maps tile identifiers
     * to used tiles and to the TileProducer that produces those tiles.
     */
    map<TileCache::Tile::Id, pair<TileProducer *, set<TileCache::Tile*> > > usedTiles;

    /**
     * Minimum level to start creating tiles.
     */
    int minLevel;

    friend class HydroFlowTile;

};

}

#endif
