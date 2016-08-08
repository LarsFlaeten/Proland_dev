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

#ifndef _PROLAND_EDIT_RESIDUAL_PRODUCER_H_
#define _PROLAND_EDIT_RESIDUAL_PRODUCER_H_

#include "proland/dem/ResidualProducer.h"

namespace proland
{

/**
 * A ResidualProducer whose tiles can be edited at runtime.
 * Intended to be used with an EditElevationProducer.
 * @ingroup edit
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class EditResidualProducer : public ResidualProducer
{
public:
    /**
     * Creates a new EditResidualProducer.
     * See #ResidualProducer.
     */
    EditResidualProducer(ptr<TileCache> cache, const char *name, int deltaLevel = 0, float zscale = 1.0);

    /**
     * Deletes this EditResidualProducer.
     */
    virtual ~EditResidualProducer();

    /**
     * Notifies this producer that a tile has been edited.
     *
     * @param level the level of the edited tile.
     * @param tx the logical x coordinate of the edited tile.
     * @param ty the logical y coordinate of the edited tile.
     * @param deltaElevation the difference between the new and the previous
     *      elevation values of the edited tile. Does not include borders
     */
    void editedTile(int level, int tx, int ty, float *deltaElevation);

    /**
     * Updates the residual tiles produced by this producer to take into
     * account all the edited tiles since the last call to this method.
     */
    void updateResiduals();

    /**
     * Cancels all editing operations performed on this producer.
     */
    void reset();

protected:
    /**
     * Creates an uninitialized EditResidualProducer.
     */
    EditResidualProducer();

    /**
     * Initializes this EditResidualProducer.
     */
    void init();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void swap(ptr<EditResidualProducer> p);

private:
    /**
     * The size of the residual tiles, including borders.
     */
    int tWidth;

    /**
     * The size of the residual tiles, without borders.
     */
    int tSize;

    /**
     * The id of the last elevation delta tile that was used in
     * #getDeltaElevation. Optimization to avoid too many lookups in
     * the #deltaElevations map.
     */
    TileCache::Tile::Id curId;

    /**
     * The last elevation delta tile that was used in #getDeltaElevation.
     * Optimization to avoid too many lookups in the #deltaElevations map.
     */
    float *curDeltaElevation;

    /**
     * The residual tiles that have been modified.
     */
    std::map<TileCache::Tile::Id, float*> modifiedTiles;

    /**
     * The elevation deltas from which to recompute the residual tiles.
     */
    std::map<TileCache::Tile::Id, float*> deltaElevations;

    /**
     * Returns the elevation delta at the given location.
     *
     * @param level a quadtree level.
     * @param w the actual tile size at this level (without borders; can be
     *      different than tSize for level less than minLevel).
     * @param n the numbers of tiles per row or column in the quadtree at
     *      this level (not simply 1 << level if minLevel != 0).
     * @param tx a logical tile x coordinate.
     * @param ty a logical tile y coordinate.
     * @param x a pixel coordinate relatively to the lower left corner of
     *      the (level,tx,ty) tile (without borders). Can be outside the
     *      tile itself (then the value will be looked up in an adjacent tile).
     * @param y a pixel coordinate relatively to the lower left corner of
     *      the (level,tx,ty) tile (without borders). Can be outside the
     *      tile itself (then the value will be looked up in an adjacent tile).
     */
    float getDeltaElevation(int level, int w, int n, int tx, int ty, int x, int y);
};

}

#endif
