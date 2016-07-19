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

#include "proland/ui/SceneVisitor.h"

#include "proland/terrain/TileSampler.h"

using namespace std;

namespace proland
{

SceneVisitor::SceneVisitor() : Object("SceneVisitor")
{
}

SceneVisitor::~SceneVisitor()
{
}

ptr<SceneVisitor> SceneVisitor::visitNode(ptr<SceneNode> node)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitNodeValue(ptr<Value> value)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitProducer(ptr<TileProducer> producer)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitLayer(ptr<TileLayer> producer)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitNodeField(string &name, ptr<Object> field)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitNodeMethod(string &name, ptr<Method> method)
{
    return this;
}

ptr<SceneVisitor> SceneVisitor::visitCache(ptr<TileCache> cache)
{
    return this;
}

void SceneVisitor::visitEnd()
{
}

void SceneVisitor::accept(ptr<SceneNode> root)
{
    set< ptr<TileCache> > caches;
    accept(root, caches);
    set< ptr<TileCache> >::iterator i = caches.begin();
    while (i != caches.end()) {
        visitCache(*i);
        i++;
    }
}

void SceneVisitor::accept(ptr<SceneNode> n, set< ptr<TileCache> > &caches)
{
    ptr<SceneVisitor> v = visitNode(n);
    SceneNode::ValueIterator u = n->getValues();
    while (u.hasNext()) {
        ptr<Value> value = u.next();
        v->visitNodeValue(value);
    }
    SceneNode::FieldIterator f = n->getFields();
    while (f.hasNext()) {
        string name;
        ptr<Object> field = f.next(name);
        ptr<SceneVisitor> w = v->visitNodeField(name, field);
        if (w != NULL) {
            ptr<TileSampler> ts = field.cast<TileSampler>();
            if (ts != NULL) {
                w->accept(ts->get(), caches);
            }
            w->visitEnd();
        }
    }
    SceneNode::MethodIterator m = n->getMethods();
    while (m.hasNext()) {
        string name;
        ptr<Method> method = m.next(name);
        v->visitNodeMethod(name, method);
    }
    for (unsigned int i = 0; i < n->getChildrenCount(); ++i) {
        v->accept(n->getChild(i), caches);
    }
    v->visitEnd();
}

void SceneVisitor::accept(ptr<TileProducer> p, set< ptr<TileCache> > &caches)
{
    caches.insert(p->getCache());
    ptr<SceneVisitor> v = visitProducer(p);
    if (v != NULL) {
        for (int i = 0; i < p->getLayerCount(); ++i) {
            v->accept(p->getLayer(i), caches);
        }
        vector< ptr<TileProducer> > producers;
        p->getReferencedProducers(producers);
        for (unsigned int i = 0; i < producers.size(); ++i) {
            v->accept(producers[i], caches);
        }
        v->visitEnd();
    }
}

void SceneVisitor::accept(ptr<TileLayer> l, set< ptr<TileCache> > &caches)
{
    ptr<SceneVisitor> v = visitLayer(l);
    if (v != NULL) {
        vector< ptr<TileProducer> > producers;
        l->getReferencedProducers(producers);
        for (unsigned int i = 0; i < producers.size(); ++i) {
            v->accept(producers[i], caches);
        }
        v->visitEnd();
    }
}

}
