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

#include "proland/ortho/ForestOrthoLayer.h"

#include "ork/resource/ResourceTemplate.h"
#include "ork/scenegraph/SceneManager.h"

#include "proland/graph/Area.h"
#include "proland/producer/ObjectTileStorage.h"
#include "proland/ortho/OrthoMargin.h"

using namespace ork;

namespace proland
{

ForestOrthoLayer::ForestOrthoLayer() :
    GraphLayer("ForestOrthoLayer")
{
}

ForestOrthoLayer::ForestOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel, bool quality, vec4f color) :
    GraphLayer("ForestOrthoLayer")
{
    init(graphProducer, layerProgram, displayLevel, quality, color);
}

void ForestOrthoLayer::init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel, bool quality, vec4f color)
{
    GraphLayer::init(graphProducer, layerProgram, displayLevel, quality, false);
    this->color = color;
    this->mesh = new Mesh<vec2f, unsigned int>(TRIANGLE_STRIP, GPU_STREAM);
    this->mesh->addAttributeType(0, 2, A32F, false);
    this->tess = new Tesselator();

    offsetU = layerProgram->getUniform3f("tileOffset");
    colorU = layerProgram->getUniform4f("color");
}

ForestOrthoLayer::~ForestOrthoLayer()
{
}

void ForestOrthoLayer::setTileSize(int tileSize, int tileBorder, float rootQuadSize)
{
    GraphLayer::setTileSize(tileSize, tileBorder, rootQuadSize);
    float borderFactor = tileSize / (tileSize - 1.0f - 2.0f * tileBorder) - 1.0f;
    graphProducer->addMargin(new OrthoMargin(tileSize - 2 * tileBorder, borderFactor, 1.0f));
}

bool ForestOrthoLayer::doCreateTile(int level, int tx, int ty, TileStorage::Slot *data)
{
    if (Logger::DEBUG_LOGGER != NULL) {
        ostringstream oss;
        oss << "OrthoForest tile " << getProducerId() << " " << level << " " << tx << " " << ty;
        Logger::DEBUG_LOGGER->log("ORTHO", oss.str());
    }
    if (level >= displayLevel) {
        ptr<FrameBuffer> fb  = SceneManager::getCurrentFrameBuffer();

        TileCache::Tile * t = graphProducer->findTile(level, tx, ty);
        assert(t != NULL);
        ObjectTileStorage::ObjectSlot *graphData = dynamic_cast<ObjectTileStorage::ObjectSlot*>(t->getData());
        GraphPtr g = graphData->data.cast<Graph>();
        if (g != NULL) {

            vec3d q = getTileCoords(level, tx, ty);
            float scale = 2.0f * (1.0f - getTileBorder() * 2.0f / getTileSize()) / q.z;
            vec3d tileOffset = vec3d(q.x + q.z / 2.0f, q.y + q.z / 2.0f, scale);
            //offsetU->set(vec3f(q.x + q.z / 2.0f, q.y + q.z / 2.0f, scale));
            offsetU->set(vec3f(0.0, 0.0, 1.0));
            colorU->set(vec4f(color.x, color.y, color.z, color.w));

            mesh->setMode(TRIANGLES);
            mesh->clear();
            ptr<Graph::AreaIterator> ai = g->getAreas();
            while (ai->hasNext()) {
                AreaPtr a = ai->next();
                tess->beginPolygon(mesh);
                drawArea(tileOffset, a, *tess);
                tess->endPolygon();
            }
            fb->draw(layerProgram, *mesh);
        } else {
            if (Logger::DEBUG_LOGGER != NULL) {
                ostringstream oss;
                oss << "NULL Graph : " << level << " " << tx << " " << ty;
                Logger::DEBUG_LOGGER->log("GRAPH", oss.str());
            }
        }
    }
    return true;
}

void ForestOrthoLayer::swap(ptr<ForestOrthoLayer> p)
{
    GraphLayer::swap(p);
    std::swap(color, p->color);
    std::swap(mesh, p->mesh);
    std::swap(tess, p->tess);
    std::swap(offsetU, p->offsetU);
    std::swap(colorU, p->colorU);
}

class ForestOrthoLayerResource : public ResourceTemplate<40, ForestOrthoLayer>
{
public:
    ForestOrthoLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<40, ForestOrthoLayer> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        ptr<GraphProducer>graphProducer;
        int displayLevel = 0;
        vec4f color = vec4f((float)30/255,(float)62/255,(float)45/255, 1.0f);

        checkParameters(desc, e, "name,graph,renderProg,level,color,quality,");
        string g = getParameter(desc, e, "graph");

        graphProducer = manager->loadResource(g).cast<GraphProducer>();
        if (e->Attribute("level") != NULL) {
            getIntParameter(desc, e, "level", &displayLevel);
        }
        if (e->Attribute("quality") != NULL) {
            quality = strcmp(e->Attribute("quality"), "true") == 0;
        }

        if (e->Attribute("color") != NULL) {
            string c = getParameter(desc, e, "color") + ",";
            string::size_type start = 0;
            string::size_type index;
            for (int i = 0; i < 3; i++) {
                index = c.find(',', start);
                color[i] = (float) atof(c.substr(start, index - start).c_str()) / 255;
                start = index + 1;
            }
        }

        ptr<Program> layerProgram = manager->loadResource(getParameter(desc, e, "renderProg")).cast<Program>();
        init(graphProducer, layerProgram, displayLevel, quality, color);
    }

    virtual bool prepareUpdate()
    {
        bool changed = false;
        if (dynamic_cast<Resource*>(layerProgram.get())->changed()) {
            changed = true;
        }

        if (changed) {
            invalidateTiles();
        }
        oldValue = NULL;
        newDesc = NULL;

        return true;
    }
};

extern const char forestOrthoLayer[] = "forestOrthoLayer";

static ResourceFactory::Type<forestOrthoLayer, ForestOrthoLayerResource> ForestOrthoLayerType;

}
