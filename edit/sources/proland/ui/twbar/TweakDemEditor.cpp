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

#include "proland/ui/twbar/TweakDemEditor.h"

#include "ork/resource/ResourceTemplate.h"
#include "proland/edit/EditElevationProducer.h"

using namespace ork;

namespace proland
{

TW_CALL void ResetDemCB(void * clientData)
{
    EditElevationProducer::getEditorHandler()->reset();
}

TW_CALL void SetDemEditorStateCallback(const void *value, void *clientData)
{
    static_cast<Editor*>(clientData)->setActive(*static_cast<const bool*>(value));
}

TW_CALL void GetDemEditorStateCallback(void *value, void *clientData)
{
    *static_cast<bool *>(value) = static_cast<Editor*>(clientData)->isActive();
}

TW_CALL void SetDemEditorGroupStateCallback(const void *value, void *clientData)
{
    EditElevationProducer::getEditorHandler()->setActive(static_cast<Editor*>(clientData)->getGroup(), *static_cast<const bool*>(value));
}

TW_CALL void GetDemEditorGroupStateCallback(void *value, void *clientData)
{
    *static_cast<bool *>(value) = EditElevationProducer::getEditorHandler()->isActive(static_cast<Editor*>(clientData)->getGroup());
}

TW_CALL void SetDemEditModeCallback(const void *value, void *clientData)
{
    ptr<EditorHandler> e = EditElevationProducer::getEditorHandler();
    for (int i = 0; i < e->getEditorCount(); ++i) {
        dynamic_cast<EditElevationProducer*>(e->getEditor(i))->setEditMode(*static_cast<const BlendEquation*>(value));
    }
}

TW_CALL void GetDemEditModeCallback(void *value, void *clientData)
{
    *static_cast<BlendEquation*>(value) = dynamic_cast<EditElevationProducer*>(EditElevationProducer::getEditorHandler()->getEditor(0))->getEditMode();
}

TweakDemEditor::TweakDemEditor(bool active)
{
    TweakBarHandler::init("Dem Editor", EditElevationProducer::getEditorHandler(), active);
}

TweakDemEditor::~TweakDemEditor()
{
}

void TweakDemEditor::setActive(bool active)
{
    TweakBarHandler::setActive(active);
    eventHandler.cast<EditorHandler>()->setActive(active);
}

void TweakDemEditor::updateBar(TwBar *bar)
{
    EditorHandler *e = dynamic_cast<EditorHandler*>(eventHandler.get());
    if (e == NULL) {
        return;
    }

    // Adding the list of edited graphs
    TwEnumVal *editModes= new TwEnumVal[2];
    editModes[0].Value = ADD;
    editModes[0].Label = "ADD";
    editModes[1].Value = MAX;
    editModes[1].Label = "MAX";

    TwType editType = TwDefineEnum("EditType", editModes, 2);
    TwAddVarCB(bar, "EditMode", editType, SetDemEditModeCallback, GetDemEditModeCallback, NULL, " label='Edit Mode' group='DemEditor' key='e' ");

    TwAddVarRW(bar, "demBrushRadius", TW_TYPE_FLOAT, &(e->relativeRadius), " group=DemEditor label='Brush Radius' help='Size of the Dem Editor Brush' min=0.0 step=0.01 ");
    TwAddVarRW(bar, "demBrushColor", TW_TYPE_FLOAT, &(e->brushColor[0]), " group=DemEditor label='Brush altitude' help='Altitude applied to the texture' step='0.1' ");
    TwAddButton(bar, "demReset", ResetDemCB, NULL, " group=DemEditor label='Reset' help='Cancels all editing operations performed on active editors' ");
    char def[200];
    sprintf(def, "%s/DemEditor label='Dem Edition'", TwGetBarName(bar));
        char name[20];
    TwDefine(def);

    set<string> groupNames;
    for (int i = 0; i < e->getEditorCount(); i++) {
        string n = e->getEditor(i)->getGroup();
        if (groupNames.find(n) == groupNames.end()) {
            groupNames.insert(n);
            sprintf(name, "demEditorGroup%d", i);
            sprintf(def, " group=%s label='Activate %s' help='Activate or Deactivate the selected Editor' ", n.c_str(), n.c_str());
            TwAddVarCB(bar, name, TW_TYPE_BOOLCPP, SetDemEditorGroupStateCallback, GetDemEditorGroupStateCallback, e->getEditor(i), def);

            sprintf(def, " group=%s ", n.c_str());
            TwAddSeparator(bar, NULL, def);

            sprintf(def, "%s/%s label='%s' group=DemEditor ", TwGetBarName(bar), n.c_str(), n.c_str());
            TwDefine(def);
        }
    }

    for (int i = 0; i < e->getEditorCount(); i++) {
        sprintf(name, "DemEditor%d", i);
        sprintf(def, " group=%s label='%s' help='Activate or Deactivate the selected Editor' ", e->getEditor(i)->getName().c_str(), e->getEditor(i)->getName().c_str());
        TwAddVarCB(bar, name, TW_TYPE_BOOLCPP, SetDemEditorStateCallback, GetDemEditorStateCallback, e->getEditor(i), def);
    }
    delete[] editModes;
}

class TweakDemEditorResource : public ResourceTemplate<55, TweakDemEditor>
{
public:
    TweakDemEditorResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<55, TweakDemEditor> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,active,");

        bool active = true;
        if (e->Attribute("active") != NULL) {
            active = strcmp(e->Attribute("active"), "true") == 0;
        }
        setActive(active);
    }
};

extern const char tweakDem[] = "tweakDem";

static ResourceFactory::Type<tweakDem, TweakDemEditorResource> TweakDemEditorType;

}
