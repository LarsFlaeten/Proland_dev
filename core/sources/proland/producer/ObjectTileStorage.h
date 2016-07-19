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

#ifndef _PROLAND_OBJECT_TILE_STORAGE_H_
#define _PROLAND_OBJECT_TILE_STORAGE_H_

#include "proland/producer/TileStorage.h"

namespace proland
{

/**
 * A TileStorage that stores Object on CPU.
 * @ingroup producer
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class ObjectTileStorage : public TileStorage
{
public:
    /**
     * A tile managed by an ObjectTileStorage.
     */
    class ObjectSlot : public Slot
    {
    public:
        /**
         * The tile data.
         */
        ptr<Object> data;

        /**
         * Creates a new ObjectSlot. This constructor does NOT allocate any
         * object.
         *
         * @param owner the TileStorage that manages this tile.
         */
        ObjectSlot(TileStorage *owner);

        /**
         * Deletes this ObjectSlot. This deletes the reference to #data.
         */
        virtual ~ObjectSlot();
    };

    /**
     * Creates a new ObjectTileStorage.
     *
     * @param capacity the number of tiles managed by this tile storage.
     */
    ObjectTileStorage(int capacity);

    /**
     * Deletes this ObjectTileStorage.
     */
    virtual ~ObjectTileStorage();

protected:
    /**
     * Creates an uninitialized ObjectTileStorage.
     */
    ObjectTileStorage();

    /**
     * Initializes this ObjectTileStorage.
     *
     * @param capacity the number of tiles managed by this tile storage.
     */
    void init(int capacity);

    void swap(ptr<ObjectTileStorage> t);
};

}

#endif
