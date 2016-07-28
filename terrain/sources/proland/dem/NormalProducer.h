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

#ifndef _PROLAND_NORMAL_PRODUCER_H_
#define _PROLAND_NORMAL_PRODUCER_H_

#include "ork/render/FrameBuffer.h"
#include "ork/render/Program.h"
#include "ork/render/Texture2D.h"
#include "proland/producer/TileProducer.h"

using namespace ork;

namespace proland
{

/**
 * A TileProducer to generate %terrain normals from %terrain elevations on GPU.
 * @ingroup dem
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class NormalProducer : public TileProducer
{
public:
    /**
     * Creates a new NormalProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a GPUTileStorage with textures of two or four
     *      components per pixel. If two components are used only one normal
     *      is stored per pixel (n.x and n.y are stored, n.z is implicit). If
     *      four components are used a coarse normal is also stored, to provide
     *      smooth interpolation of normals between quadtree levels. If floating
     *      point textures are used normals components vary between -1 and 1. If
     *      non floating point textures are used, this range is scaled to 0-1 to
     *      fit in the texture format.
     * @param elevationTiles the %producer producing the elevation tiles on GPU.
     *      The underlying storage must be a GPUTileStorage with floating point
     *      textures with at least 3 components per pixel (4 if 'deform' is true).
     *      The elevation tile size, without borders, must be equal to the normal
     *      tile size, minus 1.
     * @param normalTexture a texture used to produce the tiles. Its size must be
     *      equal to the normal tile size. Its format must be the same as the
     *      format used for the tile storage of this %producer.
     * @param normals the Program to compute normals from elevations on GPU.
     * @param gridMeshSize the size of the grid that will be used to render each tile.
     *      Must be the tile size (minus 1) divided by a power of two.
     * @param deform true if the produced normals will be mapped on a spherical
     *      %terrain.
     */
    NormalProducer(ptr<TileCache> cache, ptr<TileProducer> elevationTiles,
        ptr<Texture2D> normalTexture, ptr<Program> normals, int gridMeshSize, bool deform = false);

    /**
     * Deletes this NormalProducer.
     */
    virtual ~NormalProducer();

    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const;

    virtual void setRootQuadSize(float size);

    virtual int getBorder();

    virtual bool hasTile(int level, int tx, int ty);

protected:
    /**
     * The Program to compute normals from elevations on GPU.
     */
    ptr<Program> normals;

    /**
     * Creates an uninitialized NormalProducer.
     */
    NormalProducer();

    /**
     * Initializes this NormalProducer. See #NormalProducer.
     */
    void init(ptr<TileCache> cache, ptr<TileProducer> elevationTiles,
        ptr<Texture2D> normalTexture, ptr<Program> normals, int gridMeshSize, bool deform = false);

    virtual void *getContext() const;

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual void beginCreateTile();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void endCreateTile();

    virtual void stopCreateTile(int level, int tx, int ty);

    virtual void swap(ptr<NormalProducer> p);

private:
    ptr<FrameBuffer> frameBuffer;

    /**
     * The %producer producing the elevation tiles on GPU. The underlying storage
     * must be a GPUTileStorage with floating point textures with at least 3
     * components per pixel (4 if 'deform' is true). The elevation tile size,
     * without borders, must be equal to the normal tile size, minus 1.
     */
    ptr<TileProducer> elevationTiles;

    /**
     * A texture used to produce the tiles. Its size must be  equal to the normal
     * tile size. Its format must be the same as the format used for the tile
     * storage of this %producer.
     */
    ptr<Texture2D> normalTexture;

    /**
     * true if the produced elevations will be mapped on a spherical %terrain.
     */
    bool deform;

    /**
     * The size of the grid that will be used to render each tile.
     * Must be the tile size (without borders) divided by a power of two.
     */
    int gridMeshSize;

    ptr<Uniform3f> tileSDFU;

    ptr<UniformSampler> elevationSamplerU;

    ptr<Uniform4f> elevationOSLU;

    ptr<UniformSampler> normalSamplerU;

    ptr<Uniform4f> normalOSLU;

    ptr<UniformMatrix4f> patchCornersU;

    ptr<UniformMatrix4f> patchVerticalsU;

    ptr<Uniform4f> patchCornerNormsU;

    ptr<UniformMatrix3f> worldToTangentFrameU;

    ptr<UniformMatrix3f> parentToTangentFrameU;

    ptr<Uniform4f> deformU;

    static static_ptr<FrameBuffer> old;
};

}

#endif
