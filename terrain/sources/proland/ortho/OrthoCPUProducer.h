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

#ifndef _PROLAND_ORTHO_CPU_PRODUCER_H_
#define _PROLAND_ORTHO_CPU_PRODUCER_H_

#include <string>

#include "proland/producer/TileProducer.h"

namespace proland
{

/**
 * @defgroup ortho ortho
 * Provides producers and layers for %terrain textures.
 * @ingroup proland
 */

/**
 * A TileProducer to load any kind of texture tile from disk to CPU memory.
 * See \ref sec-orthocpu.
 * @ingroup ortho
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class OrthoCPUProducer : public TileProducer
{
public:
    /**
     * Creates a new OrthoCPUProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a CPUTileStorage of unsigned char type. The size
     *      of tiles in this storage size must be equal to the size of the
     *      tiles stored on disk, borders included.
     * @param name the name of the file containing the tiles to load.
     */
    OrthoCPUProducer(ptr<TileCache> cache, const char *name);

    /**
     * Deletes this OrthoCPUProducer.
     */
    virtual ~OrthoCPUProducer();

    virtual int getBorder();

    virtual bool hasTile(int level, int tx, int ty);

    /**
     * Returns true if the produced tiles are compressed in DXT format.
     */
    bool isCompressed();

protected:
    /**
     * Creates an uninitialized OrthoCPUProducer.
     */
    OrthoCPUProducer();

    /**
     * Initializes this OrthoCPUProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a CPUTileStorage of unsigned char type. The size
     *      of tiles in this storage size must be equal to the size of the
     *      tiles stored on disk, borders included.
     * @param name the name of the file containing the tiles to load.
     */
    virtual void init(ptr<TileCache> cache, const char *name);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void swap(ptr<OrthoCPUProducer> p);

private:
    /**
     * The name of the file containing the residual tiles to load.
     */
    std::string name;

    /**
     * The number of components per pixel in the tiles to load.
     */
    int channels;

    /**
     * The size of the tiles to load, without borders. A tile contains
     * (tileSize+4)*(tileSize+4)*channels samples.
     */
    int tileSize;

    /**
     * The size in pixels of the border around each tile. A tile contains
     * (tileSize+4)*(tileSize+4)*channels samples.
     */
    int border;

    /**
     * The maximum level of the stored tiles on disk (inclusive).
     */
    int maxLevel;

    /**
     * true if the produced tiles are compressed in DXT format.
     */
    bool dxt;

    /**
     * Offset of the first stored tile on disk. The offsets indicated in
     * the tile offsets array #offsets are relative to this offset.
     */
    int header;

    /**
     * The offsets of each tile on disk, relatively to #offset, for each
     * tile id (see #getTileId).
     */
    long long* offsets;

    /**
     * A mutex used to serializes accesses to the file storing the tiles.
     */
    void *mutex;

    /**
     * The file storing the tiles on disk.
     */
    FILE *tileFile;

    /**
     * A key to store thread specific buffers used to produce the tiles.
     */
    static void *key;

    /**
     * Returns the id of the given tile. This id is used to find the offset
     * the tile data on disk, using #offsets.
     *
     * @param level the level of the tile.
     * @param tx the logical x coordinate of the tile.
     * @param ty the logical y coordinate of the tile.
     * @return the id of the given tile.
     */
    int getTileId(int level, int tx, int ty);
};

}

#endif
