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

#ifndef _PROLAND_LAZYNODE_H_
#define _PROLAND_LAZYNODE_H_

#include "proland/graph/Node.h"

namespace proland
{

/**
 * A Node is described by it's XY coordinates.
 * It is used to represent start and end points of curves, intersections...
 * Several Curves can share the same Node.
 * This is the 'Lazy' version of Nodes. Allows real-time loading for LazyGraphs.
 * Can be deleted and reloaded at any time depending on the needs.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class LazyNode : public Node
{
public:
    /**
     * Creates a new Node.
     *
     * @param owner the graph containing this node.
     * @param id this Node's id (determined by LazyGraph).
     * @param x this node's X coord.
     * @param y this node's Y coord.
     */
    LazyNode(Graph* owner, NodeId id, double x, double y);

    /**
     * Deletes this LazyNode.
     */
    virtual ~LazyNode();

    /**
     * Returns this Node's Id.
     * For LazyNode, a NodeId is a unique number, assigned by the owner Graph,
     * (in opposition to Nodes, for which Ids are a direct reference to the Node).
     */
    virtual NodeId getId() const;

    /**
     * Adds a Curve to the Curves list.
     *
     * @param c the id of the Curve to add.
     */
    virtual void addCurve(CurveId c);

    /**
     * Removes a Curve from the Curves list.
     *
     * @param c the id of the Curve to remove.
     */
    virtual void removeCurve(CurveId c);

protected:
    /**
     * Calls the LazyGraph#releaseNode() method.
     * See Object#doRelease().
     */
    virtual void doRelease();

private:
    /**
     * This node's Id.
     */
    NodeId id;

    /**
     * Adds a Curve to the Curves list, but doesn't change the owner's cache.
     *
     * @param c the id of the Curve to add.
     */
    void loadCurve(CurveId c);

    friend class LazyGraph;
};

}

#endif
