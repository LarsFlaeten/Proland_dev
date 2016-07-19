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

#ifndef _PROLAND_TERRAIN_VIEW_CONTROLLER_H_
#define _PROLAND_TERRAIN_VIEW_CONTROLLER_H_

#include "ork/scenegraph/SceneNode.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup proland_util util
 * Provides utility classes to control camera movements.
 * @ingroup proland
 */

/**
 * A view controller for flat terrains. The camera position is specified
 * from a "look at" position (x0,y0) on ground, with a distance d between
 * camera and this position, and two angles (theta,phi) for the direction
 * of this vector. The #update method sets the localToParent transformation
 * of the SceneNode associated with this controller, which is intended to
 * represent the camera position and orientation in the scene.
 * @ingroup proland_util
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class TerrainViewController : public Object
{
public:
    /**
     * The field of view angle.
     */
    double fov;

    /**
     * The x coordinate of the point the camera is looking at on the ground.
     */
    double x0;

    /**
     * The y coordinate of the point the camera is looking at on the ground.
     */
    double y0;

    /**
     * The zenith angle of the vector between the "look at" point and the camera.
     */
    double theta;

    /**
     * The azimuth angle of the vector between the "look at" point and the camera.
     */
    double phi;

    /**
     * The distance between the "look at" point and the camera.
     */
    double d;

    /**
     * Zoom factor (realized by increasing d and decreasing fov).
     */
    double zoom;

    /**
     * The camera position in world space resulting from the x0,y0,theta,phi,
     * and d parameters.
     */
    vec3d position;

    /**
     * Creates a new TerrainViewController to control the given SceneNode.
     *
     * @param node a SceneNode representing a camera position and orientation
     *      in the scene.
     * @param d0 the initial valued of the #d distance.
     */
    TerrainViewController(ptr<SceneNode> node, double d0);

    /**
     * Deletes this TerrainViewController.
     */
    virtual ~TerrainViewController();

    /**
     * Returns the SceneNode associated with this TerrainViewController.
     * This SceneNode represents a camera position and orientation in the
     * scene.
     */
    ptr<SceneNode> getNode();

    /**
     * Sets the SceneNode associated with this TerrainViewController.
     *
     * @param node a SceneNode representing a camera position and orientation
     *      in the scene.
     */
    void setNode(ptr<SceneNode> node);

    /**
     * Returns the %terrain elevation below the camera.
     */
    float getGroundHeight();

    /**
     * Sets the %terrain elevation below the camera. This elevation is used
     * to adjust the camera position so that it is not below the ground.
     *
     * @param groundHeight the %terrain elevation below the camera.
     */
    void setGroundHeight(float groundHeight);

    /**
     * Returns the height of the camera above the z=0 surface.
     */
    virtual double getHeight();

    /**
     * Moves the "look at" point so that "oldp" appears at the position of "p"
     * on screen.
     *
     * @param oldp a %terrain point.
     * @param p another %terrain point.
     */
    virtual void move(vec3d &oldp, vec3d &p);

    virtual void moveForward(double distance);

    virtual void turn(double angle);

    /**
     * Sets the position as the interpolation of the two given positions with
     * the interpolation parameter t (between 0 and 1). The source position is
     * sx0,sy0,stheta,sphi,sd, the destination is dx0,dy0,dtheta,dphi,dd.
     *
     * @return the new value of the interpolation parameter t.
     */
    virtual double interpolate(double sx0, double sy0, double stheta, double sphi, double sd,
            double dx0, double dy0, double dtheta, double dphi, double dd, double t);

    virtual void interpolatePos(double sx0, double sy0, double dx0, double dy0, double t, double &x0, double &y0);

    /**
     * Returns a direction interpolated between the two given direction.
     *
     * @param slon start longitude.
     * @param slat start latitude.
     * @param elon end longitude.
     * @param elat end latitude.
     * @param t interpolation parameter between 0 and 1.
     * @param[out] lon interpolated longitude.
     * @param[out] lat interpolated latitude.
     */
    void interpolateDirection(double slon, double slat, double elon, double elat, double t, double &lon, double &lat);

    /**
     * Sets the localToParent transform of the SceneNode associated with this
     * TerrainViewController. The transform is computed from the view parameters
     * x0,y0,theta,phi and d.
     */
    virtual void update();

    /**
     * Sets the camera to screen perspective projection.
     *
     * @param znear an optional znear plane (0.0 means that a default value
     *      must be used).
     * @param zfar an optional zfar plane (0.0 means that a default value
     *      must be used).
     * @param viewport an optional viewport to select a part of the image.
     *      The default value [-1:1]x[-1:1] selects the whole image.
     */
    virtual void setProjection(float znear = 0.0f, float zfar = 0.0f, vec4f viewport = vec4f(-1.0f, 1.0f, -1.0f, 1.0f));

protected:
    /**
     * The SceneNode associated with this TerrainViewController.
     */
    ptr<SceneNode> node;

    /**
     * The %terrain elevation below the camera.
     */
    float groundHeight;
};

}

#endif
