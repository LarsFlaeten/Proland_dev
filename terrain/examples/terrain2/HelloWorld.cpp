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

#include <stdlib.h>

#include "ork/core/FileLogger.h"
#include "ork/render/FrameBuffer.h"
#include "ork/resource/XMLResourceLoader.h"
#include "ork/scenegraph/SceneManager.h"
#include "ork/ui/GlfwWindow.h"
#include "ork/scenegraph/ShowLogTask.h"

#include "proland/TerrainPlugin.h"
#include "proland/terrain/TerrainNode.h"
#include "proland/util/PlanetViewController.h"

using namespace ork;
using namespace proland;

class HelloWorld : public GlfwWindow
{
public:
    ptr<SceneManager> manager;
    ptr<PlanetViewController> controller;
    int mouseX, mouseY;
    bool rotate;
    bool display_extended_info;

    HelloWorld() : GlfwWindow(Window::Parameters().size(1024, 768))
    {
        FileLogger::File *out = new FileLogger::File("log.html");
        Logger::INFO_LOGGER = new FileLogger("INFO", out, Logger::INFO_LOGGER);
        Logger::WARNING_LOGGER = new FileLogger("WARNING", out, Logger::WARNING_LOGGER);
        Logger::ERROR_LOGGER = new FileLogger("ERROR", out, Logger::ERROR_LOGGER);

        ptr<XMLResourceLoader> resLoader = new XMLResourceLoader();
        resLoader->addPath(".");
        resLoader->addPath("../common");
        resLoader->addArchive("helloworld.xml");

        ptr<ResourceManager> resManager = new ResourceManager(resLoader, 8);

        ptr<SceneNode> root = resManager->loadResource("scene").cast<SceneNode>();

        ptr<SceneNode> log = new SceneNode();
        log->addFlag("overlay");
        log->addMethod("draw", new Method(
            resManager->loadResource("logMethod").cast<TaskFactory>()));
        root->addChild(log);
                                             
        ptr<SceneNode> info = new SceneNode();
        info->addFlag("overlay");
        info->addMethod("draw", new Method(
            resManager->loadResource("infoMethod").cast<TaskFactory>()));
        root->addChild(info);

        manager = new SceneManager();
        manager->setResourceManager(resManager);

        manager->setScheduler(resManager->loadResource("defaultScheduler").cast<Scheduler>());
        manager->setRoot(root);
        manager->setCameraNode("camera");
        manager->setCameraMethod("draw");

        controller = new PlanetViewController(manager->getCameraNode(), 6360000.0);

        display_extended_info = true;

    }

    virtual ~HelloWorld()
    {
    }

    virtual void redisplay(double t, double dt)
    {
        controller->setGroundHeight(TerrainNode::groundHeightAtCamera);
        controller->update();
        controller->setProjection();
        
        if(display_extended_info) {
            std::ostringstream oss;
            oss << "Sim time: " << t*1.0e-6;
            ShowInfoTask::setInfo("TIME", oss.str());
                           
            int mx, my;
            this->getMousePosition(&mx, &my);
            oss.str(std::string());
            oss << "Mouse: " << mx << ", " << my;
            ShowInfoTask::setInfo("MOUSE", oss.str());

            oss.str(std::string());
            oss << "Ground height at camera: " << 
                TerrainNode::groundHeightAtCamera;
            ShowInfoTask::setInfo("TERRAINNODE1", oss.str());
        
            oss.str(std::string());
            oss << "Height below camera: " << controller->getHeight()- TerrainNode::groundHeightAtCamera;
            ShowInfoTask::setInfo("TERRAINNODE2", oss.str());
        }

        ptr<FrameBuffer> fb = FrameBuffer::getDefault();
        fb->clear(true, false, true);

        manager->update(t, dt);
        manager->draw();

        GlfwWindow::redisplay(t, dt);

        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->flush();
        }
    }

    virtual void reshape(int x, int y)
    {
        ptr<FrameBuffer> fb = FrameBuffer::getDefault();
        fb->setDepthTest(true, LESS);
		fb->setViewport(vec4<GLint>(0, 0, x, y));
        GlfwWindow::reshape(x, y);
    }

    virtual void idle(bool damaged)
    {
        GlfwWindow::idle(damaged);
        if (damaged) {
            manager->getResourceManager()->updateResources();
        }
    }

    virtual bool mouseClick(button b, state s, modifier m, int x, int y)
    {
        mouseX = x;
        mouseY = y;
        rotate = (m & CTRL) != 0;
        return true;
    }

    virtual bool mouseMotion(int x, int y)
    {
        if (rotate) {
            controller->phi += (mouseX - x) / 500.0;
            controller->theta += (mouseY - y) / 500.0;
        } else {
            vec3d oldp = manager->getWorldCoordinates(mouseX, mouseY);
            vec3d p = manager->getWorldCoordinates(x, y);
            if (valid(oldp) && valid(p)) {
                controller->move(oldp, p);
            }
        }
        mouseX = x;
        mouseY = y;
        return true;
    }

    virtual bool mouseWheel(wheel b, modifier m, int x, int y)
    {
        if (b == WHEEL_DOWN) {
            controller->d *= 1.1;
        }
        if (b == WHEEL_UP) {
            controller->d /= 1.1;
        }
        return true;
    }

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        if (c == 27) {
            ::exit(0);
        }
        return true;
    }

    virtual bool specialKey(key k, modifier m, int x, int y)
    {
        switch (k) {
        case KEY_F5:
            manager->getResourceManager()->updateResources();
            break;
        case KEY_F2:
            display_extended_info = !display_extended_info;
        default:
            break;
        }
        return true;
    }

    bool valid(vec3d p) {
        return p.length() < controller->R * 1.1;
    }

    static static_ptr<Window> app;
};

static_ptr<Window> HelloWorld::app;

int main(int argc, char* argv[])
{
    initTerrainPlugin();
    atexit(Object::exit);
    HelloWorld::app = new HelloWorld();
    HelloWorld::app->start();
    return 0;
}
