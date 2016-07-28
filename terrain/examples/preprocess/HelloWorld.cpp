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

#include "stbi/stb_image.h"

#include "ork/core/FileLogger.h"
#include "ork/render/FrameBuffer.h"
#include "ork/resource/ResourceTemplate.h"
#include "ork/resource/XMLResourceLoader.h"
#include "ork/scenegraph/AbstractTask.h"
#include "ork/scenegraph/SceneManager.h"
#include "ork/scenegraph/ShowLogTask.h"
#include "ork/ui/GlutWindow.h"

#include "proland/preprocess/terrain/Preprocess.h"
#include "proland/ui/BasicViewHandler.h"
#include "proland/util/PlanetViewController.h"

using namespace ork;
using namespace proland;

class MyMap : public InputMap
{
public:
    unsigned char *bitmap;

    float scale;

    MyMap(const string &file, float scale) : InputMap(1350, 675, 4, 225), scale(scale)
    {
		ifstream fs(file.c_str(), ios::binary);
		fs.seekg(0, ios::end);
		unsigned int size = fs.tellg();
		unsigned char *data = new unsigned char[size];
		fs.seekg(0);
		fs.read((char*) data, size);
		fs.close();
        int w, h, channels;
        bitmap = (unsigned char*) stbi_load_from_memory(data, size, &w, &h, &channels, 0);
        delete[] data;
    }

    vec4f getValue(int x, int y)
    {
        vec4f c;
        c.x = bitmap[(x + y * width) * 3] * scale;
        c.y = bitmap[(x + y * width) * 3 + 1] * scale;
        c.z = bitmap[(x + y * width) * 3 + 2] * scale;
        c.w = 0.0;
        return c;
    }
};

class HelloWorld : public GlutWindow, public ViewManager
{
public:
    ptr<SceneManager> scene;
    ptr<TerrainViewController> controller;
    ptr<BasicViewHandler> view;
    ptr<EventHandler> ui;

    HelloWorld() : GlutWindow(Window::Parameters().size(1024, 768))
    {
    }

    virtual ~HelloWorld()
    {
    }

    virtual void redisplay(double t, double dt)
    {
        if (getViewController()->getNode() != scene->getCameraNode()) {
            updateResources();
        }

        ui->redisplay(t, dt);
        GlutWindow::redisplay(t, dt);

        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->flush();
        }
    }

    virtual void reshape(int x, int y)
    {
        ptr<FrameBuffer> fb = FrameBuffer::getDefault();
        fb->setDepthTest(true, LESS);
		fb->setViewport(vec4<GLint>(0, 0, x, y));
        ui->reshape(x, y);
        GlutWindow::reshape(x, y);
        idle(false);
    }

    virtual void idle(bool damaged)
    {
        GlutWindow::idle(damaged);
        if (damaged) {
            updateResources();
        }
        ui->idle(damaged);
    }

    virtual bool mouseClick(button b, state s, modifier m, int x, int y)
    {
        return ui->mouseClick(b, s, m, x, y);
    }

    virtual bool mouseMotion(int x, int y)
    {
        return ui->mouseMotion(x, y);
    }

    virtual bool mousePassiveMotion(int x, int y)
    {
        return ui->mousePassiveMotion(x, y);
    }

    virtual bool mouseWheel(wheel b, modifier m, int x, int y)
    {
        return ui->mouseWheel(b, m, x, y);
    }

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        if (ui->keyTyped(c, m, x, y)) {
            return true;
        }
        if (c == 27) {
            ::exit(0);
        }
        return false;
    }

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y)
    {
        return ui->keyReleased(c, m, x, y);
    }

    virtual bool specialKey(key k, modifier m, int x, int y)
    {
        if (ui->specialKey(k, m, x, y)) {
            return true;
        }

        switch (k) {
        case KEY_F1:
            ShowLogTask::enabled = !ShowLogTask::enabled;
            return true;
        case KEY_F5:
            updateResources();
            return true;
        default:
            break;
        }
        return false;
    }

    virtual bool specialKeyReleased(key k, modifier m, int x, int y)
    {
        return ui->specialKeyReleased(k, m, x, y);
    }

    virtual ptr<SceneManager> getScene()
    {
        return scene;
    }

    virtual ptr<TerrainViewController> getViewController()
    {
        return controller;
    }

    virtual vec3d getWorldCoordinates(int x, int y)
    {
        vec3d p = scene->getWorldCoordinates(x, y);
        if (controller.cast<PlanetViewController>() != NULL) {
            if (p.length() > controller.cast<PlanetViewController>()->R * 1.1) {
                p = vec3d(NAN, NAN, NAN);
            }
        } else if (abs(p.x) > 100000.0 || abs(p.y) > 100000.0 || abs(p.z) > 100000.0) {
            p = vec3d(NAN, NAN, NAN);
        }
        return p;
    }

    void updateResources()
    {
        BasicViewHandler::Position p;
        view->getPosition(p);
        scene->getResourceManager()->updateResources();
        getViewController()->setNode(scene->getCameraNode());
        view->setPosition(p);
    }

protected:
    void swap(ptr<HelloWorld> o)
    {
        std::swap(scene, o->scene);
        std::swap(ui, o->ui);
        std::swap(view, o->view);
        std::swap(controller, o->controller);
    }
};

class HelloWorldResource : public ResourceTemplate<100, HelloWorld>
{
public:
    HelloWorldResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<100, HelloWorld> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,ui,view,radius,");

        scene = new SceneManager();
        scene->setResourceManager(manager);
        scene->setScheduler(manager->loadResource("defaultScheduler").cast<Scheduler>());
        scene->setRoot(manager->loadResource("scene").cast<SceneNode>());
        scene->setCameraNode("camera");
        scene->setCameraMethod("draw");
        ui = manager->loadResource(getParameter(desc, e, "ui")).cast<EventHandler>();
        view = manager->loadResource(getParameter(desc, e, "view")).cast<BasicViewHandler>();
        if (e->Attribute("radius") != NULL) {
            float radius;
            getFloatParameter(desc, e, "radius", &radius);
            controller = new PlanetViewController(scene->getCameraNode(), radius);
        } else {
            controller = new TerrainViewController(scene->getCameraNode(), 50000.0);
        }
    }

    virtual void doRelease()
    {
        if (manager != NULL) {
            manager->close();
        }
        delete this;
    }
};

extern const char helloworld[] = "helloworld";

static ResourceFactory::Type<helloworld, HelloWorldResource> HelloWorldType;

static_ptr<Window> app;

void init()
{
    FileLogger::File *out = new FileLogger::File("log.html");
    Logger::INFO_LOGGER = new FileLogger("INFO", out, Logger::INFO_LOGGER);
    Logger::WARNING_LOGGER = new FileLogger("WARNING", out, Logger::WARNING_LOGGER);
    Logger::ERROR_LOGGER = new FileLogger("ERROR", out, Logger::ERROR_LOGGER);

    ptr<XMLResourceLoader> resLoader = new XMLResourceLoader();
    resLoader->addPath(".");
    resLoader->addArchive("helloworld.xml");
    resLoader->addPath("e:/data");

    ptr<ResourceManager> resManager = new ResourceManager(resLoader, 8);

    app = resManager->loadResource("window").cast<Window>();
}

int main(int argc, char* argv[])
{
    preprocessSphericalDem(new MyMap("srtm.png", 10000.0 / 255.0), 24, 192, 2, "data/dem", "tmpDem", 1.0);
    //preprocessSphericalAperture("data/dem", 3, 5, 3, "data/dem/aperture", "tmpAperture");
    preprocessSphericalOrtho(new MyMap("bluemarble.png", 1.0), 192, 4, 2, "data/rgb", "tmpOrtho");
    atexit(Object::exit);
    init();
    app->start();
    return 0;
}
