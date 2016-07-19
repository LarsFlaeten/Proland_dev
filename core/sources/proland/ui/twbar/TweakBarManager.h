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

#ifndef _PROLAND_TWEAKBARMANAGER_H_
#define _PROLAND_TWEAKBARMANAGER_H_

#include <vector>

#include "AntTweakBar.h"

#include "ork/scenegraph/AbstractTask.h"
#include "proland/ui/twbar/TweakBarHandler.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * Provides a modular tweak bar made of several TweakBarHandler.
 * Each TweakBarHandler provides controls to control and edit some
 * aspects of a scene. Each TweakBarHandler can be activated or
 * deactivated. The TweakBarHandler can be permanently active. If not,
 * they can be mutually exclusive with other TweakBarHandler. In this
 * case the TweakBarManager ensures that at most one such TweakBarHandler
 * is active at the same time.
 * A TweakBarManager is an EventHandler. The events are first sent to
 * the tweak bar. Unhandled events are then forwarded to all active
 * TweakBarHandler and then, if still unhandled, to an external EventHandler.
 * @ingroup twbar
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class TweakBarManager : public EventHandler
{
public:
    /**
     * A TweakBarHandler with additional options.
     */
    struct BarData
    {
    public:
        /**
         * The TweakBarManager to which this BarData belongs.
         */
        TweakBarManager *owner;

        /**
         * A TweakBarHandler.
         */
        ptr<TweakBarHandler> bar;

        /**
         * True if this TweakBarHandler must not be active at the same time
         * as the other exclusive TweakBarHandler.
         */
        bool exclusive;

        /**
         * True if this TweakBarHandler must always be active.
         */
        bool permanent;

        /**
         * Shortcut key to activate or deactive this TweakBarHandler, or 0.
         */
        char k;

        /**
         * Creates a new BarData.
         *
         * @param owner the TweakBarManager to which this BarData belongs.
         * @param bar a TweakBarHandler.
         * @param exclusive true if this TweakBarHandler must not be active
         *      at the same time as the other exclusive TweakBarHandler.
         * @param permanent true if this TweakBarHandler must always be active.
         * @param key shortcut key to activate or deactive this
         *      TweakBarHandler, or 0.
         */
        BarData(TweakBarManager *owner, ptr<TweakBarHandler> bar, bool exclusive, bool permanent, char k);

        /**
         * Activates or deactivates this TweakBarHandler.
         *
         * @param active true to activate this TweakBarHandler, false otherwise.
         */
        void setActive(bool active);
    };

    /**
     * Creates a new TweakBarManager.
     *
     * @param bars the TweakBarHandler managed by this manager.
     * @param minimized true if the tweak bar must be initially minimized.
     */
    TweakBarManager(vector<BarData> bars, bool minimized = true);

    /**
     * Deletes this TweakBarManager.
     */
    virtual ~TweakBarManager();

    /**
     * Returns the EventHandler to which the events not handled by this
     * TweakBarManager must be forwarded.
     */
    ptr<EventHandler> getNext();

    /**
     * Sets the EventHandler to which the events not handled by this
     * TweakBarManager must be forwarded.
     *
     * @param an EventHandler.
     */
    void setNext(ptr<EventHandler> next);

    virtual void redisplay(double t, double dt);

    virtual void reshape(int x, int y);

    virtual void idle(bool damaged);

    virtual bool mouseClick(button b, state s, modifier m, int x, int y);

    virtual bool mouseWheel(wheel b, modifier m, int x, int y);

    virtual bool mouseMotion(int x, int y);

    virtual bool mousePassiveMotion(int x, int y);

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y);

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y);

    virtual bool specialKey(key  k, modifier m, int x, int y);

    virtual bool specialKeyReleased(key  k, modifier m, int x, int y);

    /**
     * Deactivates all the exclusive TweakBarHandler.
     */
    void resetStates();

protected:
    /**
     * Creates an uninitialized TweakBarManager.
     */
    TweakBarManager();

    /**
     * Initializes this TweakBarManager.
     * See #TweakBarManager.
     */
    void init(vector<BarData> bars, bool minimized = true);

    void swap(ptr<TweakBarManager> t);

private:
    /**
     * The tweak bar managed by this TweakBarManager.
     */
    TwBar *selectBar;

    /**
     * The TweakBarHandler managed by this TweakBarManager.
     */
    vector<BarData> bars;

    /**
     * The EventHandler to which the events not handled by this
     * TweakBarManager must be forwarded.
     */
    ptr<EventHandler> next;

    /**
     * True if the tweak bar must be initially minimized.
     */
    int minimized;

    /**
     * True if the tweak bar has been initialized.
     */
    bool initialized;

    /**
     * True if the tweak bar must be updated.
     */
    bool updated;

    /**
     * Initializes the tweak bar managed by this manager. This method
     * clears the bar and calls TweakBarHandler#updateBar on each
     * TweakBarHandler so that it can add its own controls to the bar.
     */
    void initBar();
};

}

#endif
