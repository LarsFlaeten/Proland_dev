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

#include "proland/terrain/UpdateTileSamplersTask.h"

#include "ork/resource/ResourceTemplate.h"
#include "proland/terrain/TileSampler.h"

using namespace std;

namespace proland
{

UpdateTileSamplersTask::UpdateTileSamplersTask() : AbstractTask("UpdateTileSamplersTask")
{
}

UpdateTileSamplersTask::UpdateTileSamplersTask(const QualifiedName &terrain) :
    AbstractTask("UpdateTileSamplersTask")
{
    init(terrain);
}

void UpdateTileSamplersTask::init(const QualifiedName &terrain)
{
    this->terrain = terrain;
}

UpdateTileSamplersTask::~UpdateTileSamplersTask()
{
}

ptr<Task> UpdateTileSamplersTask::getTask(ptr<Object> context)
{
    ptr<SceneNode> n = context.cast<Method>()->getOwner();
    ptr<SceneNode> target = terrain.getTarget(n);
    ptr<TerrainNode> t = NULL;
    if (target == NULL) {
        t = n->getOwner()->getResourceManager()->loadResource(terrain.name).cast<TerrainNode>();
    } else {
        t = target->getField(terrain.name).cast<TerrainNode>();
    }
    if (t == NULL) {
        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->log("TERRAIN", "UpdateTileSamplers : cannot find terrain '" + terrain.target + "." + terrain.name + "'");
        }
        throw exception();
    }
    ptr<TaskGraph> result = new TaskGraph();
    SceneNode::FieldIterator i = n->getFields();
    while (i.hasNext()) {
        ptr<TileSampler> u = i.next().cast<TileSampler>();
        if (u != NULL) {
            ptr<Task> ut = u->update(n->getOwner(), t->root);
            if (ut.cast<TaskGraph>() == NULL || !ut.cast<TaskGraph>()->isEmpty()) {
                result->addTask(ut);
            }
        }
    }
    return result;
}

void UpdateTileSamplersTask::swap(ptr<UpdateTileSamplersTask> t)
{
    std::swap(*this, *t);
}

class UpdateTileSamplersTaskResource : public ResourceTemplate<40, UpdateTileSamplersTask>
{
public:
    UpdateTileSamplersTaskResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<40, UpdateTileSamplersTask>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,");
        string n = getParameter(desc, e, "name");
        init(QualifiedName(n));
    }
};

extern const char updateTileSamplers[] = "updateTileSamplers";

static ResourceFactory::Type<updateTileSamplers, UpdateTileSamplersTaskResource> UpdateTileSamplersTaskType;

}
