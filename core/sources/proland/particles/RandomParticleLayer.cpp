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

#include "proland/particles/RandomParticleLayer.h"

#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

RandomParticleLayer::RandomParticleLayer(box3f bounds) :
    ParticleLayer("RandomParticleLayer", sizeof(RandomParticle))
{
    init(bounds);
}

RandomParticleLayer::RandomParticleLayer() :
    ParticleLayer("RandomParticleLayer", sizeof(RandomParticle))
{
}

void RandomParticleLayer::init(box3f bounds)
{
    this->bounds = bounds;
}

RandomParticleLayer::~RandomParticleLayer()
{
}

void RandomParticleLayer::addNewParticles()
{
}

void RandomParticleLayer::initParticle(ParticleStorage::Particle *p)
{
    RandomParticle *r = getRandomParticle(p);
    r->randomPos.x = bounds.xmin + (bounds.xmax - bounds.xmin) * (rand() / float(RAND_MAX));
    r->randomPos.y = bounds.ymin + (bounds.ymax - bounds.ymin) * (rand() / float(RAND_MAX));
    r->randomPos.z = bounds.zmin + (bounds.zmax - bounds.zmin) * (rand() / float(RAND_MAX));
}

void RandomParticleLayer::swap(ptr<RandomParticleLayer> p)
{
    ParticleLayer::swap(p);
    std::swap(bounds, p->bounds);
}

class RandomParticleLayerResource : public ResourceTemplate<50, RandomParticleLayer>
{
public:
    RandomParticleLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, RandomParticleLayer>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,xmin,xmax,ymin,ymax,zmin,zmax,");

        box3f bounds = box3f(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);

        if (e->Attribute("xmin") != NULL) {
            getFloatParameter(desc, e, "xmin", &(bounds.xmin));
        }
        if (e->Attribute("xmax") != NULL) {
            getFloatParameter(desc, e, "xmax", &(bounds.xmax));
        }
        if (e->Attribute("ymin") != NULL) {
            getFloatParameter(desc, e, "ymin", &(bounds.ymin));
        }
        if (e->Attribute("ymax") != NULL) {
            getFloatParameter(desc, e, "ymax", &(bounds.ymax));
        }
        if (e->Attribute("zmin") != NULL) {
            getFloatParameter(desc, e, "zmin", &(bounds.zmin));
        }
        if (e->Attribute("zmax") != NULL) {
            getFloatParameter(desc, e, "zmax", &(bounds.zmax));
        }

        init(bounds);
    }

    virtual bool prepareUpdate()
    {
        oldValue = NULL;
        newDesc = NULL;

        return true;
    }
};

extern const char randomParticleLayer[] = "randomParticleLayer";

static ResourceFactory::Type<randomParticleLayer, RandomParticleLayerResource> RandomParticleLayerType;

}
