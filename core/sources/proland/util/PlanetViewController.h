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

#ifndef _PROLAND_PLANET_VIEW_CONTROLLER_H_
#define _PROLAND_PLANET_VIEW_CONTROLLER_H_

#include "proland/util/TerrainViewController.h"

using namespace ork;

namespace proland
{

/**
 * A TerrainViewController for spherical terrains (i.e., planets). This subclass
 * interprets the #x0 and #y0 fields as longitudes and latitudes on the planet,
 * and considers #theta and #phi as relative to the tangent plane at the (#x0,#y0)
 * point.
 * @ingroup proland_util
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class PlanetViewController : public TerrainViewController
{
public:
    /**
     * The radius of the planet at sea level.
     */
    const double R;

    /**
     * Creates a new PlanetViewController.
     *
     * @param node a SceneNode representing a camera position and orientation
     *      in the scene.
     * @param R the planet radius at sea level.
     */
    PlanetViewController(ptr<SceneNode> node, double R);

    /**
     * Deletes this PlanetViewController.
     */
    virtual ~PlanetViewController();

    virtual double getHeight();

    virtual void move(vec3d &oldp, vec3d &p);

    virtual void moveForward(double distance);

    virtual void turn(double angle);

    virtual double interpolate(double sx0, double sy0, double stheta, double sphi, double sd,
            double dx0, double dy0, double dtheta, double dphi, double dd, double t);

    virtual void interpolatePos(double sx0, double sy0, double dx0, double dy0, double t, double &x0, double &y0);

    virtual void update();
};

}

#endif
