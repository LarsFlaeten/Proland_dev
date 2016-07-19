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

#ifndef _PROLAND_SCENEVISITOR_H_
#define _PROLAND_SCENEVISITOR_H_

#include "ork/render/Uniform.h"
#include "ork/scenegraph/SceneNode.h"
#include "proland/producer/TileProducer.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup proland_ui ui
 * Provides classes to build a user interface to control and %edit a scene.
 * @ingroup proland
 */

/**
 * A visitor to visit a scene graph.
 * @ingroup proland_ui
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class SceneVisitor : public Object
{
public:
    /**
     * Creates a new SceneVisitor.
     */
    SceneVisitor();

    /**
     * Deletes this SceneVisitor.
     */
    virtual ~SceneVisitor();

    /**
     * Visits a SceneNode.
     * The default implementation of this method does nothing.
     *
     * @param node the SceneNode to visit.
     */
    virtual ptr<SceneVisitor> visitNode(ptr<SceneNode> node);

    /**
     * Visits a SceneNode.
     * The default implementation of this method does nothing.
     *
     * @param node the SceneNode to visit.
     */
    virtual ptr<SceneVisitor> visitNodeValue(ptr<Value> value);

    /**
     * Visits a TileProducer.
     * The default implementation of this method does nothing.
     *
     * @param producer the TileProducer to visit.
     */
    virtual ptr<SceneVisitor> visitProducer(ptr<TileProducer> producer);

    /**
     * Visits a TileLayer.
     * The default implementation of this method does nothing.
     *
     * @param layer the TileLayer to visit.
     */
    virtual ptr<SceneVisitor> visitLayer(ptr<TileLayer> layer);

    /**
     * Visits a SceneNode field.
     * The default implementation of this method does nothing.
     *
     * @param name the field's name.
     * @param field the field's value.
     */
    virtual ptr<SceneVisitor> visitNodeField(std::string &name, ptr<Object> field);

    /**
     * Visits a Method.
     * The default implementation of this method does nothing.
     *
     * @param name the Method's name.
     * @param method the Method to visit.
     */
    virtual ptr<SceneVisitor> visitNodeMethod(std::string &name, ptr<Method> method);

    /**
     * Visits a TileCache.
     * The default implementation of this method does nothing.
     *
     * @param cache the TileCache to visit.
     */
    virtual ptr<SceneVisitor> visitCache(ptr<TileCache> cache);

    /**
     * Ends the visits of a SceneNode, a Uniform, a TileProducer or a TileLayer.
     * The default implementation of this method does nothing.
     */
    virtual void visitEnd();

    /**
     * Makes this visitor visit the given scene graph. This method calls the
     * visitXxx methods for each scene node, uniform, scene node field, scene
     * node method, tile producer, tile layer, and tile cache encountered
     * during the recursive exploration of the given scene graph.
     *
     * @param root the scene graph to visit.
     */
    void accept(ptr<SceneNode> root);

private:
    /**
     * Makes this visitor visit the given scene node, and all its subnodes.
     *
     * @param n the SceneNode to visit.
     * @param[in,out] caches the TileCache found during this visit.
     */
    void accept(ptr<SceneNode> n, std::set< ptr<TileCache> > &caches);

    /**
     * Makes this visitor visit the given tile producer, and all its
     * referenced producers, recursively.
     *
     * @param p the TileProducer to visit.
     * @param[in,out] caches the TileCache found during this visit.
     */
    void accept(ptr<TileProducer> p, std::set< ptr<TileCache> > &caches);

    /**
     * Makes this visitor visit the given tile layer, and all its
     * referenced producers, recursively.
     *
     * @param l the TileLayer to visit.
     * @param[in,out] caches the TileCache found during this visit.
     */
    void accept(ptr<TileLayer> l, std::set< ptr<TileCache> > &caches);
};

}

#endif
