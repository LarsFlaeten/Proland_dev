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

#ifndef _PROLAND_TERRAIN_QUAD_H_
#define _PROLAND_TERRAIN_QUAD_H_

#include "ork/scenegraph/SceneManager.h"

using namespace ork;

namespace proland
{

class TerrainNode;

/**
 * A quad in a %terrain quadtree. The quadtree is subdivided based only
 * on the current viewer position. All quads are subdivided if they
 * meet the subdivision criterion, even if they are outside the view
 * frustum. The quad visibility is stored in #visible. It can be used
 * in TileSampler to decide whether or not data must be produced
 * for invisible tiles (we recall that the %terrain quadtree itself
 * does not store any %terrain data).
 * @ingroup terrain
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class TerrainQuad : public Object
{
public:
    /**
     * The parent quad of this quad.
     */
    const TerrainQuad *parent;

    /**
     * The level of this quad in the quadtree (0 for the root).
     */
    const int level;

    /**
     * The logical x coordinate of this quad (between 0 and 2^level).
     */
    const int tx;

    /**
     * The logical y coordinate of this quad (between 0 and 2^level).
     */
    const int ty;

    /**
     * The physical x coordinate of the lower left corner of this quad
     * (in local space).
     */
    const double ox;

    /**
     * The physical y coordinate of the lower left corner of this quad
     * (in local space).
     */
    const double oy;

    /**
     * The physical size of this quad (in local space).
     */
    const double l;

    /**
     * The minimum %terrain elevation inside this quad. This field must
     * be updated manually by users (the TileSamplerZ class can
     * do this for you).
     */
    float zmin;

    /**
     * The maximum %terrain elevation inside this quad. This field must
     * be updated manually by users (the TileSamplerZ class can
     * do this for you).
     */
    float zmax;

    /**
     * The four subquads of this quad. If this quad is not subdivided,
     * the four values are NULL. The subquads are stored in the
     * following order: bottomleft, bottomright, topleft, topright.
     */
    ptr<TerrainQuad> children[4];

    /**
     * The visibility of the bounding box of this quad from the current
     * viewer position. The bounding box is computed using #zmin and
     * #zmax, which must therefore be up to date to get a correct culling
     * of quads out of the view frustum. This visibility only takes frustum
     * culling into account.
     */
    SceneManager::visibility visible;

    /**
     * True if the bounding box of this quad is occluded by the bounding
     * boxes of the quads in front of it.
     */
    bool occluded;

    /**
     * True if the quad is invisible, or if all its associated tiles are
     * produced and available in cache (this may not be the case if the
     * asynchronous mode is used in a TileSampler).
     */
    bool drawable;

    /**
     * Creates a new TerrainQuad.
     *
     * @param owner the TerrainNode to which the %terrain quadtree belongs.
     * @param parent the parent quad of this quad.
     * @param tx the logical x coordinate of this quad.
     * @param ty the logical y coordinate of this quad.
     * @param ox the physical x coordinate of the lower left corner of this quad.
     * @param oy the physical y coordinate of the lower left corner of this quad.
     * @param l the physical size of this quad.
     * @param zmin the minimum %terrain elevation inside this quad.
     * @param zmax the maximum %terrain elevation inside this quad.
     */
    TerrainQuad(TerrainNode *owner, const TerrainQuad *parent, int tx, int ty, double ox, double oy, double l, float zmin, float zmax);

    /**
     * Deletes this TerrainQuad.
     */
    virtual ~TerrainQuad();

    /**
     * Returns the TerrainNode to which the %terrain quadtree belongs.
     */
    TerrainNode *getOwner();

    /**
     * Returns true if this quad is not subdivided.
     */
    bool isLeaf() const;

    /**
     * Returns the number of quads in the tree below this quad.
     */
    int getSize() const;

    /**
     * Returns the depth of the tree below this quad.
     */
    int getDepth() const;

    /**
     * Subdivides or unsubdivides this quad based on the current
     * viewer distance to this quad, relatively to its size. This
     * method uses the current viewer position provided by the
     * TerrainNode to which this quadtree belongs.
     */
    void update();

private:
    /**
     * The TerrainNode to which this %terrain quadtree belongs.
     */
    TerrainNode *owner;

    /**
     * Creates the four subquads of this quad.
     */
    void subdivide();

    friend class TerrainNode;
};

}

#endif
