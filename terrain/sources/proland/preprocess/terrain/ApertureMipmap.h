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

#ifndef _PROLAND_APERTURE_MIPMAP_
#define _PROLAND_APERTURE_MIPMAP_

#include <string>
#include <list>
#include <map>

#include "ork/math/vec3.h"
#include "proland/preprocess/terrain/Util.h"

using namespace std;
using namespace ork;

namespace proland
{

class FloatTileCache {
public:
    struct FloatTile
    {
        int level, tx, ty;

        float *data;

        FloatTile(int level, int tx, int ty, float *data = NULL) :
            level(level), tx(tx), ty(ty), data(data)
        {
        }

        ~FloatTile()
        {
            if (data != NULL) {
                delete[] data;
            }
        }

        int key()
        {
            return tx + ty * (1 << level) + ((1 << (2 * level)) - 1) / 3;
        }
    };

	FloatTileCache(int capacity = 20) :
        capacity(capacity)
	{
	}

	virtual ~FloatTileCache()
	{
        list<FloatTile*>::iterator i = tileCacheOrder.begin();
        while (i != tileCacheOrder.end()) {
            delete *i;
            ++i;
        }
    }


	float* getTile(int level, int tx, int ty);

protected:
	virtual float* readTile(int level, int tx, int ty) = 0;

private:
    int capacity;

	typedef map<int, list<FloatTile*>::iterator> Cache;

	Cache tileCache;

	list<FloatTile*> tileCacheOrder;
};

class DemTileCache : public FloatTileCache
{
public:
    FILE *tileFile;

    int tileSize;

    int rootLevel;

    int deltaLevel;

    int rootTx;

    int rootTy;

    int minLevel;

    int maxLevel;

    int analyzeLevel;

    float scale;

    unsigned int header;

    unsigned int* offsets;

    unsigned char *compressedData;

    unsigned char *uncompressedData;

	DemTileCache(const string &name, int capacity = 20);

	~DemTileCache();

    virtual float* readTile(int level, int tx, int ty);

	int getTileId(int level, int tx, int ty);

	int getTileSize(int level);
};

class ElevationTileCache : public FloatTileCache
{
public:
    DemTileCache *r;

	ElevationTileCache(DemTileCache *residuals, int capacity = 20);

	~ElevationTileCache();

    virtual float* readTile(int level, int tx, int ty);

    float getHeight(int level, int x, int y);

    float getHeight(int level, int x, int y, float dx, float dy);
};

class PlanetElevationTileCache
{
public:
    ElevationTileCache **faces;

    int level;

    int m;

    PlanetElevationTileCache(ElevationTileCache **faces, int level);

    float getHeight(double sx, double sy, double sz);
};

class ApertureMipmap
{
public:
    typedef void (*projFun)(double x, double y, double w, double &sx, double &sy, double &sz);

    ApertureMipmap(PlanetElevationTileCache *z, projFun proj, double R, int buildLevel, int minLevel, int samples);

    ~ApertureMipmap();

    void build(const string &temp);

    void generate(const string &cache, const string &file);

private:
	PlanetElevationTileCache *z;

	projFun proj;

    double R;

	int buildLevel;

	int minLevel;

    int samples;

    void compute(int level, int x, int y, float dx, float dy, float z0, float *len, int flen, float *slopes);

    float ambient(float *phi, float *cphi, float *sphi, float *slopes, vec3f &norm);
};

}

#endif
