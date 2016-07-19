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

#ifndef _PROLAND_TILELAYER_H_
#define _PROLAND_TILELAYER_H_

#include "ork/math/vec2.h"
#include "ork/math/vec3.h"
#include "ork/taskgraph/Scheduler.h"
#include "ork/taskgraph/TaskGraph.h"
#include "proland/producer/TileCache.h"

using namespace ork;

namespace proland
{

/**
 * An abstract layer for a TileProducer. Some tile producers can be
 * customized with layers modifying the default tile production algorithm
 * (for instance to add roads or rivers to an orthographic tile producer).
 * For these kind of producers, each method of this class is called during
 * the corresponding method in the TileProducer. The default implementation
 * of these methods in this class is empty.
 * @ingroup producer
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class TileLayer : public Object
{
public:
    /**
     * Creates a new TileLayer.
     *
     * @param type the layer's type.
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    TileLayer(const char *type, bool deform = false);

    /**
     * Deletes this TileLayer.
     */
    virtual ~TileLayer();

    /**
     * Returns the TileCache that stores the tiles produced by the producer using this TileLayer.
     */
    ptr<TileCache> getCache();

    /**
     * Returns the id of the producer using this TileLayer. This id is local to the TileCache used by
     * this producer, and is used to distinguish all the producers that use this
     * cache.
     */
    int getProducerId();

    /**
     * Returns the tile size, i.e. the size in pixels of each tile of the producer to which this layer
     * belongs. This size includes borders.
     */
    int getTileSize();

    /**
     * Returns the size in pixels of the border of each tile. Tiles made of
     * raster data may have a border that contains the value of the neighboring
     * pixels of the tile. For instance if the tile size (returned by
     * TileStorage#getTileSize) is 196, and if the tile border is 2, this means
     * that the actual tile data is 192x192 pixels, with a 2 pixel border that
     * contains the value of the neighboring pixels. Using a border introduces
     * data redundancy but is usefull to get the value of the neighboring pixels
     * of a tile without needing to load the neighboring tiles.
     */
    int getTileBorder();

    /**
     * Returns the size in meters of the root quad produced by the producer using this Layer.
     */
    float getRootQuadSize();

    /**
     * Returns the ox,oy,l coordinates of the given tile.
     */
    vec3d getTileCoords(int level, int tx, int ty);

    /**
     * Returns true if a spherical deformation is applied on the layer or not.
     */
    bool isDeformed();

    void getDeformParameters(vec3d tileCoords, vec2d &nx, vec2d &ny, vec2d &lx, vec2d &ly);

    /**
     * Returns true if this TileLayer is enabled.
     */
    bool isEnabled();

    /**
     * Enables or disables this TileLayer.
     *
     * @param enabled true to enable this TileLayer, false otherwise.
     */
    void setIsEnabled(bool enabled);

    /**
     * Sets the TileCache that stores the tiles produced by this Layer.
     */
    virtual void setCache(ptr<TileCache> cache, int producerId);

    /**
     * Returns the tile producers used by this TileLayer.
     *
     * @param[out] producers the tile producers used by this TileLayer.
     */
    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const;

    /**
     * Sets the tile size value.
     */
    virtual void setTileSize(int tileSize, int tileBorder, float rootQuadSize);

    /**
     * Notifies this Layer that the given tile of its TileProducer is in use.
     * This happens when this tile was not previously used, and has been
     * requested with TileProducer#getTile (see also TileCache#getTile).
     *
     * @param level the tile's quadtree level.
     * @param tx the tile's quadtree x coordinate.
     * @param ty the tile's quadtree y coordinate.
     * @param deadline the deadline at which the tile data must be ready. 0 means
     *      the current frame.
     */
    virtual void useTile(int level, int tx, int ty, unsigned int deadline);

    /**
     * Notifies this Layer that the given tile of its TileProducer is unused.
     * This happens when the number of users of this tile becomes null, after a
     * call to TileProducer#putTile (see also TileCache#putTile).
     *
     * @param level the tile's quadtree level.
     * @param tx the tile's quadtree x coordinate.
     * @param ty the tile's quadtree y coordinate.
     */
    virtual void unuseTile(int level, int tx, int ty);

    /**
     * Schedules a prefetch task to create the given tile. If the requested tile
     * is currently in use or in cache but unused, this method does nothing.
     * Otherwise it gets an unused tile storage (evicting an unused tile if
     * necessary), and then creates and schedules a task to produce the data of
     * the requested tile.
     *
     * @param level the tile's quadtree level.
     * @param tx the tile's quadtree x coordinate.
     * @param ty the tile's quadtree y coordinate.
     * @return true if this method has been able to schedule a prefetch task
     *      for the given tile.
     */
    virtual void prefetchTile(int level, int tx, int ty);

    /**
     * Starts the creation of a tile.
     * See TileProducer#startCreateTile.
     */
    virtual void startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    /**
     * Sets the execution context for the Task that produces the tile data.
     * See TileProducer#beginCreateTile.
     */
    virtual void beginCreateTile();

    /**
     * Creates the given tile.
     * See TileProducer#doCreateTile.
     */
    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data) = 0;

    /**
     * Restores the execution context for the Task that produces the tile data.
     * See TileProducer#endCreateTile.
     */
    virtual void endCreateTile();

    /**
     * Stops the creation of a tile.
     * See TileProducer#stopCreateTile.
     */
    virtual void stopCreateTile(int level, int tx, int ty);

    /**
     * Invalidates the tiles modified by this layer.
     * This means that the tasks to produce the actual data of these tiles will
     * be automatically reexecuted before the data can be used.
     */
    virtual void invalidateTiles();

protected:
    /**
     * Initializes TileLayer fields.
     *
     * @param deform whether we apply a spherical deformation on the layer or not.
     */
    void init(bool deform = false);

    virtual void swap(ptr<TileLayer> p);

private:
    /**
     * The TileCache of the TileProducer to which this TileLayer belongs.
     */
    ptr<TileCache> cache;

    /**
     * The id of the TileProducer to which this TileLayer belongs in #cache.
     */
    int producerId;

    /**
     * Size in pixels of each tile of the producer to which this layer
     * belongs. This size includes borders.
     */
    int tileSize;

    /**
     * Size in pixels of the border of each tile of the producer to which
     * this layer belongs. See TileProducer#getTileBorder.
     */
    int tileBorder;

    /**
     * Size in meters of the root quad produced by the producer to which
     * this layer belongs.
     */
    float rootQuadSize;

    /**
     * Whether we apply a spherical deformation on the layer or not.
     */
    bool deform;

    bool enabled;
};

}

#endif
