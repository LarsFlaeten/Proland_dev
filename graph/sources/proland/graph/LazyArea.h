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

#ifndef _PROLAND_LAZYAREA_H_
#define _PROLAND_LAZYAREA_H_

#include "proland/graph/Area.h"
#include "proland/graph/LazyGraph.h"

namespace proland
{

/**
 * An Area is described by 1 or more curves. It may contain a subgraph.
 * This is used to describe pastures, lakes...
 * This is the 'Lazy' version of Areas. Allows real time loading for LazyGraphs.
 * Can be deleted and reloaded at any time depending on the needs.
 * @ingroup graph
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class LazyArea : public Area
{
public:
    /**
     * Creates a new LazyArea.
     *
     * @param owner the graph containing  this area.
     * @param id this area's id (determined by LazyGraph).
     */
    LazyArea(Graph* owner, AreaId id);

    /**
     * Deletes this LazyArea.
     */
    virtual ~LazyArea();

    /**
     * Returns this area's Id.
     * For LazyArea, an AreaId is a unique number assigned by the owner Graph,
     * (in opposition to Areas, for which Ids are a direct reference to the Area).
     */
    virtual AreaId getId() const;

    /**
     * Returns the parent Area of this Area.
     * Should allways return NULL, because LazyGraph are only used on top of
     * the graph, and thus have no parent.
     */
    virtual AreaPtr getParent() const;

    /**
     * Returns a curve.
     *
     * @param i the rank of the curve to be returned.
     * @return the i'th curve describing this area, or NULL if i is out of range.
     */
    virtual CurvePtr getCurve(int i) const;

    /**
     * Returns a curve.
     *
     * @param i the rank of the curve to be returned.
     * @param orientation will contain the orientation of the i'th curve in this area.
     * @return the i'th curve describing this area, or NULL if i is out of range.
     */
    virtual CurvePtr getCurve(int i, int& orientation) const;

    /**
     * Returns the number of curves forming this area.
     */
    virtual int getCurveCount() const;

    /**
     * Sets orientation for a given curve.
     *
     * @param i rank of the curve to modify.
     * @param orientation the new orientation of the curve.
     */
    virtual void setOrientation(int i, int orientation);

    /**
     * Inverts a given curve in this area.
     *
     * @param cid the curveId of the curve to invert.
     */
    virtual void invertCurve(CurveId cid);

    /**
     * Adds a curve to this area.
     *
     * @param id the added curve's Id.
     * @param orientation the orientation to add the curve.
     */
    virtual void addCurve(CurveId id, int orientation);

protected:
    /**
     * Calls the LazyGraph#releaseArea() method.
     * See Object#doRelease().
     */
    virtual void doRelease();

private:
    /**
     * The parent Area's id. If parentId == id, there's no parent.
     */
    AreaId parentId;

    /**
     * The list of the curves describing this area
     */
    vector< pair<CurveId, int> > curveIds;

    /**
     * Sets the parent Id.
     * See #setParent().
     *
     * @param id the new parent's Id.
     */
    virtual void setParentId(AreaId id);

    /**
     * Switch 2 curves of this area.
     *
     * @param id1 the first curve's id.
     * @param id2 the second curve's id.
     */
    virtual void switchCurves(int curve1, int curve2);

    /**
     * Removes a curve from the curves list.
     *
     * @param the index of the curve to remove.
     */
    virtual void removeCurve(int index);

    /**
     * Same as #addCurve(), but doesn't notify the Owner Graph about a change.
     */
    void loadCurve(CurveId id, int orientation);

    friend class LazyGraph;
};

}

#endif
