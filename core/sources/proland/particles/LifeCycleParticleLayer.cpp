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

#include "proland/particles/LifeCycleParticleLayer.h"

#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

LifeCycleParticleLayer::LifeCycleParticleLayer(float fadeInDelay, float activeDelay, float fadeOutDelay) :
    ParticleLayer("LifeCycleParticleLayer", sizeof(LifeCycleParticle))
{
    init(fadeInDelay, activeDelay, fadeOutDelay);
}

LifeCycleParticleLayer::LifeCycleParticleLayer() :
    ParticleLayer("LifeCycleParticleLayer", sizeof(LifeCycleParticle))
{
}

void LifeCycleParticleLayer::init(float fadeInDelay, float activeDelay, float fadeOutDelay)
{
    this->fadeInDelay = fadeInDelay;
    this->activeDelay = activeDelay;
    this->fadeOutDelay = fadeOutDelay;
    this->time = 0.0f;
}

LifeCycleParticleLayer::~LifeCycleParticleLayer()
{
}

bool LifeCycleParticleLayer::isFadingIn(ParticleStorage::Particle *p)
{
    float age = time - getBirthDate(p);
    return age < fadeInDelay;
}

bool LifeCycleParticleLayer::isActive(ParticleStorage::Particle *p)
{
    float age = time - getBirthDate(p);
    return age >= fadeInDelay && age < fadeInDelay + activeDelay;
}

bool LifeCycleParticleLayer::isFadingOut(ParticleStorage::Particle *p)
{
    float age = time - getBirthDate(p);
    return age >= fadeInDelay + activeDelay;
}

void LifeCycleParticleLayer::setFadingOut(ParticleStorage::Particle *p)
{
    // making sure that the intensity won't pop to 1.0 when deleting a fading in particle
    float i = getIntensity(p);
    if (!isFadingOut(p)) {
        getLifeCycle(p)->birthDate = time - (fadeInDelay + activeDelay + (1.0f - i) * fadeOutDelay);
    }
}

float LifeCycleParticleLayer::getFadeInDelay() const
{
    return fadeInDelay;
}

float LifeCycleParticleLayer::getActiveDelay() const
{
    return activeDelay;
}

float LifeCycleParticleLayer::getFadeOutDelay() const
{
    return fadeOutDelay;
}

void LifeCycleParticleLayer::setFadeInDelay(float delay)
{
    fadeInDelay = delay;
}

void LifeCycleParticleLayer::setActiveDelay(float delay)
{
    activeDelay = delay;
}

void LifeCycleParticleLayer::setFadeOutDelay(float delay)
{
    fadeOutDelay = delay;
}

void LifeCycleParticleLayer::killParticle(ParticleStorage::Particle *p)
{
    float minBirthDate = time - (fadeInDelay + activeDelay + fadeOutDelay);
    getLifeCycle(p)->birthDate = minBirthDate - 1.0f;
}

float LifeCycleParticleLayer::getIntensity(ParticleStorage::Particle *p)
{
    float t = time - getBirthDate(p);
    if (t < fadeInDelay) {
        return t / fadeInDelay;
    } else {
        t -= fadeInDelay;
        if (t < activeDelay) {
            return 1.0f;
        } else {
            t -= activeDelay;
            return max(0.0f, 1.0f - t / fadeOutDelay);
        }
    }
}

void LifeCycleParticleLayer::moveParticles(double dt)
{
    time += dt;
}

void LifeCycleParticleLayer::removeOldParticles()
{
    // all particles with a birth date less than minBirthDate must be deleted
    float minBirthDate = time - (fadeInDelay + activeDelay + fadeOutDelay);

    ptr<ParticleStorage> s = getOwner()->getStorage();
    vector<ParticleStorage::Particle*>::iterator i = s->getParticles();
    vector<ParticleStorage::Particle*>::iterator end = s->end();
    while (i != end) {
        ParticleStorage::Particle *p = *i;
        if (getBirthDate(p) <= minBirthDate) {
            s->deleteParticle(p);
        }
        ++i;
    }
}

void LifeCycleParticleLayer::initParticle(ParticleStorage::Particle *p)
{
    getLifeCycle(p)->birthDate = time;
}

void LifeCycleParticleLayer::swap(ptr<LifeCycleParticleLayer> p)
{
    ParticleLayer::swap(p);
    std::swap(fadeInDelay, p->fadeInDelay);
    std::swap(fadeOutDelay, p->fadeOutDelay);
    std::swap(activeDelay, p->activeDelay);
    std::swap(time, p->time);
}

class LifeCycleParticleLayerResource : public ResourceTemplate<50, LifeCycleParticleLayer>
{
public:
    LifeCycleParticleLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, LifeCycleParticleLayer>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,fadeInDelay,fadeOutDelay,activeDelay,unit,");

        float fadeInDelay = 5.0f;
        float fadeOutDelay = 5.0f;
        float activeDelay = 30.0f;

        float unit = 1000000.0f; // delays are converted into seconds.

        if (e->Attribute("unit") != NULL) {
            if (strcmp(e->Attribute("unit"), "s") == 0) {
                unit = 1000000.0f;
            } else if (strcmp(e->Attribute("unit"), "ms") == 0) {
                unit = 1000.0f;
            } else if (strcmp(e->Attribute("unit"), "us") == 0) {
                unit = 10.f;
            }
        }

        //delays are taken in seconds
        getFloatParameter(desc, e, "fadeInDelay", &fadeInDelay);
        getFloatParameter(desc, e, "fadeOutDelay", &fadeOutDelay);
        getFloatParameter(desc, e, "activeDelay", &activeDelay);

        init(fadeInDelay * unit, activeDelay * unit, fadeOutDelay * unit);
    }

    virtual bool prepareUpdate()
    {
        oldValue = NULL;
        newDesc = NULL;

        return true;
    }
};

extern const char lifeCycleParticleLayer[] = "lifeCycleParticleLayer";

static ResourceFactory::Type<lifeCycleParticleLayer, LifeCycleParticleLayerResource> LifeCycleParticleLayerType;

}
