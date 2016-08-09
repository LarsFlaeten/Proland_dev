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

#ifndef _PROLAND_PREPROCESS_TREE_
#define _PROLAND_PREPROCESS_TREE_

#include <vector>

#include "ork/render/Mesh.h"
#include "ork/render/Texture2D.h"
#include "ork/render/Texture2DArray.h"

using namespace std;
using namespace ork;

namespace proland
{

/**
 * A mesh with a texture, to describe a (part of) tree model.
 *
 * @ingroup preprocess
 * @author Eric Bruneton
 */
PROLAND_API class TreeMesh
{
public:
    struct Vertex
    {
        vec3f pos;

        vec2f uv;
    };

    ptr< Mesh<Vertex, unsigned int> > mesh;

    ptr<Texture2D> texture;

    TreeMesh(ptr< Mesh<Vertex, unsigned int> > mesh, ptr<Texture2D> texture);
};

/**
 * TODO.
 */
typedef void (*loadTreeMeshFunction)(vector<TreeMesh> &tree);

/**
 * TODO.
 */
typedef ptr<Texture2DArray> (*loadTreeViewsFunction)();

/**
 * Precomputes the textures for the given tree model.
 *
 * @param loadTree function to load the 3D tree model (whose bounding box
 *      must be -1:1 x -1:1 x -1:1).
 * @param output the folder where to write the generated textures.
 */
PROLAND_API void preprocessTree(loadTreeMeshFunction loadTree, int n, int w, const char *output);

/**
 * TODO.
 */
PROLAND_API void preprocessTreeTables(float minRadius, float maxRadius, float treeHeight, float treeTau, int nViews, loadTreeViewsFunction loadTree, const char *output);

/**
 * TODO.
 */
PROLAND_API void mergeTreeTables(const char* input1, const char* input2, const char *output);

/**
 * TODO.
 */
PROLAND_API void preprocessMultisample(const char *output);

}

#endif
