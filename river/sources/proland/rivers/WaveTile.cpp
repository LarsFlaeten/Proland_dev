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

#include "proland/rivers/WaveTile.h"

#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

WaveTile::WaveTile() : Object("WaveTile"), tex(NULL), gridSize(0), tileSize(0), timeLoop(0), waveLength(1.0f)
{
}

WaveTile::WaveTile(string &name, ptr<Texture2D> tex, int gridSize, int tileSize, float waveLength, int timeLoop) : Object("WaveTile")
{
    init(name, tex, gridSize, tileSize, waveLength, timeLoop);
}

WaveTile::~WaveTile()
{
}

void WaveTile::timeStep(float dt)
{
    time += dt;
}

void WaveTile::init(string &name, ptr<Texture2D> tex, int gridSize, int tileSize, float waveLength, int timeLoop)
{
    this->tex = tex;
    this->gridSize = gridSize;
    this->tileSize = tileSize;
    this->waveLength = waveLength;
    this->timeLoop = timeLoop;
    this->time = 0.f;
    this->name = name;
    lastProgram = NULL;
}

float WaveTile::getWaveLength() const
{
    return waveLength;
}

void WaveTile::setWaveLength(float length)
{
    waveLength = length;
}

void WaveTile::checkUniforms(ptr<Program> p)
{
    if (lastProgram != p) {
        patternTexU = p->getUniformSampler(name + ".patternTex");
        patternTexSizeU = p->getUniform1f(name + ".patternTexSize");
        lengthU = p->getUniform1f(name + ".length");
        timeU = p->getUniform1f(name + ".time");
        timeLoopU = p->getUniform1f(name + ".timeLoop");

        lastProgram = p;
    }
}

void WaveTile::updateUniform(ptr<Program> p)
{
    checkUniforms(p);

    if (patternTexU != NULL) {
        patternTexU->set(tex);
    }
    if (patternTexSizeU != NULL) {
        patternTexSizeU->set(tileSize);
    }
    if (lengthU != NULL) {
        lengthU->set(waveLength);
    }
    if (timeU != NULL) {
        timeU->set(time);
    }
    if (timeLoopU != NULL) {
        timeLoopU->set(timeLoop);
    }
}

void WaveTile::swap(ptr<WaveTile> t)
{
    std::swap(tex, t->tex);
}

class WaveTileResource : public ResourceTemplate<50, WaveTile>
{
public:
    WaveTileResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, WaveTile>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;

        checkParameters(desc, e, "name,samplerName,texture,gridSize,tileSize,waveLength,timeLoop,");

        ptr<Texture2D> tex = manager->loadResource(getParameter(desc, e, "texture")).cast<Texture2D>();
        int gridSize = 256;
        int tileSize = 1;
        float waveLength = 1.0f;
        int timeLoop = 64;
        string sName = e->Attribute("samplerName");

        if (e->Attribute("gridSize") != NULL) {
            getIntParameter(desc, e, "gridSize", &gridSize);
        }
        if (e->Attribute("tileSize") != NULL) {
            getIntParameter(desc, e, "tileSize", &tileSize);
        }
        if (e->Attribute("waveLength") != NULL) {
            getFloatParameter(desc, e, "waveLength", &waveLength);
        }
        if (e->Attribute("timeLoop") != NULL) {
            getIntParameter(desc, e, "timeLoop", &timeLoop);
        }
        init(sName, tex, gridSize, tileSize, waveLength, timeLoop);
    }
};

extern const char waveTile[] = "waveTile";

static ResourceFactory::Type<waveTile, WaveTileResource> WaveTileType;

}

