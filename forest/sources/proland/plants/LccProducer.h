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

#ifndef _PROLAND_LCC_PRODUCER_H_
#define _PROLAND_LCC_PRODUCER_H_

#include "proland/producer/TileProducer.h"
#include "proland/terrain/Deformation.h"
#include "proland/plants/Plants.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * TODO.
 * @ingroup plants
 * @author Eric Bruneton
 */
PROLAND_API class LccProducer : public TileProducer
{
public:
    LccProducer(ptr<TileProducer> delegate, ptr<Plants> plants, ptr<Texture2D> lccTexture,
        ptr<Program> copy, ptr<Program> dots, int maxLevel, bool deform);

    virtual ~LccProducer();

    ptr<TileProducer> getDelegate();

    virtual void setRootQuadSize(float size);

    virtual int getBorder();

    virtual bool hasTile(int level, int tx, int ty);

    virtual TileCache::Tile* findTile(int level, int tx, int ty, bool includeCache = false, bool done = false);

    virtual TileCache::Tile* getTile(int level, int tx, int ty, unsigned int deadline);

    vec4f getGpuTileCoords(int level, int tx, int ty, TileCache::Tile **tile);

    virtual bool prefetchTile(int level, int tx, int ty);

    virtual void putTile(TileCache::Tile *t);

    virtual void invalidateTiles();

    virtual void invalidateTile(int level, int tx, int ty);

    virtual void update(ptr<SceneManager> scene);

    virtual void getReferencedProducers(vector< ptr<TileProducer> > &producers) const;

protected:
    ptr<FrameBuffer> frameBuffer;

    ptr<Texture2D> lccTexture;

    ptr<Program> copy;

    ptr<Program> dots;

    LccProducer();

    void init(ptr<TileProducer> delegate, ptr<Plants> plants, ptr<Texture2D> lccTexture,
        ptr<Program> copy, ptr<Program> dots, int maxLevel, bool deform);

    virtual void *getContext() const;

    virtual ptr<Task> startCreateTile(int level, int tx, int ty, unsigned int deadline, ptr<Task> task, ptr<TaskGraph> owner);

    virtual void beginCreateTile();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

    virtual void endCreateTile();

    virtual void stopCreateTile(int level, int tx, int ty);

private:
    ptr<TileProducer> delegate;

    ptr<Plants> plants;

    int maxLevel;

    bool deform;

    float lastTreeDensity;

    float lastFov;

    ptr<Uniform1f> densityU;

    ptr<UniformSampler> sourceSamplerU;

    ptr<Uniform4f> sourceOSLU;

    ptr<Uniform4f> tileOffsetU;

    ptr<UniformMatrix2f> tileDeformU;

    ptr<Uniform4f> tileClipU;

    ptr<UniformSampler> densitySamplerU;

    ptr<Uniform4f> densityOSLU;

    ptr<Deformation> deformation;

    static static_ptr<FrameBuffer> old;
};

}

#endif
