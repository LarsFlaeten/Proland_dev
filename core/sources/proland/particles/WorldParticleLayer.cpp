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

#include "proland/particles/WorldParticleLayer.h"

#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

WorldParticleLayer::WorldParticleLayer(float speedFactor) :
    ParticleLayer("WorldParticleLayer", sizeof(WorldParticle))
{
    init(speedFactor);
}

WorldParticleLayer::WorldParticleLayer() :
    ParticleLayer("WorldParticleLayer", sizeof(WorldParticle))
{
}

void WorldParticleLayer::init(float speedFactor)
{
    this->speedFactor = speedFactor;
    this->paused = false;
}

WorldParticleLayer::~WorldParticleLayer()
{
}

float WorldParticleLayer::getSpeedFactor() const
{
    return speedFactor;
}

void WorldParticleLayer::setSpeedFactor(float speedFactor)
{
    this->speedFactor = speedFactor;
}

bool WorldParticleLayer::isPaused() const
{
    return paused;
}

void WorldParticleLayer::setPaused(bool paused)
{
    this->paused = paused;
}

void WorldParticleLayer::moveParticles(double dt)
{
    if (paused) {
        return;
    }
    float DT = dt * speedFactor * 1e-6;
    ptr<ParticleStorage> s = getOwner()->getStorage();
    vector<ParticleStorage::Particle*>::iterator i = s->getParticles();
    vector<ParticleStorage::Particle*>::iterator end = s->end();
    while (i != end) {
        WorldParticle *w = getWorldParticle(*i);
        if (w->worldPos.x != UNINITIALIZED && w->worldPos.y != UNINITIALIZED && w->worldPos.z != UNINITIALIZED && w->worldVelocity.x != UNINITIALIZED && w->worldVelocity.y != UNINITIALIZED && w->worldVelocity.z != UNINITIALIZED) {
            w->worldPos += w->worldVelocity.cast<double>() * DT;
        }
        ++i;
    }
}

void WorldParticleLayer::initParticle(ParticleStorage::Particle *p)
{
    WorldParticle *w = getWorldParticle(p);
    w->worldPos = vec3d(UNINITIALIZED, UNINITIALIZED, UNINITIALIZED);
    w->worldVelocity = vec3f(UNINITIALIZED, UNINITIALIZED, UNINITIALIZED);
}

void WorldParticleLayer::swap(ptr<WorldParticleLayer> p)
{
    ParticleLayer::swap(p);
    std::swap(speedFactor, p->speedFactor);
}

class WorldParticleLayerResource : public ResourceTemplate<50, WorldParticleLayer>
{
public:
    WorldParticleLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, WorldParticleLayer>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,speedFactor,");

        float speedFactor = 1.0f;

        if (e->Attribute("speedFactor") != NULL) {
            getFloatParameter(desc, e, "speedFactor", &speedFactor);
        }

        init(speedFactor);
    }

    virtual bool prepareUpdate()
    {
        oldValue = NULL;
        newDesc = NULL;

        return true;
    }
};

extern const char worldParticleLayer[] = "worldParticleLayer";

static ResourceFactory::Type<worldParticleLayer, WorldParticleLayerResource> WorldParticleLayerType;

}
