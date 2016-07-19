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

#include "proland/producer/TileLayer.h"

using namespace std;

namespace proland
{

TileLayer::TileLayer(const char *type, bool deform) :
    Object(type), tileSize(0), tileBorder(0), rootQuadSize(0.0f), deform(deform), enabled(true)
{
}

TileLayer::~TileLayer()
{
}

void TileLayer::init(bool deform)
{
    this->deform = deform;
    this->enabled = true;
}

ptr<TileCache> TileLayer::getCache()
{
    return cache;
}

int TileLayer::getProducerId()
{
    return producerId;
}

int TileLayer::getTileSize()
{
    return tileSize;
}

int TileLayer::getTileBorder()
{
    return tileBorder;
}

float TileLayer::getRootQuadSize()
{
    return rootQuadSize;
}

vec3d TileLayer::getTileCoords(int level, int tx, int ty)
{
    double ox = rootQuadSize * (double(tx) / (1 << level) - 0.5f);
    double oy = rootQuadSize * (double(ty) / (1 << level) - 0.5f);
    double l = rootQuadSize / (1 << level);
    return vec3d(ox, oy, l);
}

bool TileLayer::isDeformed()
{
    return deform;
}

void TileLayer::getDeformParameters(vec3d tileCoords, vec2d &nx, vec2d &ny, vec2d &lx, vec2d &ly)
{
    if (isDeformed()) {
        double x = tileCoords.x + tileCoords.z / 2.0f;
        double y = tileCoords.y + tileCoords.z / 2.0f;
        double R = getRootQuadSize() / 2.0f;
        double yR = y*y + R*R;
        double xyR = x*x + yR;
        double d = R * sqrt(xyR);
        double e = R / (sqrt(yR) * xyR);
        nx = vec2d(x*y / d, yR / d);
        ny = vec2d(-((x*x + R*R) / d), -(x*y / d));
        lx = vec2d(e * yR, 0.0);
        ly = vec2d(-(e * x * y), e * d);
    }
}

bool TileLayer::isEnabled()
{
    return enabled;
}

void TileLayer::setIsEnabled(bool enabled)
{
    this->enabled = enabled;
    invalidateTiles();
}

void TileLayer::setCache(ptr<TileCache> cache, int producerId)
{
    this->cache = cache;
    this->producerId = producerId;
}

void TileLayer::getReferencedProducers(vector< ptr<TileProducer> > &producers) const
{
}

void TileLayer::setTileSize(int tileSize, int tileBorder, float rootQuadSize)
{
    this->tileSize = tileSize;
    this->tileBorder = tileBorder;
    this->rootQuadSize = rootQuadSize;
}

void TileLayer::useTile(int level, int tx, int ty, unsigned int deadline)
{
}

void TileLayer::unuseTile(int level, int tx, int ty)
{
}

void TileLayer::prefetchTile(int level, int tx, int ty)
{
}

void TileLayer::startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner)
{
}

void TileLayer::beginCreateTile()
{
}

void TileLayer::endCreateTile()
{
}

void TileLayer::stopCreateTile(int level, int tx, int ty)
{
}

void TileLayer::swap(ptr<TileLayer> p)
{
    cache->invalidateTiles(producerId);
    p->cache->invalidateTiles(p->producerId);
    std::swap(cache, p->cache);
    std::swap(producerId, p->producerId);
    std::swap(tileSize, p->tileSize);
    std::swap(tileBorder, p->tileBorder);
    std::swap(rootQuadSize, p->rootQuadSize);
    std::swap(deform, p->deform);
}

void TileLayer::invalidateTiles()
{
    getCache()->invalidateTiles(getProducerId());
}

}
