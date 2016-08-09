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

#include "proland/plants/DrawPlantsTask.h"

#include "ork/math/mat2.h"
#include "ork/resource/ResourceTemplate.h"
#include "ork/render/FrameBuffer.h"
#include "ork/scenegraph/ShowInfoTask.h"

#include "proland/plants/PlantsProducer.h"

namespace proland
{

DrawPlantsTask::DrawPlantsTask() : AbstractTask("DrawPlantsTask")
{
}

DrawPlantsTask::DrawPlantsTask(const string &terrain, ptr<Plants> plants) :
    AbstractTask("DrawPlantsTask")
{
    init(terrain, plants);
}

void DrawPlantsTask::init(const string &terrain, ptr<Plants> plants)
{
    this->terrain = terrain;
    this->plants = plants;
    this->cameraPosU = plants->renderProg->getUniform3f("cameraRefPos");
    this->clipPlane0U = plants->renderProg->getUniform4f("clip[0]");
    this->clipPlane1U = plants->renderProg->getUniform4f("clip[1]");
    this->clipPlane2U = plants->renderProg->getUniform4f("clip[2]");
    this->clipPlane3U = plants->renderProg->getUniform4f("clip[3]");
    this->localToTangentFrameU = plants->renderProg->getUniformMatrix4f("localToTangentFrame");
    this->tangentFrameToScreenU = plants->renderProg->getUniformMatrix4f("tangentFrameToScreen");
    this->tangentFrameToWorldU = plants->renderProg->getUniformMatrix4f("tangentFrameToWorld");
    this->tangentSpaceToWorldU = plants->renderProg->getUniformMatrix3f("tangentSpaceToWorld");
    this->tangentSunDirU = plants->renderProg->getUniform3f("tangentSunDir");
    this->focalPosU = plants->renderProg->getUniform3f("focalPos");
    this->plantRadiusU = plants->renderProg->getUniform1f("plantRadius");
}

DrawPlantsTask::~DrawPlantsTask()
{
}

ptr<Task> DrawPlantsTask::getTask(ptr<Object> context)
{
    ptr<SceneNode> n = context.cast<Method>()->getOwner();
    if (producers.empty()) {
        SceneNode::FieldIterator fields = n->getFields();
        while (fields.hasNext()) {
            string name;
            ptr<Object> value = fields.next(name);
            if (name.compare(0, terrain.length(), terrain) == 0) {
                ptr<SceneNode> tn = value.cast<SceneNode>();
                ptr<PlantsProducer> p = PlantsProducer::getPlantsProducer(tn, plants);
                producers.push_back(p);
            }
        }
    }

    return new Impl(this, n);
}

void DrawPlantsTask::swap(ptr<DrawPlantsTask> t)
{
    std::swap(terrain, t->terrain);
    std::swap(plants, t->plants);
    std::swap(producers, t->producers);
    std::swap(cameraPosU, t->cameraPosU);
    std::swap(clipPlane0U, t->clipPlane0U);
    std::swap(clipPlane1U, t->clipPlane1U);
    std::swap(clipPlane2U, t->clipPlane2U);
    std::swap(clipPlane3U, t->clipPlane3U);
    std::swap(localToTangentFrameU, t->localToTangentFrameU);
    std::swap(tangentFrameToScreenU, t->tangentFrameToScreenU);
    std::swap(tangentFrameToWorldU, t->tangentFrameToWorldU);
    std::swap(tangentSpaceToWorldU, t->tangentSpaceToWorldU);
    std::swap(tangentSunDirU, t->tangentSunDirU);
    std::swap(focalPosU, t->focalPosU);
    std::swap(plantRadiusU, t->plantRadiusU);
}

void DrawPlantsTask::drawPlants(ptr<SceneNode> context)
{
    if (Logger::DEBUG_LOGGER != NULL) {
        Logger::DEBUG_LOGGER->log("PLANTS", "DrawPlants");
    }

    ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();

    //fb->setMultisample(true);
    //fb->setSampleAlpha(true, true);

    int totalTiles = 0;
    int totalTrees = 0;
    int realTrees = q == NULL ? 0 : q->getResult() / 2;
    if (q == NULL) {
        q = new Query(PRIMITIVES_GENERATED);
    }
    q->begin();
    for (unsigned int i = 0; i < producers.size(); ++i) {
        ptr<PlantsProducer> p = producers[i].cast<PlantsProducer>();
        if (p->count == 0) {
            continue;
        }
        if (cameraPosU != NULL) {
            double d = (1.0 - p->node->getOwner()->getCameraToScreen()[2][2]) / 2.0;
            cameraPosU->set(vec3f(p->localCameraPos.x - p->cameraRefPos.x, p->localCameraPos.y - p->cameraRefPos.y, d));
        }
        if (clipPlane0U != NULL) {
            vec4d clipPlanes[6];
            SceneManager::getFrustumPlanes(p->tangentFrameToScreen, clipPlanes);
            clipPlane0U->set((clipPlanes[0] / clipPlanes[0].xyz().length()).cast<float>());
            clipPlane1U->set((clipPlanes[1] / clipPlanes[1].xyz().length()).cast<float>());
            clipPlane2U->set((clipPlanes[2] / clipPlanes[2].xyz().length()).cast<float>());
            clipPlane3U->set((clipPlanes[3] / clipPlanes[3].xyz().length()).cast<float>());
        }
        localToTangentFrameU->setMatrix(p->localToTangentFrame.cast<float>());
        tangentFrameToScreenU->setMatrix(p->tangentFrameToScreen.cast<float>());
        if (tangentFrameToWorldU != NULL) {
            tangentFrameToWorldU->setMatrix(p->tangentFrameToWorld.cast<float>());
        }
        if (tangentSpaceToWorldU != NULL) {
            tangentSpaceToWorldU->setMatrix(p->tangentSpaceToWorld.cast<float>());
        }
        if (tangentSunDirU != NULL) {
            tangentSunDirU->set(p->tangentSunDir.cast<float>());
        }
        if (focalPosU != NULL) {
            vec2d cgDir = (p->cameraToTangentFrame * vec4d(0.0, 0.0, 1.0, 0.0)).xyz().xy().normalize(1000.0);
            focalPosU->set(vec3f(cgDir.x, cgDir.y, p->tangentCameraPos.z));
        }
        if (plantRadiusU != NULL) {
            plantRadiusU->set(plants->getPoissonRadius() * p->terrain->root->l / (1 << plants->getMaxLevel()));
        }

        totalTiles += p->count;
        totalTrees += p->total;
        fb->multiDraw(plants->renderProg, *(p->getPlantsMesh()), POINTS, p->offsets, p->sizes, p->count);
    }
    q->end();

    //fb->setMultisample(false);

    char buf[256];
    sprintf(buf, "%d trees (%d seeds, %d tiles)", realTrees, totalTrees, totalTiles);
    ork::ShowInfoTask::setInfo(string(), string(buf));
}

DrawPlantsTask::Impl::Impl(ptr<DrawPlantsTask> owner, ptr<SceneNode> context) :
    Task("DrawPlants", true, 0), owner(owner), context(context)
{
}

DrawPlantsTask::Impl::~Impl()
{
}

bool DrawPlantsTask::Impl::run()
{
    owner->drawPlants(context);
    return true;
}

class DrawPlantsTaskResource : public ResourceTemplate<50, DrawPlantsTask>
{
public:
    DrawPlantsTaskResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<50, DrawPlantsTask>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,plants,");
        string n = getParameter(desc, e, "name");
        ptr<Plants> plants = manager->loadResource(getParameter(desc, e, "plants")).cast<Plants>();
        init(n, plants);
    }

    virtual bool prepareUpdate()
    {
        bool changed = false;
        if (Resource::prepareUpdate()) {
            changed = true;
        } else if (dynamic_cast<Resource*>(plants.get())->changed()) {
            changed = true;
        }

        if (changed) {
            oldValue = NULL;
            try {
                oldValue = new DrawPlantsTaskResource(manager, name, newDesc == NULL ? desc : newDesc);
            } catch (...) {
            }
            if (oldValue != NULL) {
                swap(oldValue);
                return true;
            }
            return false;
        }
        return true;
    }
};

extern const char drawPlants[] = "drawPlants";

static ResourceFactory::Type<drawPlants, DrawPlantsTaskResource> DrawPlantsTaskType;

}
