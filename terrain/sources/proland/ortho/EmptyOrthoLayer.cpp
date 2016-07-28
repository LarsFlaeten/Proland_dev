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

#include "proland/ortho/EmptyOrthoLayer.h"

#include "ork/resource/ResourceTemplate.h"
#include "ork/render/FrameBuffer.h"
#include "ork/scenegraph/SceneManager.h"

using namespace std;
using namespace ork;

namespace proland
{

EmptyOrthoLayer::EmptyOrthoLayer() :
    TileLayer("EmptyOrthoLayer")
{
}

EmptyOrthoLayer::EmptyOrthoLayer(vec4f color) :
    TileLayer("EmptyOrthoLayer")
{
    TileLayer::init(false);
    init(color);
}

void EmptyOrthoLayer::init(vec4f color)
{
    this->color = color;
}

EmptyOrthoLayer::~EmptyOrthoLayer()
{
}

bool EmptyOrthoLayer::doCreateTile(int level, int tx, int ty, TileStorage::Slot *data)
{
    if (Logger::DEBUG_LOGGER != NULL) {
        ostringstream oss;
        oss << "Empty tile " << getProducerId() << " " << level << " " << tx << " " << ty;
        Logger::DEBUG_LOGGER->log("ORTHO", oss.str());
    }

    ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
    fb->setClearColor(color);
    fb->clear(true, false, false);
    fb->setClearColor(vec4f::ZERO);

    return true;
}

void EmptyOrthoLayer::swap(ptr<EmptyOrthoLayer> p)
{
    TileLayer::swap(p);
    std::swap(color, p->color);
}

class EmptyOrthoLayerResource : public ResourceTemplate<40, EmptyOrthoLayer>
{
public:
    EmptyOrthoLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<40, EmptyOrthoLayer> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        vec4f color = vec4f(1.0f, 1.0f, 1.0f, 1.0f);

        checkParameters(desc, e, "name,color,");

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

        init(color);
    }

    virtual bool prepareUpdate()
    {
        oldValue = NULL;
        newDesc = NULL;

        return true;
    }
};

extern const char emptyOrthoLayer[] = "emptyOrthoLayer";

static ResourceFactory::Type<emptyOrthoLayer, EmptyOrthoLayerResource> EmptyOrthoLayerType;

}
