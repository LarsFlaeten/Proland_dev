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

#ifndef _PROLAND_TWEAKGRAPHLAYER_H_
#define _PROLAND_TWEAKGRAPHLAYER_H_

#include "proland/ui/twbar/TweakBarHandler.h"
#include "proland/edit/EditGraphOrthoLayer.h"

namespace proland
{

/**
 * A TweakBarHandler providing %graph edition tools.
 * @ingroup twbar
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class TweakGraphLayer : public TweakBarHandler
{
public:
    enum CONTEXT_MENU_MODE
    {
        HIDDEN = 0,
        CLOSE = 1,
        CLICK = 2,
        DISPLAY_MENU = 3
    };

    TweakGraphLayer(bool active);

    virtual ~TweakGraphLayer();

    virtual void setActive(bool active);

    virtual void redisplay(double t, double dt, bool &needUpdate);

    virtual bool mouseClick(EventHandler::button b, EventHandler::state s, EventHandler::modifier m, int x, int y, bool &needUpdate);

    /**
     * Context Menu. Appears when right clicking on a curve.
     * CTRL + right click will open the menu without changing the current selection.
     * Useful when you don't see the curve you are editing or when there are lots of nearly curves.
     */
    static TwBar *contextBar;

protected:
    TweakGraphLayer();

    virtual void init(bool active);

    virtual void updateBar(TwBar *bar);

    /**
     * Creates a new Edition Tweakbar.
     */
    virtual void createTweakBar();

    /**
     * Displays currently selected Curve Info in TwBar b (including width, type... and points info).
     *
     * @param b the TwBar in which to display the Curve Infos.
     * @param curveData the TwEditBarData containing informations about the current selection.
     */
    virtual void displayCurveInfo(TwBar *b, EditGraphOrthoLayer::SelectionData* curveData);

    /**
     * Hides the context menu.
     */
    void closeMenu();

    /**
     * Opens a context menu, whose content will depend of the current selection.
     */
    void displayMenu(int mousePosX, int mousePosY);

    /**
     * Contains data on the current selection. see TWEditBarData class.
     */
    EditGraphOrthoLayer::SelectionData selectedCurveData;

    /**
     * True if the context menu is opened.
     */
    int displayContext;

    bool initialized;

    vec2i menuPos;

    int lastActiveGraph;

};

}

#endif
