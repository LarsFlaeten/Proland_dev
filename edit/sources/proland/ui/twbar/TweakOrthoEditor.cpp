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

#include "proland/ui/twbar/TweakOrthoEditor.h"

#include "ork/resource/ResourceTemplate.h"
#include "proland/edit/EditOrthoProducer.h"

using namespace ork;

namespace proland
{

TW_CALL void ResetOrthoCB(void * clientData)
{
    EditOrthoProducer::getEditorHandler()->reset();
}

TW_CALL void SetOrthoEditorStateCallback(const void *value, void *clientData)
{
    static_cast<Editor*>(clientData)->setActive(*static_cast<const bool*>(value));
}

TW_CALL void GetOrthoEditorStateCallback(void *value, void *clientData)
{
    *static_cast<bool *>(value) = static_cast<Editor*>(clientData)->isActive();
}

TW_CALL void SetOrthoEditorGroupStateCallback(const void *value, void *clientData)
{
    EditOrthoProducer::getEditorHandler()->setActive(static_cast<Editor*>(clientData)->getGroup(), *static_cast<const bool*>(value));
}

TW_CALL void GetOrthoEditorGroupStateCallback(void *value, void *clientData)
{
    *static_cast<bool *>(value) = EditOrthoProducer::getEditorHandler()->isActive(static_cast<Editor*>(clientData)->getGroup());
}

TweakOrthoEditor::TweakOrthoEditor(bool active)
{
    TweakBarHandler::init("Ortho Editor", EditOrthoProducer::getEditorHandler(), active);
}

TweakOrthoEditor::~TweakOrthoEditor()
{
}

void TweakOrthoEditor::setActive(bool active)
{
    TweakBarHandler::setActive(active);
    eventHandler.cast<EditorHandler>()->setActive(active);
}

void TweakOrthoEditor::updateBar(TwBar *bar)
{
    EditorHandler *e = dynamic_cast<EditorHandler*>(eventHandler.get());
    if (e == NULL) {
        return;
    }

    TwAddVarRW(bar, "orthoBrushRadius", TW_TYPE_FLOAT, &(e->relativeRadius), " group=OrthoEditor label='Brush Radius' help='Size of the Ortho Editor Brush' min=0.0 step=0.01 ");
    TwAddVarRW(bar, "orthoBrushColor", TW_TYPE_COLOR4F, &(e->brushColor), " group=OrthoEditor label='Brush Color' help='Color applied to the texture' ");
    TwAddButton(bar, "orthoReset", ResetOrthoCB, NULL, " group=OrthoEditor label='Reset' help='Cancels all editing operations performed on active editors' ");
    char def[200];
    char name[20];
    sprintf(def, "%s/OrthoEditor label='Ortho Edition'", TwGetBarName(bar));
    TwDefine(def);

    set<string> groupNames;

    for (int i = 0; i < e->getEditorCount(); i++) {
        string n = e->getEditor(i)->getGroup();
        if (groupNames.find(n) == groupNames.end()) {
            groupNames.insert(n);
            sprintf(name, "orthoEditorGroup%d", i);
            sprintf(def, " group=%s label='Activate %s' help='Activate or Deactivate the selected Editor' ", n.c_str(), n.c_str());
            TwAddVarCB(bar, name, TW_TYPE_BOOLCPP, SetOrthoEditorGroupStateCallback, GetOrthoEditorGroupStateCallback, e->getEditor(i), def);

            sprintf(def, " group=%s ", n.c_str());
            TwAddSeparator(bar, NULL, def);

            sprintf(def, "%s/%s label='%s' group=OrthoEditor ", TwGetBarName(bar), n.c_str(), n.c_str());
            TwDefine(def);
        }
    }

    for (int i = 0; i < e->getEditorCount(); i++) {
        sprintf(name, "orthoEditor%d", i);
        sprintf(def, " group=%s label='%s' help='Activate or Deactivate the selected Editor' ", e->getEditor(i)->getGroup().c_str(), e->getEditor(i)->getName().c_str());
        TwAddVarCB(bar, name, TW_TYPE_BOOLCPP, SetOrthoEditorStateCallback, GetOrthoEditorStateCallback, e->getEditor(i), def);
    }
}

class TweakOrthoEditorResource : public ResourceTemplate<55, TweakOrthoEditor>
{
public:
    TweakOrthoEditorResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<55, TweakOrthoEditor> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,active,editor,");

        bool active = true;
        if (e->Attribute("active") != NULL) {
            active = strcmp(e->Attribute("active"), "true") == 0;
        }
        setActive(active);
    }
};

extern const char tweakOrtho[] = "tweakOrtho";

static ResourceFactory::Type<tweakOrtho, TweakOrthoEditorResource> TweakOrthoEditorType;

}
