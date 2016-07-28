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

#ifndef _PROLAND_EMPTYORTHOLAYER_H_
#define _PROLAND_EMPTYORTHOLAYER_H_

#include "ork/math/vec4.h"
#include "proland/producer/TileLayer.h"

using namespace ork;

namespace proland
{

/**
 * An OrthoGPUProducer layer that simply fills tiles with a constant color.
 * This layer is useful when the OrthoGPUProducer is used without an
 * OrthoCPUProducer. It can provide a background color for other layers.
 * @ingroup ortho
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class EmptyOrthoLayer : public TileLayer
{
public:
    /**
     * Creates a new EmptyOrthoLayer.
     *
     * @param color the color to be used to fill the produced tiles.
     */
    EmptyOrthoLayer(vec4f color);

    /**
     * Deletes this EmptyOrthoLayer.
     */
    virtual ~EmptyOrthoLayer();

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

protected:
    /**
     * Creates an uninitialized EmptyOrthoLayer.
     */
    EmptyOrthoLayer();

    /**
     * Initializes this EmptyOrthoLayer.
     *
     * @param color the color to be used to fill the produced tiles.
     */
    void init(vec4f color);

    virtual void swap(ptr<EmptyOrthoLayer> p);

private:
    /**
     * The color to be used to fill the produced tiles.
     */
    vec4f color;
};

}

#endif
