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

#ifndef _PROLAND_FLOWTILE_H
#define _PROLAND_FLOWTILE_H

#include "ork/core/Object.h"
#include "ork/math/vec2.h"

using namespace ork;

namespace proland
{

/**
 * Computes the velocity of a flow at a given point. Can be used for any
 * moving elements. In this class, the word Tile will refer to a 2D region
 * on a terrain. It may correspond to a TileProducer Tiles, but can also
 * be completely independent from that. You just need to define the
 * considered region on that terrain.
 * @ingroup terrainl
 * @author Antoine Begault
 */
PROLAND_API class FlowTile : public Object
{
public:
    /**
     * Determines the type of data at a point.
     */
    enum DATATYPE
    {
        UNKNOWN = 0,
        INSIDE = 1,
        LEAVING = 2,
        NEAR = 3,
        OUTSIDE = 4,
        LEAVING_DOMAIN = 5,
        OUTSIDE_DOMAIN = 6,
        ON_SKY = 7
    };

    /**
     * Creates a new FlowTile.
     *
     * @param ox the x coordinate of the lower left corner of this Tile.
     * @param oy the y coordinate of the lower left corner of this Tile.
     * @param size width of this Tile.
     */
    FlowTile(float ox, float oy, float size);

    /**
     * Deletes this FlowTile.
     */
    virtual ~FlowTile();

    /**
     * Returns a defined type for a given position in the tile.
     *
     * @param pos a position inside the tile's viewport.
     */
    virtual int getType(vec2d &pos);

    /**
     * Returns the velocity at a given point, depending on the data contained in this FlowTile.
     *
     * @param pos a XY position inside the viewport of this FlowTile.
     * @param[out] velocity a vec2f containing the 2D velocity at given coordinates.
     * @param[out] type type of data at given coordinates. See #dataType
     */
    virtual void getVelocity(vec2d &pos, vec2d &velocity, int &type) = 0;

    /**
     * Returns the data type at a given point. Simplified version of #getVelocity().
     * @param pos a XY position inside the viewport of this FlowTile.
     * @param[out] type type of data at given coordinates. See #dataType.
     */
    virtual void getDataType(vec2d &pos, int &type);

    /**
     * Returns true if the given point is inside this FlowTile's viewport.
     *
     * @param x x coordinate.
     * @param y y coordinate.
     */
    bool contains(float x, float y);

protected:
    /**
     * The x coordinate of the lower left corner of this Tile.
     */
    float ox;

    /**
     * The y coordinate of the lower left corner of this Tile.
     */
    float oy;

    /**
     * Width of this Tile.
     */
    float size;
};

}

#endif
