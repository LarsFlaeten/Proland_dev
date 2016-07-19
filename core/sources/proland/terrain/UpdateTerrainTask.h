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

#ifndef _PROLAND_UPDATE_TERRAIN_TASK_H_
#define _PROLAND_UPDATE_TERRAIN_TASK_H_

#include "ork/scenegraph/AbstractTask.h"
#include "proland/terrain/TerrainNode.h"

using namespace ork;

namespace proland
{

/**
 * An AbstractTask to update a %terrain quadtree. This task simply calls
 * the TerrainNode#update method. In fact this method is called directly
 * by the #getTask method, which returns an empty task graph. Indeed the
 * %terrain quadtree must be up to date in order to get a correct task
 * graph for the proland::TileProducer tasks for the newly
 * created quads (these task graphs are computed by UpdateTileSamplersTask).
 * @ingroup terrain
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class UpdateTerrainTask : public AbstractTask
{
public:
    /**
     * Creates a new UpdateTerrainTask.
     *
     * @param terrain The %terrain whose quadtree must be updated. The first part
     *      of this "node.name" qualified name specifies the scene node containing
     *      the TerrainNode field. The second part specifies the name of this
     *      TerrainNode field.
     */
    UpdateTerrainTask(const QualifiedName &terrain);

    /**
     * Deletes this UpdateTerrainTask.
     */
    virtual ~UpdateTerrainTask();

    virtual ptr<Task> getTask(ptr<Object> context);

protected:
    /**
     * Creates an uninitialized UpdateTerrainTask.
     */
    UpdateTerrainTask();

    /**
     * Initializes this UpdateTerrainTask.
     *
     * @param terrain The %terrain whose quadtree must be updated. The first part
     *      of this "node.name" qualified name specifies the scene node containing
     *      the TerrainNode field. The second part specifies the name of this
     *      TerrainNode field.
     */
    void init(const QualifiedName &terrain);

    void swap(ptr<UpdateTerrainTask> t);

private:
    /**
     * The %terrain whose quadtree must be updated. The first part of this "node.name"
     * qualified name specifies the scene node containing the TerrainNode
     * field. The second part specifies the name of this TerrainNode field.
     */
    QualifiedName terrain;
};

}

#endif
