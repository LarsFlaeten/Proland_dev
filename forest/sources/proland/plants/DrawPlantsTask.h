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

#ifndef _PROLAND_DRAW_PLANTS_TASK_H_
#define _PROLAND_DRAW_PLANTS_TASK_H_

#include <vector>

#include "ork/scenegraph/AbstractTask.h"

#include "proland/producer/TileProducer.h"
#include "proland/plants/Plants.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * TODO.
 * @ingroup plants
 * @author Eric Bruneton
 */
PROLAND_API class DrawPlantsTask : public AbstractTask
{
public:
    /**
     * Creates a new DrawPlantsTask.
     *
     * @param terrain used to determine which subNodes are pointing to the Terrain SceneNodes.
     * @param plants the Plants that contains the patterns & models used for our vegetation.
     */
    DrawPlantsTask(const string &terrain, ptr<Plants> plants);

    /**
     * Deletes a DrawPlantsTask.
     */
    virtual ~DrawPlantsTask();

    /**
     * Initializes #terrainInfos and creates the actual task that will draw plants.
     */
    virtual ptr<Task> getTask(ptr<Object> context);

protected:
    /**
     * Plant models and amplification parameters.
     */
    ptr<Plants> plants; // plant models and amplification parameters

    /**
     * Creates a new DrawPlantsTask.
     */
    DrawPlantsTask();

    /**
     * Initializes the field of a DrawPlantsTask.
     *
     * See #DrawPlantsTask().
     */
    void init(const string &terrain, ptr<Plants> plants);

    void swap(ptr<DrawPlantsTask> t);

private:
    /**
     * Name of the terrain to be amplified.
     */
    string terrain; // name of the terrain to be amplified

    vector< ptr<TileProducer> > producers;

    // Uniforms (renderPlantProg)
    ptr<Uniform3f> cameraPosU;
    ptr<Uniform4f> clipPlane0U;
    ptr<Uniform4f> clipPlane1U;
    ptr<Uniform4f> clipPlane2U;
    ptr<Uniform4f> clipPlane3U;
    ptr<UniformMatrix4f> localToTangentFrameU;
    ptr<UniformMatrix4f> tangentFrameToScreenU;
    ptr<UniformMatrix4f> tangentFrameToWorldU;
    ptr<UniformMatrix3f> tangentSpaceToWorldU;
    ptr<Uniform3f> tangentSunDirU;
    ptr<Uniform3f> focalPosU;
    ptr<Uniform1f> plantRadiusU;

    ptr<Query> q;

    void drawPlants(ptr<SceneNode> context);

    class Impl : public Task
    {
    public:
        ptr<DrawPlantsTask> owner;

        ptr<SceneNode> context;

        Impl(ptr<DrawPlantsTask> owner, ptr<SceneNode> context);

        virtual ~Impl();

        virtual bool run();
    };
};

}

#endif
