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

#undef __STRICT_ANSI__

#include "proland/ortho/OrthoCPUProducer.h"

#include <fstream>
#include <sstream>

#include "tiffio.h"

#include "ork/core/Logger.h"
#include "ork/resource/ResourceTemplate.h"
#include "proland/producer/CPUTileStorage.h"
#include "proland/util/mfs.h"

#include <pthread.h>

using namespace std;
using namespace ork;

namespace proland
{

//#define SINGLE_FILE

#define MAX_TILE_SIZE 512

void *OrthoCPUProducer::key = NULL;

void orthoCPUDelete(void* data)
{
    delete[] (unsigned char*) data;
}

OrthoCPUProducer::OrthoCPUProducer(ptr<TileCache> cache, const char *name) :
    TileProducer("OrthoCPUProducer", "CreateOrthoCPUTile")
{
    init(cache, name);
}

OrthoCPUProducer::OrthoCPUProducer() : TileProducer("OrthoCPUProducer", "CreateOrthoCPUTile")
{
}

void OrthoCPUProducer::init(ptr<TileCache> cache, const char *name)
{
    TileProducer::init(cache, false);
    this->name = name;
    if (strlen(name) == 0) {
        maxLevel = 1;
        tileSize = 0;
        dxt = 0;
        border = 2;
    } else {
        fopen(&tileFile, name, "rb");
        if (tileFile == NULL) {
            if (Logger::ERROR_LOGGER != NULL) {
                Logger::ERROR_LOGGER->log("ORTHO", "Cannot open file '" + string(name) + "'");
            }
            maxLevel = -1;
        } else {
            int root;
            int tx;
            int ty;
            int flags;
            fread(&maxLevel, sizeof(int), 1, tileFile);
            fread(&tileSize, sizeof(int), 1, tileFile);
            fread(&channels, sizeof(int), 1, tileFile);
            fread(&root, sizeof(int), 1, tileFile);
            fread(&tx, sizeof(int), 1, tileFile);
            fread(&ty, sizeof(int), 1, tileFile);
            fread(&flags, sizeof(int), 1, tileFile);
            dxt = (flags & 1) != 0;
            border = (flags & 2) != 0 ? 0 : 2;
        }

        int ntiles = ((1 << (maxLevel * 2 + 2)) - 1) / 3;
        header = 7 * sizeof(int) + 2 * ntiles * sizeof(long long);
        offsets = new long long[2 * ntiles];
        if (tileFile != NULL) {
            fread(offsets, sizeof(long long) * ntiles * 2, 1, tileFile);
    #ifndef SINGLE_FILE
            fclose(tileFile);
            tileFile = NULL;
    #endif
        }

        if (key == NULL) {
            key = new pthread_key_t;
            pthread_key_create((pthread_key_t*) key, orthoCPUDelete);
        }

        assert(tileSize + 2*border < MAX_TILE_SIZE);

    #ifdef SINGLE_FILE
        mutex = new pthread_mutex_t;
        pthread_mutex_init((pthread_mutex_t*) mutex, NULL);
    #endif
    }
}

OrthoCPUProducer::~OrthoCPUProducer()
{
#ifdef SINGLE_FILE
    fclose(tileFile);
    pthread_mutex_destroy((pthread_mutex_t*) mutex);
    delete (pthread_mutex_t*) mutex;
#endif
    delete[] offsets;
}

int OrthoCPUProducer::getBorder()
{
    return border;
}

bool OrthoCPUProducer::hasTile(int level, int tx, int ty)
{
    return level <= maxLevel;
}

bool OrthoCPUProducer::isCompressed()
{
    return dxt;
}

bool OrthoCPUProducer::doCreateTile(int level, int tx, int ty, TileStorage::Slot *data)
{
    if (Logger::DEBUG_LOGGER != NULL) {
        ostringstream oss;
        oss << "CPU tile " << getId() << " " << level << " " << tx << " " << ty;
        Logger::DEBUG_LOGGER->log("ORTHO", oss.str());
    }

    CPUTileStorage<unsigned char>::CPUSlot *cpuData = dynamic_cast<CPUTileStorage<unsigned char>::CPUSlot*>(data);
    assert(cpuData != NULL);

    int tileid = getTileId(level, tx, ty);

    if ((int) name.size() == 0) {
        for (int i = 0; i < cpuData->size; i++) {
            cpuData->data[i] = 0;
        }
    } else {
        assert(dynamic_cast<CPUTileStorage<unsigned char>*>(cpuData->getOwner())->getChannels() == channels);
        assert(cpuData->getOwner()->getTileSize() == tileSize + 2*border);
        assert(level <= maxLevel);

        unsigned char *compressedData = (unsigned char*) pthread_getspecific(*((pthread_key_t*) key));
        if (compressedData == NULL) {
            compressedData = new unsigned char[MAX_TILE_SIZE * MAX_TILE_SIZE * 4 * 2];
            pthread_setspecific(*((pthread_key_t*) key), compressedData);
        }

        int fsize = (int) (offsets[2 * tileid + 1] - offsets[2 * tileid]);
        assert(fsize < (tileSize + 2*border) * (tileSize + 2*border) * channels * 2);

        if (dxt) {
    #ifdef SINGLE_FILE
            pthread_mutex_lock((pthread_mutex_t*) mutex);
            fseek64(tileFile, header + offsets[2 * tileid], SEEK_SET);
            fread(cpuData->data, fsize, 1, tileFile);
            pthread_mutex_unlock((pthread_mutex_t*) mutex);
    #else
            FILE *file;
            fopen(&file, name.c_str(), "rb");
            fseek64(file, header + offsets[2 * tileid], SEEK_SET);
            fread(cpuData->data, fsize, 1, file);
            fclose(file);
            /*ifstream fs(name.c_str(), ios::binary);
            fs.seekg(header + offsets[2 * tileid], ios::beg);
            fs.read((char*) cpuData->data, fsize);
            fs.close();*/
    #endif
            cpuData->size = fsize;
        } else {
            unsigned char* srcData = compressedData;
    #ifdef SINGLE_FILE
            pthread_mutex_lock((pthread_mutex_t*) mutex);
            fseek64(tileFile, header + offsets[2 * tileid], SEEK_SET);
            fread(compressedData, fsize, 1, tileFile);
            pthread_mutex_unlock((pthread_mutex_t*) mutex);
    #else
            FILE *file;
            fopen(&file, name.c_str(), "rb");
            fseek64(file, header + offsets[2 * tileid], SEEK_SET);
            fread(compressedData, fsize, 1, file);
            fclose(file);
            /*ifstream fs(name.c_str(), ios::binary);
            fs.seekg(header + offsets[2 * tileid], ios::beg);
            fs.read((char*) src, fsize);
            fs.close();*/
    #endif

            mfs_file fd;
            mfs_open(srcData, fsize, (char *)"r", &fd);
            TIFF* tf = TIFFClientOpen("name", "r", &fd,
                (TIFFReadWriteProc) mfs_read, (TIFFReadWriteProc) mfs_write, (TIFFSeekProc) mfs_lseek,
                (TIFFCloseProc) mfs_close, (TIFFSizeProc) mfs_size, (TIFFMapFileProc) mfs_map,
                (TIFFUnmapFileProc) mfs_unmap);
            TIFFReadEncodedStrip(tf, 0, cpuData->data, (tsize_t) -1);
            TIFFClose(tf);
        }
    }

    return true;
}

void OrthoCPUProducer::swap(ptr<OrthoCPUProducer> p)
{
    TileProducer::swap(p);
    std::swap(name, p->name);
    std::swap(channels, p->channels);
    std::swap(tileSize, p->tileSize);
    std::swap(maxLevel, p->maxLevel);
    std::swap(dxt, p->dxt);
    std::swap(offsets, p->offsets);
    std::swap(mutex, p->mutex);
    std::swap(tileFile, p->tileFile);
}

int OrthoCPUProducer::getTileId(int level, int tx, int ty)
{
    return tx + ty * (1 << level) + ((1 << (2 * level)) - 1) / 3;
}

class OrthoCPUProducerResource : public ResourceTemplate<2, OrthoCPUProducer>
{
public:
    OrthoCPUProducerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<2, OrthoCPUProducer>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        ptr<TileCache> cache;
        string file;
        checkParameters(desc, e, "name,cache,file,");
        cache = manager->loadResource(getParameter(desc, e, "cache")).cast<TileCache>();
        if (e->Attribute("file") != NULL) {
            file = getParameter(desc, e, "file");
            file = manager->getLoader()->findResource(file);
        }
        init(cache, file.c_str());
    }
};

extern const char orthoCpuProducer[] = "orthoCpuProducer";

static ResourceFactory::Type<orthoCpuProducer, OrthoCPUProducerResource> OrthoCPUProducerType;

}
