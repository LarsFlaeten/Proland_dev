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

#include "proland/terrain/DrawTerrainTask.h"

#include "ork/resource/ResourceTemplate.h"
#include "ork/render/FrameBuffer.h"

using namespace std;
using namespace ork;

namespace proland
{

DrawTerrainTask::DrawTerrainTask() : AbstractTask("DrawTerrainTask")
{
}

DrawTerrainTask::DrawTerrainTask(const QualifiedName &terrain, const QualifiedName &mesh, bool culling) :
    AbstractTask("DrawTerrainTask")
{
    init(terrain, mesh, culling);
}

void DrawTerrainTask::init(const QualifiedName &terrain, const QualifiedName &mesh, bool culling)
{
    this->terrain = terrain;
    this->mesh = mesh;
    this->culling = culling;
}

DrawTerrainTask::~DrawTerrainTask()
{
}

ptr<Task> DrawTerrainTask::getTask(ptr<Object> context)
{
    ptr<SceneNode> n = context.cast<Method>()->getOwner();

    ptr<TerrainNode> t = NULL;
    ptr<SceneNode> target = terrain.getTarget(n);
    if (target == NULL) {
        t = n->getOwner()->getResourceManager()->loadResource(terrain.name).cast<TerrainNode>();
    } else {
        t = target->getField(terrain.name).cast<TerrainNode>();
    }
    if (t == NULL) {
        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->log("TERRAIN", "DrawTerrain : cannot find terrain '" + terrain.target + "." + terrain.name + "'");
        }
        throw exception();
    }

    ptr<MeshBuffers> m = NULL;
    target = mesh.getTarget(n);
    if (target == NULL) {
        m = n->getOwner()->getResourceManager()->loadResource(mesh.name + ".mesh").cast<MeshBuffers>();
    } else {
        m = target->getMesh(mesh.name);
    }
    if (m == NULL) {
        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->log("SCENEGRAPH", "DrawMesh : cannot find mesh '" + mesh.target + "." + mesh.name + "'");
        }
        throw exception();
    }
    return new Impl(n, t, m, culling);
}

void DrawTerrainTask::swap(ptr<DrawTerrainTask> t)
{
    std::swap(*this, *t);
}

DrawTerrainTask::Impl::Impl(ptr<SceneNode> n, ptr<TerrainNode> t, ptr<MeshBuffers> m, bool culling) :
    Task("DrawTerrain", true, 0), n(n), t(t), m(m), culling(culling)
{
}

DrawTerrainTask::Impl::~Impl()
{
}

bool DrawTerrainTask::Impl::run()
{
    if (t != NULL) {
        if (Logger::DEBUG_LOGGER != NULL) {
            Logger::DEBUG_LOGGER->log("TERRAIN", "DrawTerrain");
        }
        ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
        async = false;
        vector< ptr<TileSampler> > uniforms;
        SceneNode::FieldIterator i = n->getFields();
        while (i.hasNext()) {
            ptr<TileSampler> u = i.next().cast<TileSampler>();
            if (u != NULL) {
                if (u->getTerrain(0) != NULL) {
                    u->setTileMap();
                }
                if (u->getStoreLeaf() && u->getTerrain(0) == NULL) {
                    uniforms.push_back(u);
                    if (u->getAsync() && !u->getMipMap()) {
                        async = true;
                    }
                }
            }
        }

        ptr<Program> p = SceneManager::getCurrentProgram();
        t->deform->setUniforms(n, t, p);
        if (async) {
            int k = 0;
            switch (m->mode) {
                case TRIANGLES:
                    k = 6;
                    break;
                case TRIANGLES_ADJACENCY:
                    k = 12;
                    break;
                case LINES_ADJACENCY:
                case PATCHES:
                    k = 4;
                    break;
                default:
                    // unsupported formats
                    assert(false);
            }
            int n = int(sqrt((double)m->nvertices)) - 1;
            gridSize = (n / 2) * (n / 2) * k;
            assert(m->nindices >= gridSize * 32);

            findDrawableQuads(t->root, uniforms);
        }
        drawQuad(t->root, uniforms);
    }
    return true;
}

void DrawTerrainTask::Impl::findDrawableQuads(ptr<TerrainQuad> q, const vector< ptr<TileSampler> > &uniforms)
{
    q->drawable = false;

    if (culling && q->visible == SceneManager::INVISIBLE) {
        q->drawable = true;
        return;
    }

    if (q->isLeaf()) {
        for (unsigned int i = 0; i < uniforms.size(); ++i) {
            if (!uniforms[i]->getAsync() || uniforms[i]->getMipMap()) {
                continue;
            }
            ptr<TileProducer> p = uniforms[i]->get();
            if (p->hasTile(q->level, q->tx, q->ty) && p->findTile(q->level, q->tx, q->ty) == NULL) {
                return;
            }
        }
    } else {
        int nDrawable = 0;
        for (int i = 0; i < 4; ++i) {
            findDrawableQuads(q->children[i], uniforms);
            if (q->children[i]->drawable) {
                ++nDrawable;
            }
        }
        if (nDrawable < 4) {
            for (unsigned int i = 0; i < uniforms.size(); ++i) {
                if (!uniforms[i]->getAsync() || uniforms[i]->getMipMap()) {
                    continue;
                }
                ptr<TileProducer> p = uniforms[i]->get();
                if (p->hasTile(q->level, q->tx, q->ty) && p->findTile(q->level, q->tx, q->ty) == NULL) {
                    return;
                }
            }
        }
    }

    q->drawable = true;
}

void DrawTerrainTask::Impl::drawQuad(ptr<TerrainQuad> q, const vector< ptr<TileSampler> > &uniforms)
{
    if (culling && q->visible == SceneManager::INVISIBLE) {
        return;
    }
    if (async && !q->drawable) {
        return;
    }

    ptr<Program> p = SceneManager::getCurrentProgram();
    if (q->isLeaf()) {
        for (unsigned int i = 0; i < uniforms.size(); ++i) {
            uniforms[i]->setTile(q->level, q->tx, q->ty);
        }
        t->deform->setUniforms(n, q, p);
        if (async) {
            SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, 0, gridSize * 4);
        } else {
            if (m->nindices == 0) {
                SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, 0, m->nvertices);
            } else {
                SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, 0, m->nindices);
            }
        }
    } else {
        int order[4];
        double ox = t->getLocalCamera().x;
        double oy = t->getLocalCamera().y;

        double cx = q->ox + q->l / 2.0;
        double cy = q->oy + q->l / 2.0;
        if (oy < cy) {
            if (ox < cx) {
                order[0] = 0;
                order[1] = 1;
                order[2] = 2;
                order[3] = 3;
            } else {
                order[0] = 1;
                order[1] = 0;
                order[2] = 3;
                order[3] = 2;
            }
        } else {
            if (ox < cx) {
                order[0] = 2;
                order[1] = 0;
                order[2] = 3;
                order[3] = 1;
            } else {
                order[0] = 3;
                order[1] = 1;
                order[2] = 2;
                order[3] = 0;
            }
        }

        int done = 0;
        for (int i = 0; i < 4; ++i) {
            if (culling && q->children[order[i]]->visible == SceneManager::INVISIBLE) {
                done |= (1 << order[i]);
            } else if (!async || q->children[order[i]]->drawable) {
                drawQuad(q->children[order[i]], uniforms);
                done |= (1 << order[i]);
            }
        }
        if (done < 15) {
            int sizes[16] = { 0, 4, 7, 10, 12, 15, 17, 19, 20, 23, 25, 27, 28, 30, 31, 32 };
            for (unsigned int i = 0; i < uniforms.size(); ++i) {
                uniforms[i]->setTile(q->level, q->tx, q->ty);
            }
            t->deform->setUniforms(n, q, p);
            SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, gridSize * sizes[done], gridSize * (sizes[done+1] - sizes[done]));
        }
    }
}

class DrawTerrainTaskResource : public ResourceTemplate<40, DrawTerrainTask>
{
public:
    DrawTerrainTaskResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<40, DrawTerrainTask>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,mesh,culling,");
        string n = getParameter(desc, e, "name");
        string m = getParameter(desc, e, "mesh");
        bool culling = false;
        if (e->Attribute("culling") != NULL && strcmp(e->Attribute("culling"), "true") == 0) {
            culling = true;
        }
        init(QualifiedName(n), QualifiedName(m), culling);
    }
};

extern const char drawTerrain[] = "drawTerrain";

static ResourceFactory::Type<drawTerrain, DrawTerrainTaskResource> DrawTerrainTaskType;

}
