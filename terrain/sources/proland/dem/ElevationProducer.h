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

#ifndef _PROLAND_ELEVATION_PRODUCER_H_
#define _PROLAND_ELEVATION_PRODUCER_H_

#include "ork/resource/Resource.h"
#include "ork/render/FrameBuffer.h"
#include "ork/render/Program.h"
#include "ork/render/Texture2D.h"
#include "ork/render/RenderBuffer.h"
#include "proland/producer/TileProducer.h"

using namespace ork;

namespace proland
{

/**
 * @defgroup dem dem
 * Provides producers and layers for %terrain elevations and normals.
 * @ingroup proland
 */

/**
 * A TileProducer to create elevation tiles on GPU from CPU residual tiles.
 * This %producer is intended to be used with a ResidualProducer, but any other
 * %producer creating floating point CPU tiles can be used instead. This
 * %producer can create additional details with noise whose amplitude and frequency
 * and be specified for each level (and the upsampling shader can further modify this
 * noise to make it vary with the slope, curvature, altitude, etc). In fact
 * the residual %producer is optional. When no residuals are specified, a fully
 * random fractal %terrain is created from noise amplitudes. See \ref sec-elevation.
 * @ingroup dem
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class ElevationProducer : public TileProducer
{
public:
    /**
     * Creates a new ElevationProducer.
     *
     * @param cache the cache to store the produced tiles. The underlying
     *      storage must be a GPUTileStorage with floating point textures of
     *      at least two components per pixel.
     * @param residualTiles the %producer producing the residual tiles. This
     *      %producer should produce its tiles in a CPUTileStorage of float type.
     *      Maybe NULL to create a fully random fractal %terrain. The size of the
     *      residual tiles (without borders) must be a multiple of the size of the
     *      elevation tiles (without borders).
     * @param demTexture a texture used to produce the tiles. Its size must be equal
     *      to the elevation tile size (including borders). Its format must be RGBA32F.
     * @param layerTexture a texture used to combine the layers of this %producer
     *      with the raw %terrain (maybe NULL if there are no layers; otherwise its
     *      size must be equal to the elevation tile size, including borders. Its
     *      format must be RGBA32F).
     * @param residualTexture a texture used to produce the tiles. Its size must be
     *      equal to the elevation tile size (including borders). Its format must be
     *      I32F.
     * @param upsample the Program to perform the upsampling and add procedure on GPU.
     *      See \ref sec-elevation.
     * @param blend the Program to blend the layers of this %producer with the raw
     *      %terrain elevations.
     * @param gridMeshSize the size of the grid that will be used to render each tile.
     *      Must be the tile size (without borders) divided by a power of two.
     * @param noiseAmp the amplitude of the noise to be added for each level
     *      (one amplitude per level).
     * @param flipDiagonals true if the grid used to render each tile will use diagonal
     *      flipping to reduce geometric aliasing.
     */
    ElevationProducer(ptr<TileCache> cache, ptr<TileProducer> residualTiles,
        ptr<Texture2D> demTexture, ptr<Texture2D> layerTexture, ptr<Texture2D> residualTexture,
        ptr<Program> upsample, ptr<Program> blend, int gridMeshSize,
        std::vector<float> &noiseAmp, bool flipDiagonals = false);

    /**
     * Deletes this ElevationProducer.
     */
    virtual ~ElevationProducer();

    virtual void getReferencedProducers(std::vector< ptr<TileProducer> > &producers) const;

    virtual void setRootQuadSize(float size);

    virtual int getBorder();

protected:
    ptr<FrameBuffer> frameBuffer;

    /**
     * The Program to perform the upsampling and add procedure on GPU.
     * See \ref sec-elevation.
     */
    ptr<Program> upsample;

    /**
     * The Program to blend the layers of this %producer with the raw %terrain
     * elevations.
     */
    ptr<Program> blend;

    /**
     * The %producer producing the residual tiles. This %producer should produce its
     * tiles in a CPUTileStorage of float type. Maybe NULL to create a fully random
     * fractal %terrain. The size of the residual tiles (without borders) must be a
     * multiple of the size of the elevation tiles (without borders).
     */
    ptr<TileProducer> residualTiles;

    /**
     * A texture used to produce the tiles. Its size must be equal to the elevation
     * tile size (including borders). Its format must be RGBA32F.
     */
    ptr<Texture2D> demTexture;

    /**
     * A texture used to produce the tiles. Its size must be equal to the elevation
     * tile size (including borders). Its format must be I32F.
     */
    ptr<Texture2D> residualTexture;

    /**
     * A texture used to combine the layers of this %producer with the raw %terrain
     * (maybe NULL if there are no layers; otherwise its size must be equal to the
     * elevation tile size, including borders. Its format must be RGBA32F).
     */
    ptr<Texture2D> layerTexture;

    /**
     * Cube face ID for producers targeting spherical terrains.
     */
    int face;

    /**
     * Creates an uninitialized ElevationProducer.
     */
    ElevationProducer();

    /**
     * Initializes this ElevationProducer. See #ElevationProducer.
     */
    void init(ptr<TileCache> cache, ptr<TileProducer> residualTiles,
        ptr<Texture2D> demTexture, ptr<Texture2D> layerTexture, ptr<Texture2D> residualTexture,
        ptr<Program> upsample, ptr<Program> blend, int gridMeshSize,
        std::vector<float> &noiseAmp, bool flipDiagonals = false);

    /**
     * Initializes this ElevationProducer from a Resource.
     *
     * @param manager the manager that will manage the created %resource.
     * @param r the %resource.
     * @param name the %resource name.
     * @param desc the %resource descriptor.
     * @param e an optional XML element providing contextual information (such
     *      as the XML element in which the %resource descriptor was found).
     */
    void init(ptr<ResourceManager> manager, Resource *r, const std::string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL);

    virtual void *getContext() const;

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual void beginCreateTile();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void endCreateTile();

    virtual void stopCreateTile(int level, int tx, int ty);

    virtual void swap(ptr<ElevationProducer> p);

private:
    /**
     * The amplitude of the noise to be added for each level (one amplitude per level).
     */
    std::vector<float> noiseAmp;

    /**
     * A buffer to convert a residual tile produced by #residualTiles to the
     * appropriate size.
     */
    float *residualTile;

    /**
     * The size of the grid that will be used to render each tile.
     * Must be the tile size (without borders) divided by a power of two.
     */
    int gridMeshSize;

    /**
     * true if the grid used to render each tile will use diagonal flipping to
     * reduce geometric aliasing.
     */
    bool flipDiagonals;

    ptr<Texture2DArray> noiseTexture;

    ptr<Uniform4f> tileWSDFU;

    ptr<UniformSampler> coarseLevelSamplerU;

    ptr<Uniform4f> coarseLevelOSLU;

    ptr<UniformSampler> residualSamplerU;

    ptr<Uniform4f> residualOSHU;

    ptr<UniformSampler> noiseSamplerU;

    ptr<Uniform4f> noiseUVLHU;

    ptr<UniformSampler> elevationSamplerU;

    ptr<UniformSampler> blendCoarseLevelSamplerU;

    ptr<Uniform1f> blendScaleU;

    static static_ptr<FrameBuffer> old;
};

}

#endif
