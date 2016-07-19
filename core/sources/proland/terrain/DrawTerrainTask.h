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

#ifndef _PROLAND_DRAW_TERRAIN_TASK_H_
#define _PROLAND_DRAW_TERRAIN_TASK_H_

#include "ork/scenegraph/AbstractTask.h"
#include "proland/terrain/TerrainNode.h"
#include "proland/terrain/TileSampler.h"

using namespace ork;

namespace proland
{

/**
 * An AbstractTask to draw a %terrain. This task draws a mesh for each visible
 * leaf quad of the %terrain quadtree, using the current program and framebuffers.
 * Before drawing each quad, this task calls TileSampler::setTile on each
 * TileSampler associated with the TerrainNode in its owner SceneNode. It
 * also calls the Deformation#setUniforms methods using the %terrain deformation.
 * @ingroup terrain
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
PROLAND_API class DrawTerrainTask : public AbstractTask
{
public:
    /**
     * Creates a new DrawTerrainTask.
     *
     * @param terrain the %terrain to be drawn. The first part of this "node.name"
     *      qualified name specifies the scene node containing the TerrainNode
     *      field. The second part specifies the name of this TerrainNode field.
     * @param mesh the mesh to be used to draw each quad. The first part of this
     *      "node.name" qualified name specifies the scene node containing the mesh
     *      field. The second part specifies the name of this mesh field.
     * @param culling true to draw only visible leaf quads, false to draw all
     *      leaf quads.
     */
    DrawTerrainTask(const QualifiedName &terrain, const QualifiedName &mesh, bool culling);

    /**
     * Deletes this DrawTerrainTask.
     */
    virtual ~DrawTerrainTask();

    virtual ptr<Task> getTask(ptr<Object> context);

protected:
    /**
     * Creates an uninitialized DrawTerrainTask.
     */
    DrawTerrainTask();

    /**
     * Initializes this DrawTerrainTask.
     *
     * @param terrain the %terrain to be drawn. The first part of this "node.name"
     *      qualified name specifies the scene node containing the TerrainNode
     *      field. The second part specifies the name of this TerrainNode field.
     * @param mesh the mesh to be used to draw each quad. The first part of this
     *      "node.name" qualified name specifies the scene node containing the mesh
     *      field. The second part specifies the name of this mesh field.
     * @param culling true to draw only visible leaf quads, false to draw all
     *      leaf quads.
     */
    void init(const QualifiedName &terrain, const QualifiedName &mesh, bool culling);

    void swap(ptr<DrawTerrainTask> t);

private:
    /**
     * The %terrain to be drawn. The first part of this "node.name"
     * qualified name specifies the scene node containing the TerrainNode
     * field. The second part specifies the name of this TerrainNode field.
     */
    QualifiedName terrain;

    /**
     * The mesh to be drawn for each %terrain quad. The first part of this
     * "node.name" qualified name specifies the scene node containing the mesh
     * field. The second part specifies the name of this mesh field.
     */
    QualifiedName mesh;

    /**
     * True to draw only visible leaf quads, false to draw all leaf quads.
     */
    bool culling;

    /**
     * A Task to draw a %terrain.
     */
    class Impl : public Task
    {
    public:
        /**
         * The SceneNode describing the %terrain position and its associated
         * data (via TileSampler fields).
         */
        ptr<SceneNode> n;

        /**
         * The TerrainNode describing the %terrain and its quadtree.
         */
        ptr<TerrainNode> t;

        /**
         * The mesh to be drawn for each leaf quad.
         */
        ptr<MeshBuffers> m;

        /**
         * True to draw only visible leaf quads, false to draw all leaf quads.
         */
        bool culling;

        /**
         * True if one the TileSampler associated with this terrain
         * uses the asynchronous mode.
         */
        bool async;

        /**
         * Number of primitives (triangles, lines, etc) per *quarter* of
         * the grid mesh. Used to draw only parts of the mesh to fill holes
         * when using asynchronous mode.
         */
        int gridSize;

        /**
         * Creates a new Impl.
         *
         * @param n the SceneNode describing the %terrain position.
         * @param t the TerrainNode describing the %terrain and its quadtree.
         * @param m the mesh to be drawn for each leaf quad.
         * @param culling true to draw only visible leaf quads.
         */
        Impl(ptr<SceneNode> n, ptr<TerrainNode> t, ptr<MeshBuffers> m, bool culling);

        /**
         * Deletes this Impl.
         */
        virtual ~Impl();

        virtual bool run();

        /**
         * Finds the quads whose associated tiles are ready (this may not be
         * the case of all quads if async is true).
         *
         * @param q the %terrain quadtree to be drawn.
         * @param uniforms the TileSampler associated with the %terrain.
         */
        void findDrawableQuads(ptr<TerrainQuad> q, const std::vector< ptr<TileSampler> > &uniforms);

        /**
         * Draw the mesh #m for the leaf quads of the given quadtree. Before drawing each
         * quad, this method calls Deformation#setUniforms for this quad, and calls
         * TileSampler#setTile on each TileSampler the given uniforms vector.
         *
         * @param q the %terrain quadtree to be drawn.
         * @param uniforms the TileSampler associated with the %terrain.
         */
        void drawQuad(ptr<TerrainQuad> q, const std::vector< ptr<TileSampler> > &uniforms);
    };
};

}

#endif
