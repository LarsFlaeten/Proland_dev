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

#include "proland/particles/ParticleProducer.h"

#include "ork/math/pmath.h"
#include "ork/render/CPUBuffer.h"
#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

ParticleProducer::ParticleProducer() :
    Object("ParticleProducer")
{
}

ParticleProducer::ParticleProducer(const char* type, ptr<ParticleStorage> storage) :
    Object(type)
{
    init(storage);
}

ParticleProducer::ParticleProducer(const char* type) :
    Object(type)
{
}

void ParticleProducer::init(ptr<ParticleStorage> storage)
{
    this->storage = storage;
    this->paramSize = 0;
    this->params = NULL;
    this->initialized = false;
}

ParticleProducer::~ParticleProducer()
{
}

ptr<ParticleStorage> ParticleProducer::getStorage() const
{
    return storage;
}

int ParticleProducer::getLayerCount() const
{
    return int(layers.size());
}

ptr<ParticleLayer> ParticleProducer::getLayer(int index) const
{
    return layers[index];
}

bool ParticleProducer::hasLayers() const
{
    return layers.size() > 0;
}

void ParticleProducer::addLayer(ptr<ParticleLayer> l)
{
    assert(l->owner == NULL);
    l->owner = this;
    layers.push_back(l);
}

void ParticleProducer::getReferencedProducers(vector< ptr<TileProducer> > &producers) const
{
    for (int i = 0; i < (int) layers.size(); i++) {
        layers[i]->getReferencedProducers(producers);
    }
}

void ParticleProducer::updateParticles(double dt)
{
    if (!initialized) {
        initialize();
    }
    moveParticles(dt);
    removeOldParticles();
    addNewParticles();
}

int ParticleProducer::getParticleSize()
{
    return 0;
}

void ParticleProducer::moveParticles(double dt)
{
    for (unsigned int i = 0; i < layers.size(); ++i) {
        if (layers[i]->isEnabled()) {
            layers[i]->moveParticles(dt);
        }
    }
}

void ParticleProducer::removeOldParticles()
{
    for (unsigned int i = 0; i < layers.size(); ++i) {
        if (layers[i]->isEnabled()) {
            layers[i]->removeOldParticles();
        }
    }
}

void ParticleProducer::addNewParticles()
{
    for (unsigned int i = 0; i < layers.size(); ++i) {
        if (layers[i]->isEnabled()) {
            layers[i]->addNewParticles();
        }
    }
}

ParticleStorage::Particle *ParticleProducer::newParticle()
{
    ParticleStorage::Particle *p = storage->newParticle();
    if (p != NULL) {
        for (unsigned int i = 0; i < layers.size(); ++i) {
            layers[i]->initParticle(p);
        }
    }
    return p;
}

ptr<Texture2D> ParticleProducer::copyToTexture(ptr<Texture2D> t, int paramCount, getParticleParams getParams, bool useFuncRes)
{
    int width = (int) ceil(paramCount / 4.0f);
    int height = getStorage()->getCapacity();
    if (t == NULL || t->getWidth() != width || t->getHeight() != height) {
        t = new Texture2D(width, height, RGBA16F,
            RGBA, FLOAT, Texture::Parameters().wrapS(CLAMP_TO_BORDER).wrapT(CLAMP_TO_BORDER).min(NEAREST).mag(NEAREST), Buffer::Parameters(), CPUBuffer(NULL));
    }
    if (params == NULL || paramSize < 4 * width * height) {
        if (params != NULL) {
            delete[] params;
        }
        params = new float[4 * width * height];
    }
    int maxHeight = 0;
    vector<ParticleStorage::Particle*>::iterator i = storage->getParticles();
    vector<ParticleStorage::Particle*>::iterator end = storage->end();
    int h = 0;
    while (i != end) {
        ParticleStorage::Particle *p = *i++;
        if (useFuncRes) {
            h += getParams(this, p, params + 4 * width * h);
        } else {
            h = storage->getParticleIndex(p);
            getParams(this, p, params + 4 * width * h);
            ++h;
        }
        maxHeight = max(maxHeight, h);
    }

    if (maxHeight > 0) {
        t->setSubImage(0, 0, 0, width, maxHeight, RGBA, FLOAT, Buffer::Parameters(), CPUBuffer(params));
    }
    return t;
}

void ParticleProducer::initialize()
{
    assert(!initialized);
    int totalSize = (getParticleSize() / 8) * 8 + 8 * (getParticleSize() % 8 != 0);
    for (unsigned int i = 0; i < layers.size(); ++i) {
        layers[i]->offset = totalSize;
        totalSize += layers[i]->getParticleSize();
    }

    storage->initCpuStorage(totalSize);
    for (unsigned int i = 0; i < layers.size(); ++i) {
        layers[i]->initialize();
    }
    initialized = true;
}

void ParticleProducer::swap(ptr<ParticleProducer> p)
{
    std::swap(storage, p->storage);
//    std::swap(layers, p->layers);
    std::swap(paramSize, p->paramSize);
    std::swap(params, p->params);
    std::swap(initialized, p->initialized);
}

class ParticleProducerResource : public ResourceTemplate<50, ParticleProducer>
{
public:
    ParticleProducerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, ParticleProducer>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,storage,");

        ptr<ParticleStorage> storage = manager->loadResource(getParameter(desc, e, "storage")).cast<ParticleStorage>();

        const TiXmlNode *n = e->FirstChild();
        while (n != NULL) {
            const TiXmlElement *f = n->ToElement();
            if (f == NULL) {
                n = n->NextSibling();
                continue;
            }

            ptr<ParticleLayer> l = manager->loadResource(desc, f).cast<ParticleLayer>();
            if (l != NULL) {
                addLayer(l);
            } else {
                if (Logger::WARNING_LOGGER != NULL) {
                    log(Logger::WARNING_LOGGER, desc, f, "Unknown scene node element '" + f->ValueStr() + "'");
                }
            }
            n = n->NextSibling();
        }

        init(storage);

    }
};

extern const char particleProducer[] = "particleProducer";

static ResourceFactory::Type<particleProducer, ParticleProducerResource> ParticleProducerType;

}
