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

#include "proland/ui/twbar/TweakViewHandler.h"

#include "ork/resource/ResourceTemplate.h"
#include "ork/render/FrameBuffer.h"

using namespace std;
using namespace ork;

namespace proland
{

TW_CALL void GetViewCallback(void *clientData)
{
    BasicViewHandler::Position q;
    ((BasicViewHandler*) clientData)->getPosition(q);
    if (Logger::INFO_LOGGER != NULL) {
        ostringstream oss;
        oss << "<view name=\"untitled\" key=\"0\" x0=\"";
        oss << q.x0 << "\" y0=\"";
        oss << q.y0 << "\" theta=\"";
        oss << q.theta << "\" phi=\"";
        oss << q.phi << "\" d=\"";
        oss << q.d << "\" sx=\"";
        oss << q.sx << "\" sy=\"";
        oss << q.sy << "\" sz=\"";
        oss << q.sz << "\"/>";
        Logger::INFO_LOGGER->log("UI", oss.str());
    }
}

TW_CALL void SetViewCallback(void *clientData)
{
    TweakViewHandler::Position *p = (TweakViewHandler::Position*) clientData;
    p->go();
}

void TweakViewHandler::Position::go()
{
    if (owner->animate) {
        owner->viewHandler->goToPosition(*this);
    } else {
        owner->viewHandler->jumpToPosition(*this);
    }
}

TweakViewHandler::TweakViewHandler() : TweakBarHandler()
{
}

TweakViewHandler::TweakViewHandler(ptr<BasicViewHandler> viewHandler, const vector<Position> &views, bool animate, bool active)
{
    init(viewHandler, views, animate, active);
}

void TweakViewHandler::init(ptr<BasicViewHandler> viewHandler, const vector<Position> &views, bool animate, bool active)
{
    assert(FrameBuffer::getError() == 0);
    TweakBarHandler::init("View", NULL, active);
    this->viewHandler = viewHandler;
    this->views = views;
    this->animate = animate;
}

TweakViewHandler::~TweakViewHandler()
{
}

void TweakViewHandler::updateBar(TwBar *bar)
{
    for (unsigned int i = 0; i < views.size(); ++i) {
        char def[256] = "group='View'";
        if (views[i].key != 0) {
            sprintf(def, "key='%c' group='View'", views[i].key);
        }
        TwAddButton(bar, views[i].name, SetViewCallback, &(views[i]), def);
    }
    if (!views.empty()) {
        TwAddSeparator(bar, NULL, "group='View'");
        TwAddVarRW(bar, "Animate", TW_TYPE_BOOL8, &animate, "group='View'");
        TwAddButton(bar, "Print", GetViewCallback, viewHandler.get(), "group='View'");
    }
}

void TweakViewHandler::swap(ptr<TweakViewHandler> o)
{
    TweakBarHandler::swap(o);
    std::swap(viewHandler, o->viewHandler);
    std::swap(views, o->views);
    std::swap(animate, o->animate);
}

class TweakViewHandlerResource : public ResourceTemplate<55, TweakViewHandler>
{
public:
    TweakViewHandlerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<55, TweakViewHandler> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,viewHandler,animate,active,");

        ptr<BasicViewHandler> viewHandler = manager->loadResource(getParameter(desc, e, "viewHandler")).cast<BasicViewHandler>();

        bool active = true;
        if (e->Attribute("active") != NULL) {
            active = strcmp(e->Attribute("active"), "true") == 0;
        }

        vector<Position> views;
        const TiXmlNode *n = e->FirstChild();
        while (n != NULL) {
            const TiXmlElement *f = n->ToElement();
            if (f == NULL) {
                n = n->NextSibling();
                continue;
            }
            if (strcmp(f->Value(), "view") == 0) {
                checkParameters(desc, f, "name,key,x0,y0,theta,phi,d,sx,sy,sz,");
                float x0, y0, theta, phi, d, sx, sy, sz;
                getFloatParameter(desc, f, "x0", &x0);
                getFloatParameter(desc, f, "y0", &y0);
                getFloatParameter(desc, f, "theta", &theta);
                getFloatParameter(desc, f, "phi", &phi);
                getFloatParameter(desc, f, "d", &d);
                getFloatParameter(desc, f, "sx", &sx);
                getFloatParameter(desc, f, "sy", &sy);
                getFloatParameter(desc, f, "sz", &sz);
                Position p;
                p.owner = this;
                p.name = f->Attribute("name");
                p.key = f->Attribute("key") == NULL ? 0 : f->Attribute("key")[0];
                p.x0 = x0;
                p.y0 = y0;
                p.theta = theta;
                p.phi = phi;
                p.d = d;
                p.sx = sx;
                p.sy = sy;
                p.sz = sz;
                views.push_back(p);
            }
            n = n->NextSibling();
        }

        bool animate = true;
        if (e->Attribute("animate") != NULL) {
            animate = strcmp(e->Attribute("animate"), "true") == 0;
        }

        init(viewHandler, views, animate, active);
    }
};

extern const char tweakView[] = "tweakView";

static ResourceFactory::Type<tweakView, TweakViewHandlerResource> TweakViewHandlerType;

}
