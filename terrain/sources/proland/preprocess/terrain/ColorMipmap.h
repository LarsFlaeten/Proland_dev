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

#ifndef _PROLAND_COLOR_MIPMAP_
#define _PROLAND_COLOR_MIPMAP_

#include <string>
#include <cmath>

#include "tiffio.h"

#include "proland/preprocess/terrain/AbstractTileCache.h"

using namespace std;

namespace proland
{

class ColorMipmap : public AbstractTileCache
{
public:
    class ColorFunction
    {
    public:
        virtual vec4f getColor(int x, int y) = 0;
    };

    ColorMipmap *left;
    ColorMipmap *right;
    ColorMipmap *bottom;
    ColorMipmap *top;
    int leftr;
    int rightr;
    int bottomr;
    int topr;

    ColorMipmap(ColorFunction *colorf, int baseLevelSize, int tileSize, int border, int channels, float (*rgbToLinear)(float), float (*linearToRgb)(float), const string &cache);

    ~ColorMipmap();

    static void setCube(ColorMipmap *hm1, ColorMipmap *hm2, ColorMipmap *hm3, ColorMipmap *hm4, ColorMipmap *hm5, ColorMipmap *hm6);

    void compute();

    void computeMipmap();

    void generate(int rootLevel, int rootTx, int rootTy, bool dxt, bool jpg, int jpg_quality, const string &file);

    void generateResiduals(bool jpg, int jpg_quality, const string &in, const string &out);

    void reorderResiduals(const string &in, const string &out);

protected:
    virtual unsigned char* readTile(int tx, int ty);

    ColorFunction *colorf;

    int baseLevelSize;

    int tileSize;

    int tileWidth;

    int border;

    int channels;

    float (*r2l)(float);

    float (*l2r)(float);

    string cache;

    int maxLevel;

    unsigned char *tile;

    unsigned char *rgbaTile;

    unsigned char *dxtTile;

    int currentLevel;

    bool dxt;

    bool jpg;

    int jpg_quality;

    FILE *in;

    int iheader;

    long long *ioffsets;

    bool oJpg;

    int oJpg_quality;

    map<int, int> constantTileIds;

    unsigned char *compressedInputTile;

    unsigned char *inputTile;

    void buildBaseLevelTiles();

    void buildBaseLevelTile(int tx, int ty, TIFF *f);

    virtual void buildMipmapLevel(int level);

    void produceRawTile(int level, int tx, int ty);

    virtual void produceTile(int level, int tx, int ty);

    void produceTile(int level, int tx, int ty, long long *offset, long long *offsets, FILE *f);

    void produceTilesLebeguesOrder(int l, int level, int tx, int ty, long long *offset, long long *offsets, FILE *f);

    void readInputTile(int level, int tx, int ty);

    void convertTile(int level, int tx, int ty, unsigned char *parent);

    void convertTile(int level, int tx, int ty, unsigned char *parent, long long *offset, long long *offsets, FILE *f);

    void convertTiles(int level, int tx, int ty, unsigned char *parent, long long *outOffset, long long *outOffsets, FILE *f);

    void reorderTilesLebeguesOrder(int l, int level, int tx, int ty, long long *outOffset, long long *outOffsets, FILE *f);
};

}

#endif
