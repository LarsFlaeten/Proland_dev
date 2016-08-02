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

#ifndef _PROLAND_GRAPH_PRODUCER_H_
#define _PROLAND_GRAPH_PRODUCER_H_

#include "ork/resource/ResourceTemplate.h"
#include "proland/producer/TileProducer.h"
#include "proland/graph/LazyGraph.h"
#include "proland/graph/BasicGraph.h"
#include "proland/graph/ComposedMargin.h"
#include "proland/graph/GraphListener.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup producer producer
 * Provides a tile producer and an abstract tile layer for graphs.
 * @ingroup graph
 */

/**
 * Produces the required graphs at a given tile and at a given level.
 * Produced tiles will be used in layers (elevation, normals, ortho...)
 * to display corresponding elements (edit, road, rivers...).
 * @ingroup producer
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class GraphProducer : public TileProducer, public GraphListener
{
public:
    /**
     * Creates Graph objects. GraphFactory is used to determine what kind
     * of graph will be created in a GraphProducer.
     */
    class GraphFactory : public Object
    {
    public:
        GraphFactory();

        virtual ~GraphFactory();

        /**
         * Creates a Graph. Should be overriden for every type of Graph.
         */
        virtual Graph *newGraph(int nodeCacheSize, int curveCacheSize,
                int areaCacheSize);

        virtual void swap(ptr<GraphFactory> g);
    };

    /**
     * A GraphFactory that creates LazyGraph.
     */
    class LazyGraphFactory : public GraphFactory
    {
    public:
        LazyGraphFactory();

        virtual ~LazyGraphFactory();

        virtual Graph *newGraph(int nodeCacheSize, int curveCacheSize,
                int areaCacheSize);
    };

    /**
     * Cache that manages the precomputed Graphs (including root Graph).
     * It can load directly from the disk every tiles at a given level. If the
     * file doesn't exist, the tile will be created from the root Graph (in
     * GraphProducer#doCreateTile()) and then stored in cache and saved to disk.
     */
    class GraphCache : public Object {
    public:

        /**
         * Creates a new GraphCache.
         *
         * @param root the root Graph.
         * @param graphName the name of the file from which we loaded the root
         *      Graph. This is used to load and save the Graphs at level
         *      GraphProducer#precomputedLevel, if different from 0.
         * @param manager the resource manager that will load the .graph files.
         * @param loadSubgraphs whether to load subgraphs or not.
         */
        GraphCache(GraphPtr root,  string graphName, ptr<ResourceManager> manager,
                bool loadSubgraphs = false);

        /**
         * Deletes this GraphCache.
         */
        ~GraphCache();

        /**
         * Returns the graph corresponding to a precomputed Tile. If it's in
         * cache, it just returns it. If not, it checks if it can load and
         * return it. If it's not available, returns NULL. It will then be
         * recomputed from the root Tile in GraphProducer, added to the cache,
         * and saved on the disk.
         *
         * @param tileId the id of the tile to load.
         * @return the corresponding Graph.
         */
        GraphPtr getTile(TileCache::Tile::Id tileId);

        /**
         * Adds a graph to the cache and save it to the disk.
         *
         * @param id the id of the tile to add.
         * @param graph the corresponding Graph.
         */
        void add(TileCache::Tile::Id id, GraphPtr graph);

    protected:
        virtual void swap(ptr<GraphCache> p);

    private:
        /**
         * Determines whether to load subgraphs or not.
         */
        bool loadSubgraphs;

        /**
         * The name of the file from which we loaded the root Graph. This is
         * used to load and save the Graphs at level
         * GraphProducer#precomputedLevel, if different from 0.
         */
        string graphName;

        /**
         * The resource manager used to load .graph files.
         */
        ptr<ResourceManager> manager;

        /**
         * Maps Tile Ids with Graphs.
         */
        map<TileCache::Tile::Id, GraphPtr> graphs;
    };

    /**
     * Creates a new GraphProducer.
     *
     * @param name the name of this resource.
     * @param cache the tile cache that stores the tiles produced by this
     *      %producer.
     * @param precomputedGraphs the precomputed graphs cache. Manages the
     *      precomputed Graphs (including root Graph).
     * @param precomputedLevels levels at which we can directly load the
     *      graphs from the disk. For each tile of those levels, the
     *      corresponding graph must be precomputed and stored in a directory
     *      with the name of the graph. If not existing, it will be computed
     *      from the root tile and stored in cache. level 1 -> l-1 will be
     *      computed from the root tile if required afterwards.
     * @param doFlatten if true, the producer will call Graph#flatten()
     *      on each produced subgraph after clipping. Default = false.
     * @param flatnessFactor factor for flatten() method (square of the
     *      maximum allowed distance between a limit curve and its polyline
     *      approximation). Default = 0.5f.
     * @param storeParents if true, we keep each tile in the memory cache
     *      instead of deleting them when temporarily not used.
     *      Default = false.
     * @param maxNodes maximum amount of nodes allowed in a Graph. If under that
     *      number, the graph is considered small enough to not have to be
     *      clipped, and will be used for all its children.
     *      If set to 0, the Graph will be clipped as usual.
     */
    GraphProducer(string name, ptr<TileCache> cache,
            ptr<GraphCache> precomputedGraphs, set<int> precomputedLevels,
            bool doFlatten = false, float flatnessFactor = 0.5f,
            bool storeParents = false, int maxNodes = 0);

    /**
     * Creates a new GraphProducer. No precomputed levels.
     *
     * @param name the name of this resource.
     * @param cache the tile cache that stores the tiles produced by this
     *      %producer.
     * @param precomputedGraphs the precomputed graphs cache. Manages the
     *      precomputed Graphs (including root Graph).
     * @param doFlatten if true, the producer will call Graph#flatten()
     *      on each produced subgraph after clipping. Default = false.
     * @param flatnessFactor factor for flatten() method (square of the
     *      maximum allowed distance between a limit curve and its polyline
     *      approximation). Default = 0.5f.
     * @param storeParents if true, we keep each tile in the memory cache
     *      instead of deleting them when temporarily not used.
     *      Default = false.
     * @param maxNodes maximum amount of nodes allowed in a Graph. If under that
     *      number, the graph is considered small enough to not have to be
     *      clipped, and will be used for all its children.
     *      If set to 0, the Graph will be clipped as usual.
     */
    GraphProducer(string name, ptr<TileCache> cache,
            ptr<GraphCache> precomputedGraphs,
            bool doFlatten = false, float flatnessFactor = 0.5f,
            bool storeParents = false, int maxNodes = 0);

    /**
     * Deletes this GraphProducer.
     */
    virtual ~GraphProducer();

    /**
     * Returns the level 0 Graph.
     */
    GraphPtr getRoot();

    virtual int getBorder();

    /**
     * Sets tileSize value.
     *
     * @param tileSize the new tileSize.
     */
    void setTileSize(int tileSize);

    /**
     * Adds a margin to this graphProducer.
     * Each layer may have a different margin, and only the largest will be
     * used for the Graph#clip() method.
     */
    virtual void addMargin(Margin *m);

    /**
     * Removes a given margin.
     *
     * @param m a margin.
     */
    virtual void removeMargin(Margin *m);

    virtual TileCache::Tile* getTile(int level, int tx, int ty, unsigned int deadline);

    virtual void putTile(TileCache::Tile *t);

    /**
     * Invalidates the root tile and thus forces to recompute changed tiles
     * (and only changed tiles).
     */
    virtual void graphChanged();

    /**
     * Returns the precomputed graphs cache.
     */
    inline ptr<GraphCache> getPrecomputedGraphs()
    {
         return precomputedGraphs;
    }

    /**
     * Returns the first precomputed level after level 0. Returns level 0 if no other precomputed levels are specified.
     * First Level at which we can directly load the graphs from the disk.
     * For each tile of those levels, the corresponding graph must be precomputed
     * and stored in a directory with the name of the graph. If not existing,
     * it will be computed from the root tile and stored in cache. level 1 ->
     * l-1 will be computed from the root tile if required afterwards.
     */
    inline int getFirstPrecomputedLevel()
    {
        if ((int) precomputedLevels.size() > 1) {
            return *(precomputedLevels.lower_bound(1));
        }
        return 0;
    }

    bool isPrecomputedLevel(int level);

    /**
     * Returns the flattened Curve corresponding to a given Curve.
     * Handles a reference count of flattenCurves.
     *
     * @param c the Curve for which we want a flattened version.
     */
    CurvePtr getFlattenCurve(CurvePtr c);

    /**
     * Releases a given FlattenCurve.
     *
     * @param id the id of the corresponding Curve.
     */
    void putFlattenCurve(CurveId id);

    /**
     * Returns the name of this Resource.
     */
    string getName();

protected:
    /**
     * Creates a new GraphProducer.
     */
    GraphProducer();

    /**
     * Initializes the fields of a GraphProducer.
     *
     * See #GraphProducer.
     */
    void init(string name, ptr<TileCache> cache, ptr<GraphCache> precomputedGraphs,
        set<int> precomputedLevels, bool doFlatten = false, float flatnessFactor = 0.5f,
        bool storeParents = false, int maxNodes = 0);

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void stopCreateTile(int level, int tx, int ty);

    virtual void swap(ptr<GraphProducer> p);

private:
    /**
     * Updates the cache of flatten curves and their associated CurveDatas.
     * When a Curve is changed, it's data will need to be recomputed.
     * To simply do that, it is just removed from the cache and will be
     * rebuilt when required.
     *
     * @param changes list of curves that changed during last update and
     *      for which the CurveData is outdated.
     */
    void updateFlattenCurve(const set<CurveId> &changedCurves);

    /**
     * The name of this resource.
     */
    string name;

    /**
     * Root Tile size.
     */
    int tileSize;

    /**
     * Levels at which we can directly load the graphs from the disk.
     * For each tile in those levels, the corresponding graph must be
     * precomputed and stored in a directory with the name of the graph.
     * If not existing, it will be computed from the root tile and stored
     * in cache. level 1 -> l-1 will be computed from the root tile
     * if required afterwards.
     */
    set<int> precomputedLevels;

    /**
     * Maximum amount of nodes allowed in a Graph. If under that
     * number, the graph is considered small enough to not have to be
     * clipped, and will be used for all its children.
     * If set to 0, the Graph will be clipped as usual.
     */
    int maxNodes;

    /**
     * List of margins associated to this GraphProducer (added from each
     * layer associated to this GraphProducer).
     */
    ComposedMargin* margins;

    /**
     * If true, the producer will call flatten() on each produced subgraph
     * after clipping.
     */
    bool doFlatten;

    /**
     * Factor for flatten() method (square of the maximum allowed distance
     * between a limit curve and its polyline approximation).
     */
    float flatnessFactor;

    /**
     * If true, will store parent tiles when creating a new one (call to
     * useTile()) so it won't be deleted if temporarily not used.
     */
    bool storeParents;

    /**
     * The cache that manages the precomputed Graphs (including root Graph).
     */
    ptr<GraphCache> precomputedGraphs;

    /**
     * lists of every used flattened Curves, mapped to their Id.
     */
    map<CurveId, CurvePtr> flattenCurves;

    /**
     * Reference counts for each flattened Curve.
     */
    map<CurvePtr, int> flattenCurveCount;
};

}

#endif
