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

#ifndef _PROLAND_DRAW_OCEAN_TASK_H_
#define _PROLAND_DRAW_OCEAN_TASK_H_

#include "ork/scenegraph/AbstractTask.h"
#include "ork/render/Mesh.h"
#include "ork/render/Uniform.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup ocean ocean
 * Provides a task to draw a flat or spherical animated %ocean.
 * @ingroup proland
 */

/**
 * An AbstractTask to draw a flat or spherical %ocean.
 * @ingroup ocean
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class DrawOceanTask : public AbstractTask
{
public:
    /**
     * Creates a new DrawOceanTask.
     *
     * @param radius the radius of the planet for a spherical ocean, or
     *      0 for a flat ocean.
     * @param zmin the maximum altitude at which the ocean must be
     *      displayed.
     * @param brdfShader the Shader used to render the ocean surface.
     */
    DrawOceanTask(float radius, float zmin, ptr<Module> brdfShader);

    /**
     * Deletes this DrawOceanTask.
     */
    virtual ~DrawOceanTask();

    virtual ptr<Task> getTask(ptr<Object> context);

protected:
    /**
     * Creates an uninitialized DrawOceanTask.
     */
    DrawOceanTask();

    /**
     * Initializes this DrawOceanTask.
     *
     * @param radius the radius of the planet for a spherical ocean, or
     *      0 for a flat ocean.
     * @param zmin the maximum altitude at which the ocean must be
     *      displayed.
     * @param brdfShader the Shader used to display the ocean surface.
     */
    void init(float radius, float zmin, ptr<Module> brdfShader);

    void swap(ptr<DrawOceanTask> t);

private:
    /**
     * The radius of the planet for a spherical ocean, or 0 for a flat ocean.
     */
    float radius;

    /**
     * The maximum altitude at which the ocean must be displayed.
     */
    float zmin;

    /**
     * Number of wave trains used to synthesize the ocean surface.
     */
    int nbWaves;

    /**
     * Minimum wavelength of the waves.
     */
    float lambdaMin;

    /**
     * Maximum wavelength of the waves.
     */
    float lambdaMax;

    /**
     * Parameter to color the height of waves.
     */
    float heightMax;

    /**
     * Color of the seabed.
     */
    vec3f seaColor;

    // -------

    /**
     * Variance of the x slope over the sea surface.
     */
    float sigmaXsq;

    /**
     * Variance of the y slope over the sea surface.
     */
    float sigmaYsq;

    /**
     * Average height of the sea surface.
     */
    float meanHeight;

    /**
     * Variance of the sea surface height.
     */
    float heightVariance;

    /**
     * Maximum amplitude between crests and throughs.
     */
    float amplitudeMax;

    // -------

    /**
     * Number of pixels per cell to use for the screen space grid
     * used to display the ocean surface.
     */
    int resolution;

    /**
     * Current width of the viewport, in pixels.
     */
    int screenWidth;

    /**
     * Current height of the viewport, in pixels.
     */
    int screenHeight;

    /**
     * The mesh used to display the ocean surface.
     */
    ptr< Mesh<vec2f, unsigned int> > screenGrid;

    // -------

    mat4d oldLtoo;

    vec3d offset;

    // -------

    /**
     * The Shader used to render the ocean surface.
     */
    ptr<Module> brdfShader;

    ptr<Uniform1f> nbWavesU;

    ptr<UniformSampler> wavesU;

    ptr<UniformMatrix4f> cameraToOceanU;

    ptr<UniformMatrix4f> screenToCameraU;

    ptr<UniformMatrix4f> cameraToScreenU;

    ptr<UniformMatrix4f> oceanToWorldU;

    ptr<UniformMatrix3f> oceanToCameraU;

    ptr<Uniform3f> oceanCameraPosU;

    ptr<Uniform3f> oceanSunDirU;

    ptr<Uniform3f> horizon1U;

    ptr<Uniform3f> horizon2U;

    ptr<Uniform1f> timeU;

    ptr<Uniform1f> radiusU;

    ptr<Uniform1f> heightOffsetU;

    ptr<Uniform4f> lodsU;

    void generateWaves();

    class Impl : public Task
    {
    public:
        ptr<SceneNode> n;

        ptr<DrawOceanTask> o;

        Impl(ptr<SceneNode> n, ptr<DrawOceanTask> owner);

        virtual ~Impl();

        virtual bool run();
    };
};

}

#endif
