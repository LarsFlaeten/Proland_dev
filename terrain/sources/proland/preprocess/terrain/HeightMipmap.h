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

#ifndef _PROLAND_HEIGHT_MIPMAP_
#define _PROLAND_HEIGHT_MIPMAP_

#include <string>
#include <cmath>

#include "tiffio.h"

#include "proland/preprocess/terrain/AbstractTileCache.h"

namespace proland
{

class HeightMipmap : public AbstractTileCache
{
public:
    class HeightFunction
    {
    public:
        virtual float getHeight(int x, int y) = 0;
    };

    HeightMipmap *left;
    HeightMipmap *right;
    HeightMipmap *bottom;
    HeightMipmap *top;
    int leftr;
    int rightr;
    int bottomr;
    int topr;

    HeightMipmap(HeightFunction *height, int topLevelSize, int baseLevelSize, int tileSize, float scale, const string &cache);

    ~HeightMipmap();

    static void setCube(HeightMipmap *hm1, HeightMipmap *hm2, HeightMipmap *hm3, HeightMipmap *hm4, HeightMipmap *hm5, HeightMipmap *hm6);

    void compute1();

    bool compute2();

    void generate(int rootLevel, int rootTx, int rootTy, float scale, const string &file);

    virtual float getTileHeight(int x, int y);

    virtual void reset(int width, int height, int tileSize);

protected:
    virtual unsigned char* readTile(int tx, int ty);

    virtual void getTile(int level, int tx, int ty, float *tile);

private:
    HeightFunction *height;

    int topLevelSize;

    int baseLevelSize;

    int tileSize;

    float scale;

    string cache;

    int minLevel;

    int maxLevel;

    int currentMipLevel;

    unsigned char *tile;

    int currentLevel;

    int constantTile;

    void buildBaseLevelTiles();

    void buildBaseLevelTile(int tx, int ty, TIFF *f);

    void buildMipmapLevel(int level);

    void buildResiduals(int level);

    void getApproxTile(int level, int tx, int ty, float *tile);

    void saveApproxTile(int level, int tx, int ty, float *tile);

    void computeResidual(float *parentTile, float *tile, int level, int tx, int ty, float *residual, float &maxR, float &meanR);

    void encodeResidual(int level, float *residual, unsigned char *encoded);

    void computeApproxTile(float *parentTile, float *residual, int level, int tx, int ty, float *tile, float &maxErr);

    void produceTile(int level, int tx, int ty, unsigned int *offset, unsigned int *offsets, FILE *f);

    void produceTilesLebeguesOrder(int l, int level, int tx, int ty, unsigned int *offset, unsigned int *offsets, FILE *f);
};

}

#endif
