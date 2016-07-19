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

#ifndef _PROLAND_BASICVIEWHANDLER_H_
#define _PROLAND_BASICVIEWHANDLER_H_

#include "ork/scenegraph/SceneManager.h"
#include "ork/ui/EventHandler.h"
#include "proland/util/TerrainViewController.h"

using namespace ork;

namespace proland
{

/**
 * Provides access to a SceneManager, to a TerrainViewController and to
 * the screen to world transformation. Intended to be used with a
 * BasicViewHandler.
 * @ingroup proland_ui
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class ViewManager
{
public:
    /**
     * Returns the SceneManager. Used to find the light SceneNode
     * controlled by a BasicViewHandler.
     */
    virtual ptr<SceneManager> getScene() = 0;

    /**
     * Returns the TerrainViewController. Used by a BasicViewHandler to
     * control the view.
     */
    virtual ptr<TerrainViewController> getViewController() = 0;

    /**
     * Converts screen coordinates to world space coordinates. Used
     * by a BasicViewHandler to find the location of mouse clics.
     *
     * @param x a screen x coordinate.
     * @param y a screen y coordinate.
     * @return the world space point corresponding to (x,y) on screen.
     */
    virtual vec3d getWorldCoordinates(int x, int y) = 0;
};

/**
 * An EventHandler to control a TerrainViewController and a light source with
 * the mouse and/or the keyboard. This EventHandler relies on a ViewManager
 * to find the TerrainViewController, to find the light SceneNode (using a
 * SceneManager), and to convert between screen and world coordinates.
 * This implementation allows the user to move the view and the light with
 * the mouse and the PAGE_UP and PAGE_DOWN keys.
 * It also provides methods to instantly change the view and light positions,
 * and to start an animation to go smoothly from one position to another.
 * @ingroup proland_ui
 */
PROLAND_API class BasicViewHandler : public EventHandler
{
public:
    /**
     * A TerrainViewController position and a light source position.
     */
    struct Position {

        double x0, y0, theta, phi, d, sx, sy, sz;

        Position();
    };

    /**
     * Creates a new BasicViewHandler.
     *
     * @param smooth true to use exponential damping to go to target
     *      positions, false to go to target positions directly.
     * @param view the object used to access the view controller.
     * @param next the EventHandler to which the events not handled by this
     *      EventHandler must be forwarded.
     */
    BasicViewHandler(bool smooth, ViewManager *view, ptr<EventHandler> next);

    /**
     * Deletes this BasicViewHandler.
     */
    virtual ~BasicViewHandler();

    virtual void redisplay(double t, double dt);

    virtual void reshape(int x, int y);

    virtual void idle(bool damaged);

    virtual bool mouseClick(button b, state s, modifier m, int x, int y);

    virtual bool mouseMotion(int x, int y);

    virtual bool mousePassiveMotion(int x, int y);

    virtual bool mouseWheel(wheel b, modifier m, int x, int y);

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y);

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y);

    virtual bool specialKey(key k, modifier m, int x, int y);

    virtual bool specialKeyReleased(key k, modifier m, int x, int y);

    /**
     * Returns the current view and light positions.
     *
     * @param[out] p the current view and light position.
     */
    void getPosition(Position &p, bool light = true);

    /**
     * Sets the current view and light position.
     *
     * @param p the new view and light position.
     */
    void setPosition(const Position &p, bool light = true);

    /**
     * Starts an animation to go smoothly from the current position
     * to the given position.
     *
     * @param p the target position.
     */
    virtual void goToPosition(const Position &p);

    /**
     * Goes immediately to the given position.
     *
     * @param p the new view and light position.
     */
    void jumpToPosition(const Position &p);

protected:
    /**
     * The ViewManager to find the TerrainViewController, to find the light
     * SceneNode, and to convert between screen and world coordinates.
     */
    ViewManager *viewManager;

    /**
     * Creates an uninitialized BasicViewHandler.
     */
    BasicViewHandler();

    /**
     * Initializes this BasicViewHandler.
     * See #BasicViewHandler.
     */
    void init(bool smooth, ViewManager *view, ptr<EventHandler> next);

    /**
     * Returns the ViewManager used by this BasicViewHandler to find the
     * TerrainViewController, to find the light SceneNode, and to convert
     * between screen and world coordinates.
     */
    virtual ViewManager *getViewManager();

    /**
     * Updates the view for the current frame based on user inputs.
     */
    virtual void updateView(double t, double dt);

    void swap(ptr<BasicViewHandler> o);

private:
    /**
     * The EventHandler to which the events not handled by this EventHandler
     * must be forwarded.
     */
    ptr<EventHandler> next;

    /**
     * True to use exponential damping to go to target positions, false to go
     * to target positions directly.
     */
    bool smooth;

    /**
     * True if the PAGE_DOWN key is currently pressed.
     */
    bool near;

    /**
     * True if the PAGE_UP key is currently pressed.
     */
    bool far;

    /**
     * True if the UP key is currently pressed.
     */
    bool forward;

    /**
     * True if the DOWN key is currently pressed.
     */
    bool backward;

    /**
     * True if the LEFT key is currently pressed.
     */
    bool left;

    /**
     * True if the RIGHT key is currently pressed.
     */
    bool right;

    /**
     * A navigation mode.
     */
    enum userMode {
        move, ///< moves the "look-at" point.
        rotate, ///< rotates around the "look-at" point
        light ///< moves the light.
    };

    /**
     * The current navigation mode.
     */
    userMode mode;

    /**
     * The mouse x coordinate at the last call to #mouseClick or #mouseMotion.
     */
    int oldx;

    /**
     * The mouse x coordinate at the last call to #mouseClick or #mouseMotion.
     */
    int oldy;

    /**
     * The target position manipulated by the user via the mouse and keyboard.
     */
    Position target;

    /**
     * True if the target position #target is initialized.
     */
    bool initialized;

    /**
     * Start position for an animation between two positions.
     */
    Position start;

    /**
     * End position for an animation between two positions.
     */
    Position end;

    /**
     * Animation status. Negative values mean no animation.
     * 0 corresponds to the start position, 1 to the end position,
     * and values between 0 and 1 to intermediate positions between
     * the start and end positions.
     */
    double animation;
};

}

#endif
