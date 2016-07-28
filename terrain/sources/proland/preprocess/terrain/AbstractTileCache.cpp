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

#include "proland/preprocess/terrain/AbstractTileCache.h"

namespace proland
{

unsigned char* AbstractTileCache::getTile(int tx, int ty)
{
    int key = Tile(tx, ty).key(width / tileSize + 1);
    Cache::iterator i = tileCache.find(key);
    if (i == tileCache.end()) {
        unsigned char *data = readTile(tx, ty);

        list<Tile*>::iterator li;

        if ((int) tileCache.size() == capacity) {
            // evict least recently used tile if cache is full
            li = tileCacheOrder.begin();
            Tile *t = *li;
            tileCache.erase(t->key(width / tileSize + 1));
            tileCacheOrder.erase(li);
            delete t;
        }

        // create tile, put it at the end of tileCacheOrder, and update the map
        Tile *t = new Tile(tx, ty, data);
        li = tileCacheOrder.insert(tileCacheOrder.end(), t);
        tileCache[key] = li;
        assert(tileCache.find(key) != tileCache.end());
        return data;
    } else {
        list<Tile*>::iterator li = i->second;
        list<Tile*>::iterator lj = li;
        Tile *t = *li;
        assert(t->tx == tx && t->ty == ty);
        // put t at the end of tileCacheOrder, and update the map accordingly
        lj++;
        if (lj != tileCacheOrder.end()) {
            tileCacheOrder.erase(li);
            li = tileCacheOrder.insert(tileCacheOrder.end(), t);
            tileCache[key] = li;
        }
        return t->data;
    }
}

float AbstractTileCache::getTileHeight(int x, int y)
{
    x = max(min(x, width), 0);
    y = max(min(y, height), 0);
    int tx = min(x, width - 1) / tileSize;
    int ty = min(y, height - 1) / tileSize;
    x = (x == width ? tileSize : x % tileSize) + 2;
    y = (y == height ? tileSize : y % tileSize) + 2;
    unsigned char* data = getTile(tx, ty);
    if (channels == 1) {
        int off = (x + y * (tileSize + 5));
        return float(data[off]);
    } else {
        int off = (x + y * (tileSize + 5)) * 2;
        int hl = data[off++];
        int hh = data[off];
        short sh = short(hh) << 8 | short(hl);
        return float(sh);
    }
}

vec4f AbstractTileCache::getTileColor(int x, int y)
{
    x = max(min(x, width - 1), 0);
    y = max(min(y, height - 1), 0);
    int tx = x / tileSize;
    int ty = y / tileSize;
    x = x % tileSize + 2;
    y = y % tileSize + 2;
    unsigned char* data = getTile(tx, ty);
    int off = (x + y * (tileSize + 4)) * channels;
    vec4f c;
    c.x = data[off];
    if (channels > 1) {
        c.y = data[off + 1];
    }
    if (channels > 2) {
        c.z = data[off + 2];
    }
    if (channels > 3) {
        c.w = data[off + 3];
    }
    return c;
}

void AbstractTileCache::reset(int width, int height, int tileSize)
{
    list<Tile*>::iterator i = tileCacheOrder.begin();
    while (i != tileCacheOrder.end()) {
        delete *i;
        ++i;
    }
    tileCache.clear();
    tileCacheOrder.clear();
    this->width = width;
    this->height = height;
    this->tileSize = tileSize;
}

}
