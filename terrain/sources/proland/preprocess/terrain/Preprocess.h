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

#ifndef _PROLAND_PREPROCESS_
#define _PROLAND_PREPROCESS_

#include <string>
#include <map>
#include <list>

#include "ork/math/vec4.h"

using namespace std;
using namespace ork;

namespace proland
{

/**
 * @defgroup preprocess preprocess
 * Provides classes to precompute terrain data files.
 * @ingroup proland
 */

/**
 * An abstract raster data map. A map is a 2D array of pixels, whose
 * values can come from anywhere (this depends on how you implement
 * the #getValue method). A map can be read pixel by pixel, or tile
 * by tile. The tiles are cached for better efficiency.
 *
 * @ingroup preprocess
 * @author Eric Bruneton
 */
PROLAND_API class InputMap
{
public:
    /**
     * The width of this map.
     */
    int width;

    /**
     * The height of this map.
     */
    int height;

    /**
     * The number of components per pixel of this map.
     */
    int channels;

    /**
     * The tile size to use when reading this map by tile.
     * The width and height must be multiples of this size.
     */
    int tileSize;

    /**
     * Creates a new map.
     *
     * @param width the width of this map.
     * @param height the height of this map.
     * @param channels the number of components per pixel of this map.
     * @param tileSize the tile size to use when reading this map by tile.
     *      The width and height must be multiples of this size.
     * @param cache how much tiles can be cached at the same time.
     */
    InputMap(int width, int height, int channels, int tileSize, int cache = 20);

    /**
     * Deletes this input map.
     */
    ~InputMap();

    /**
     * Returns the value of the given pixel. You can implement this
     * method any way you want.
     *
     * @param x the x coordinate of the pixel to be read.
     * @param y the y coordinate of the pixel to be read.
     * @return the value of the (x,y) pixel.
     */
    virtual vec4f getValue(int x, int y) = 0;

    /**
     * Returns the values of the pixels of the given tile. The default
     * implementation of this method calls #getValue to read each
     * pixel. If #getValue reads a value from disk, it is strongly
     * advised to override this method for better efficiency.
     *
     * @param tx the tx coordinate of the pixel to be read.
     * @param ty the ty coordinate of the pixel to be read.
     * @return an array of size #tileSize x #tileSize, containing the
     *      values of the pixels in the [ tx * #tileSize , (tx+1) *
     *      #tileSize [ x [ ty * #tileSize , (ty+1) * #tileSize [ region.
     */
    virtual float* getValues(int tx, int ty);

    /**
     * Returns the value of the given pixel. This method uses a cache
     * for better efficiency: it reads the tile containing the given pixel,
     * if it is not already in cache, puts it in cache, and returns the
     * requested pixel from this tile.
     *
     * @param x the x coordinate of the pixel to be read.
     * @param y the y coordinate of the pixel to be read.
     * @return the value of the (x,y) pixel.
     */
    vec4f get(int x, int y);

private:
    struct Tile
    {
        int tx, ty;

        float *data;

        Tile(int tx, int ty, float *data);

        ~Tile();
    };

	typedef map<pair<int, int>, list<Tile*>::iterator> Cache;

    int capacity;

	Cache tileCache;

	list<Tile*> tileCacheOrder;

	float* getTile(int tx, int ty);
};

/**
 * Preprocess an elevation map into a file that can be used with a
 * proland::ResidualProducer.
 * @ingroup preprocess
 * @author Eric Bruneton
 *
 * @param src the map to be preprocessed. Only the x channel is used.
 * @param dstMinTileSize the size of the root tile (without borders). E.g., a size
 *     of 24 corresponds to a full size of 24+5=29.
 * @param dstTileSize the maximum size of the tiles (without borders). E.g., a size
 *     of 192 corresponds to a full size of 192+5=197. dstTileSize must be a
 *     multiple of dstMinTileSize.
 * @param dstMaxLevel the maximum quadtree level to compute. This level is computed
 *     from the first tile having the maximum size. E.g., if dstMinTileSize is 24,
 *     dstTileSize is 192, and maxLevel is 2, this means that the last computed
 *     level will be 5 (level 0 has size 24, level 1 48, level 2 96, level 3 192,
 *     level 4 2*192 and level 5 4*192).
 * @param dstFolder where the precomputed file must be saved.
 * @param tmpFolder where temporary files must be saved.
 * @param residualScale the scale factor to use to quantify the residuals in
 *     short values. Residuals are divided by this factor before beeing quantified.
 *     A small value gives better precision, but can lead to overflows. If you get
 *     overflows during the precomputations (i.e. if the maximum residual, indicated
 *     in the standard ouput is larger than 65535), retry with a larger value.
 */
PROLAND_API void preprocessDem(InputMap *src, int dstMinTileSize, int dstTileSize, int dstMaxLevel,
        const string &dstFolder, const string &tmpFolder, float residualScale);

/**
 * Preprocess a spherical elevation map into six files that can be used with six
 * proland::ResidualProducer to form a planet.
 * @ingroup preprocess
 * @author Eric Bruneton
 *
 * @param src the spherical map to be preprocessed. The x coordinate corresponds to
 *     longitudes, and the y coordinate to latitudes (i.e. using the
 *     equirectangular projection, aka the equidirectional projection,
 *     equidistant cylindrical projection, geographic projection, or plate
 *     carrée projection). Only the x channel is used.
 * @param dstMinTileSize the size of the root tile (without borders). E.g., a size
 *     of 24 corresponds to a full size of 24+5=29.
 * @param dstTileSize the maximum size of the tiles (without borders). E.g., a size
 *     of 192 corresponds to a full size of 192+5=197. dstTileSize must be a
 *     multiple of dstMinTileSize.
 * @param dstMaxLevel the maximum quadtree level to compute. This level is computed
 *     from the first tile having the maximum size. E.g., if dstMinTileSize is 24,
 *     dstTileSize is 192, and maxLevel is 2, this means that the last computed
 *     level will be 5 (level 0 has size 24, level 1 48, level 2 96, level 3 192,
 *     level 4 2*192 and level 5 4*192).
 * @param dstFolder where the precomputed file must be saved.
 * @param tmpFolder where temporary files must be saved.
 * @param residualScale the scale factor to use to quantify the residuals in
 *     short values. Residuals are divided by this factor before beeing quantified.
 *     A small value gives better precision, but can lead to overflows. If you get
 *     overflows during the precomputations (i.e. if the maximum residual, indicated
 *     in the standard ouput is larger than 65535), retry with a larger value.
 */
PROLAND_API void preprocessSphericalDem(InputMap *src, int dstMinTileSize, int dstTileSize, int dstMaxLevel,
        const string &dstFolder, const string &tmpFolder, float residualScale);

/**
 * Preprocess a spherical elevation map into six files that can be used with six
 * proland::OrthoCPUProducer to compute terrain shadows with "Ambient Aperture
 * Lighting", Christopher Oat, Pedro V. Sander, I3D 2007. <b>WARNING</b> the
 * current implementation of this method only works when 'maxLevel' is such that
 * a tile at this level can be "reasonably" considered as "flat". In practice,
 * maxLevel should be at least 10.
 * @ingroup preprocess
 * @author Eric Bruneton
 *
 * @param src the spherical map to be preprocessed. The x coordinate corresponds to
 *     longitudes, and the y coordinate to latitudes (i.e. using the
 *     equirectangular projection, aka the equidirectional projection,
 *     equidistant cylindrical projection, geographic projection, or plate
 *     carrée projection). Only the x channel is used.
 * @param srcFolder where the preprocessed elevation map files are stored (these
 *     files are supposed to have been generated with #preprocessSphericalDem).
 * @param minLevel the minimum quadtree level used to sample visibility around
 *     a point. This level is computed from the first tile having the maximum
 *     size (see proland::preprocessSphericalDem).
 * @param maxLevel the maximum quadtree level use to sample visibility around a
 *     point. This is also the maximum quadtree level of the produced quadtree.
 *     This level is computed from the first tile having the maximum size (see
 *     proland::preprocessSphericalDem).
 * @param samples the number of samples per direction and per level, used to
 *     sample visibility around a point.
 * @param dstFolder where the precomputed file must be saved.
 * @param tmpFolder where temporary files must be saved.
 */
PROLAND_API void preprocessSphericalAperture(const string &srcFolder, int minLevel, int maxLevel, int samples,
        const string &dstFolder, const string &tmpFolder);

/**
 * Preprocess a map into files that can be used with a proland::OrthoCPUProducer
 * (and either a proland::OrthoGPUProducer or a proland::OrthoProducer).
 * @ingroup preprocess
 * @author Eric Bruneton
 *
 * @param src the map to be preprocessed.
 * @param dstTileSize the size of the generated tiles (without borders). E.g., a size
 *     of 192 corresponds to a full size of 192+4=196.
 * @param dstChannels the number of components per pixel in the generated files.
 * @param dstMaxLevel the maximum quadtree level to compute.
 * @param dstFolder where the precomputed file must be saved.
 * @param tmpFolder where temporary files must be saved.
 * @param rgbToLinear an optional transformation to transform the colors of the
 *     input map into a "linear" space where they can be safely interpolated.
 *     For instance, if the input map contains the square root of a reflectance (to
 *     get better precision for low values), this function should be the square
 *     function. A NULL value indicates the identity function.
 * @param linearToRgb an optional transformation, which must be the inverse of
 *     'rgbToLinear'. A NULL value indicates the identity function.
 */
PROLAND_API void preprocessOrtho(InputMap *src, int dstTileSize, int dstChannels, int dstMaxLevel,
        const string &dstFolder, const string &tmpFolder, float (*rgbToLinear)(float) = NULL, float (*linearToRgb)(float) = NULL);

/**
 * Preprocess a spherical map into files that can be used with a
 * proland::OrthoCPUProducer (and either a proland::OrthoGPUProducer or a
 * proland::OrthoProducer).
 * @ingroup preprocess
 * @author Eric Bruneton
 *
 * @param src the spherical map to be preprocessed.
 * @param dstTileSize the size of the generated tiles (without borders). E.g., a size
 *     of 192 corresponds to a full size of 192+4=196.
 * @param dstChannels the number of components per pixel in the generated files.
 * @param dstMaxLevel the maximum quadtree level to compute.
 * @param dstFolder where the precomputed file must be saved.
 * @param tmpFolder where temporary files must be saved.
 * @param rgbToLinear an optional transformation to transform the colors of the
 *     input map into a "linear" space where they can be safely interpolated.
 *     For instance, if the input map contains the square root of a reflectance (to
 *     get better precision for low values), this function should be the square
 *     function. A NULL value indicates the identity function.
 * @param linearToRgb an optional transformation, which must be the inverse of
 *     'rgbToLinear'. A NULL value indicates the identity function.
 */
PROLAND_API void preprocessSphericalOrtho(InputMap *src, int dstTileSize, int dstChannels, int dstMaxLevel,
        const string &dstFolder, const string &tmpFolder, float (*rgbToLinear)(float) = NULL, float (*linearToRgb)(float) = NULL);

}

#endif
