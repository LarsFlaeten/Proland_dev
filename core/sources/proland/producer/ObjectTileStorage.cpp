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

#include "proland/producer/ObjectTileStorage.h"

#include "ork/resource/ResourceTemplate.h"

using namespace std;
using namespace ork;

namespace proland
{

ObjectTileStorage::ObjectSlot::ObjectSlot(TileStorage *owner) : Slot(owner)
{
    data = NULL;
}

ObjectTileStorage::ObjectSlot::~ObjectSlot()
{
    data = NULL;
}

ObjectTileStorage::ObjectTileStorage(int capacity) : TileStorage()
{
    init(capacity);
}

ObjectTileStorage::ObjectTileStorage() : TileStorage()
{
}

void ObjectTileStorage::init(int capacity)
{
    TileStorage::init(0, capacity);
    for (int i = 0; i < capacity; ++i) {
        freeSlots.push_back(new ObjectSlot(this));
    }
}

ObjectTileStorage::~ObjectTileStorage()
{
}

void ObjectTileStorage::swap(ptr<ObjectTileStorage> t)
{
}

class ObjectTileStorageResource : public ResourceTemplate<0, ObjectTileStorage>
{

public:
    ObjectTileStorageResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<0, ObjectTileStorage> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        int capacity;
        Resource::checkParameters(desc, e, "name,capacity,");
        Resource::getIntParameter(desc, e, "capacity", &capacity);
        ObjectTileStorage::init(capacity);
    }
};

extern const char objectTileStorage[] = "objectTileStorage";

static ResourceFactory::Type<objectTileStorage, ObjectTileStorageResource> ObjectTileStorageType;

}
