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

#include "ork/resource/stbi/stb_image.h"

#include "ork/core/FileLogger.h"
#include "ork/render/FrameBuffer.h"
#include "ork/resource/ResourceTemplate.h"
#include "ork/resource/XMLResourceLoader.h"
#include "ork/scenegraph/AbstractTask.h"
#include "ork/scenegraph/SceneManager.h"
#include "ork/scenegraph/ShowLogTask.h"
#include "ork/ui/GlfwWindow.h"

#include "proland/preprocess/terrain/Preprocess.h"
#include "proland/ui/BasicViewHandler.h"
#include "proland/util/PlanetViewController.h"

using namespace ork;
using namespace proland;

class MyMap : public InputMap
{
public:
    unsigned char *bitmap;

    int w, h, ch;

    float scale;

    MyMap(const string &file, float scale, int width, int height, int channels) : InputMap(width, height, channels, width/4), scale(scale)
    {
		ifstream fs(file.c_str(), ios::binary);
        if(!fs.is_open())
            throw runtime_error("File not found");
		fs.seekg(0, ios::end);
		unsigned int size = fs.tellg();
		unsigned char *data = new unsigned char[size];
		fs.seekg(0);
		fs.read((char*) data, size);
		fs.close();
        bitmap = (unsigned char*) stbi_load_from_memory(data, size, &w, &h, &ch, 0);
        assert( w == width && h == height && ch == channels);
        if(bitmap == 0)
            throw runtime_error(stbi_failure_reason());
        delete[] data;
    }

    vec4f getValue(int x, int y)
    {
        vec4f c;
        if(channels == 3)
        {
            c.x = bitmap[(x + y * width) * 3] * scale;
            c.y = bitmap[(x + y * width) * 3 + 1] * scale;
            c.z = bitmap[(x + y * width) * 3 + 2] * scale;
            c.w = 0.0;
            return c;
        }
        else if (channels == 1)
        {
            c.x = bitmap[(x + y * width)] * scale;
            c.y = c.x;
            c.z = c.x;
            c.w = 0.0;
            return c;                                                      
        } else
            throw runtime_error("Unsupported no of channels");
    }
};

class HelloWorld : public GlfwWindow, public ViewManager
{
public:
    ptr<SceneManager> scene;
    ptr<TerrainViewController> controller;
    ptr<BasicViewHandler> view;
    ptr<EventHandler> ui;

    HelloWorld() : GlfwWindow(Window::Parameters().size(1024, 768))
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

        std::ostringstream oss;
        oss << "Sim time: " << t*1.0e-6;
        ShowInfoTask::setInfo("TIME", oss.str());

        oss.str(std::string());
        oss << "Camera height above ground: " << controller->getHeight();
        ShowInfoTask::setInfo("TERRAINNODE1", oss.str());

        oss.str(std::string());
        oss << "Ground height below camera: " << controller->getGroundHeight();
        ShowInfoTask::setInfo("TERRAINNODE2", oss.str());

        ui->redisplay(t, dt);
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
        ui->reshape(x, y);
        GlfwWindow::reshape(x, y);
        idle(false);
    }

    virtual void idle(bool damaged)
    {
        GlfwWindow::idle(damaged);
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

    ptr<ResourceManager> resManager = new ResourceManager(resLoader, 8);

    app = resManager->loadResource("window").cast<Window>();
}

int main(int argc, char* argv[])
{
	// Scale for moon. From the LOLA data sets:
    // http://imbrium.mit.edu/DATA/SLDEM2015/GLOBAL/FLOAT_IMG/                  SLDEM2015_256_60S_60N_000_360_FLOAT.LBL
    //   MAXIMUM               = 10.781 [km]
    //   MINIMUM               = -8.717 [km]
    // These DEMs use a reference radius of 1737.4 km, and the DEM is +/-
    // We change the radius to r = 1728.683 km, with a scale of 19.498 km
    // for the heights. Max radius is then 1748.181km
    //

    
    preprocessSphericalDem(new MyMap("Moon_1024.png", 19498.0 / 255.0, 1024, 512, 1), 24, 192, 2, "data/dem", "tmpDem", 1.0);
    preprocessSphericalOrtho(new MyMap("Moon_1024.png", 1.0, 1024, 512, 1), 192, 4, 2, "data/rgb", "tmpOrtho");
    atexit(Object::exit);
    init();
    app->start();
    return 0;
}
