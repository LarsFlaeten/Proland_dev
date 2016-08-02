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

#include "proland/ortho/LineOrthoLayer.h"

#include "ork/resource/ResourceTemplate.h"
#include "ork/scenegraph/SceneManager.h"

#include "proland/producer/ObjectTileStorage.h"

using namespace ork;

namespace proland
{

LineOrthoLayer::LineOrthoLayer() : GraphLayer("LineOrthoLayer")
{
}

LineOrthoLayer::LineOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel) :
    GraphLayer("LineOrthoLayer")
{
    init(graphProducer, layerProgram, displayLevel);
}

void LineOrthoLayer::init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram, int displayLevel)
{
    GraphLayer::init(graphProducer, layerProgram, displayLevel);
    mesh = new Mesh<vec2f, unsigned int>(LINE_STRIP, GPU_STREAM);
    mesh->addAttributeType(0, 2, A32F, false);

    tileOffsetU = layerProgram->getUniform3f("tileOffset");
}

LineOrthoLayer::~LineOrthoLayer()
{
}

bool LineOrthoLayer::doCreateTile(int level, int tx, int ty, TileStorage::Slot *data)
{
    if (Logger::DEBUG_LOGGER != NULL) {
        ostringstream oss;
        oss << "LineOrtho tile " << getProducerId() << " " << level << " " << tx << " " << ty;
        Logger::DEBUG_LOGGER->log("ORTHO", oss.str());
    }
    if (level >= displayLevel) {
        TileCache::Tile* t = graphProducer->findTile(level, tx, ty);
        assert(t != NULL);

        ObjectTileStorage::ObjectSlot* graphData = static_cast<ObjectTileStorage::ObjectSlot*>(t->getData());
        GraphPtr graphForThisTile = graphData->data.cast<Graph>();

        vec3d tileCoords = getTileCoords(level, tx, ty);
        float scale = 2.0f * (1.0f - getTileBorder() * 2.0f / getTileSize()) / tileCoords.z;

        ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
        tileOffsetU->set(vec3f(tileCoords.x + tileCoords.z / 2.0f, tileCoords.y + tileCoords.z / 2.0f, scale));

        ptr<Graph::CurveIterator> curveIterator = graphForThisTile->getCurves();
        while (curveIterator->hasNext()) {
            CurvePtr curve = curveIterator->next();
            int numberOfPoints = curve->getSize();
            assert(numberOfPoints >= 2);
            mesh->clear();
            for (int i = 0; i < numberOfPoints; ++i) {
                vec2f vertexPosition = curve->getXY(i).cast<float>();
                mesh->addVertex(vertexPosition);
            }
            fb->draw(layerProgram, *mesh);
        }
    }
    return true;
}

void LineOrthoLayer::swap(ptr<LineOrthoLayer> p)
{
    GraphLayer::swap(p);
    std::swap(mesh, p->mesh);
    std::swap(tileOffsetU, p->tileOffsetU);
}

class LineOrthoLayerResource : public ResourceTemplate<40, LineOrthoLayer>
{
public:
    LineOrthoLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<40, LineOrthoLayer> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        ptr<GraphProducer>graphProducer;
        int displayLevel = 0;

        checkParameters(desc, e, "name,graph,renderProg,level,");
        string g = getParameter(desc, e, "graph");

        graphProducer = manager->loadResource(g).cast<GraphProducer>();

        if (e->Attribute("level") != NULL) {
            getIntParameter(desc, e, "level", &displayLevel);
        }
        assert(displayLevel >= 0);

        ptr<Program> layerProgram = manager->loadResource(getParameter(desc, e, "renderProg")).cast<Program>();
        init(graphProducer, layerProgram, displayLevel);
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

extern const char lineOrthoLayer[] = "lineOrthoLayer";

static ResourceFactory::Type<lineOrthoLayer, LineOrthoLayerResource> LineOrthoLayerType;

}
