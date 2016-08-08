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

#ifndef _PROLAND_EDIT_ORTHO_CPU_PRODUCER_H_
#define _PROLAND_EDIT_ORTHO_CPU_PRODUCER_H_

#include "proland/ortho/OrthoCPUProducer.h"

namespace proland
{

/**
 * An OrthoCPUProducer whose tiles can be edited at runtime.
 * Intended to be used with an EditOrthoProducer.
 * @ingroup edit
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class EditOrthoCPUProducer : public OrthoCPUProducer
{
public:
    /**
     * Creates a new EditOrthoCPUProducer.
     * See #OrthoCPUProducer.
     */
    EditOrthoCPUProducer(ptr<TileCache> cache, const char *name);

    /**
     * Deletes this EditOrthoCPUProducer.
     */
    virtual ~EditOrthoCPUProducer();

    /**
     * Notifies this producer that a tile has been edited.
     *
     * @param level the level of the edited tile.
     * @param tx the logical x coordinate of the edited tile.
     * @param ty the logical y coordinate of the edited tile.
     * @param deltaColor the difference between the new and the previous
     *      color values of the edited tile. Does not include borders.
     */
    void editedTile(int level, int tx, int ty, int *deltaColor);

    /**
     * Updates the residual tiles produced by this producer to take into
     * account all the edited tiles since the last call to this method.
     */
    void updateTiles();

    virtual bool hasTile(int level, int tx, int ty);

    /**
     * Cancels all editing operations performed on this producer.
     */
    void reset();

protected:
    /**
     * Creates an uninitialized EditOrthoCPUProducer.
     */
    EditOrthoCPUProducer();

    /**
     * Initializes this EditOrthoCPUProducer.
     * See #EditOrthoCPUProducer.
     */
    virtual void init(ptr<TileCache> cache, const char *name);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void swap(ptr<EditOrthoCPUProducer> p);

private:
    /**
     * True if there is no file associated with this producer.
     */
    bool empty;

    /**
     * The size of the color residual tiles, including borders.
     */
    int tWidth;

    /**
     * The size of the color residual tiles, without borders.
     */
    int tSize;

    /**
     * The number of channels of the color residual tiles.
     */
    int tChannels;

    /**
     * The id of the last color delta tile that was used in
     * #getDeltaColor. Optimization to avoid too many lookups in
     * the #deltaColors map.
     */
    TileCache::Tile::Id curId;

    /**
     * The last color delta tile that was used in #getDeltaColor.
     * Optimization to avoid too many lookups in the #deltaColors map.
     */
    int *curDeltaColor;

    /**
     * The color residual tiles that have been modified.
     */
    std::map<TileCache::Tile::Id, unsigned char*> modifiedTiles;

    /**
     * The color deltas from which to recompute the color residual tiles.
     */
    std::map<TileCache::Tile::Id, int*> deltaColors;

    /**
     * Returns the color delta at the given location.
     *
     * @param level a quadtree level.
     * @param n the numbers of tiles per row or column in the quadtree at
     *      this level (1 << level).
     * @param tx a logical tile x coordinate.
     * @param ty a logical tile y coordinate.
     * @param x a pixel coordinate relatively to the lower left corner of
     *      the (level,tx,ty) tile (without borders). Can be outside the
     *      tile itself (then the value will be looked up in an adjacent tile).
     * @param y a pixel coordinate relatively to the lower left corner of
     *      the (level,tx,ty) tile (without borders). Can be outside the
     *      tile itself (then the value will be looked up in an adjacent tile).
     */
    int *getDeltaColor(int level, int n, int tx, int ty, int x, int y);
};

}

#endif
