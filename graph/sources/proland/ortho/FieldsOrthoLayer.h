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

#ifndef _PROLAND_FIELDSORTHOLAYER_H_
#define _PROLAND_FIELDSORTHOLAYER_H_

#include "proland/graph/producer/GraphLayer.h"

namespace proland
{

/**
 * An OrthoGPUProducer layer to draw fields.
 * @ingroup ortho
 * @author Antoine Begault, Guillaume Piolat
 */
PROLAND_API class FieldsOrthoLayer : public GraphLayer
{
public:
    /**
     * Creates a new FieldsOrthoLayer.
     *
     * @param graphProducer the GraphProducer that produces the graphs to
     *      be drawn by this layer.
     * @param layerProgram the Program to be used to draw the graphs.
     * @param fillProgram the GLSL Program to be used to fill areas in this Layer.
     * @param displayLevel the quadtree level at which the display of
     *      this layer must start.
     * @param quality enable or not the quality mode (better display).
     */
    FieldsOrthoLayer(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        ptr<Program> fillProgram, int displayLevel = 0, bool quality = true);

    /**
     * Deletes this FieldsOrthoLayer.
     */
    virtual ~FieldsOrthoLayer();

    virtual void setTileSize(int tileSize, int tileBorder, float rootQuadSize);

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data);

protected:
    /**
     * Program used to fill areas.
     */
    ptr<Program> fill;

    /**
     * Creates an uninitialized FieldsOrthoLayer.
     */
    FieldsOrthoLayer();

    /**
     * Initializes this FieldsOrthoLayer. See #FieldsOrthoLayer.
     */
    void init(ptr<GraphProducer> graphProducer, ptr<Program> layerProgram,
        ptr<Program> fillProgram, int displayLevel = 0, bool quality = true);

    virtual void swap(ptr<FieldsOrthoLayer> p);

private:
    /**
     * Fields colors.
     */
    static vec4f COLOR[10];

    /**
     * Fields dColor.
     */
    static mat3f DCOLOR[10];

    /**
     * Fields stripe size.
     */
    static vec3f STRIPES[10];

    /**
     * The mesh used for drawing curves.
     */
    ptr< Mesh<vec2f, unsigned int> > mesh;

    /**
     * The tesselator used for drawing areas.
     */
    ptr<Tesselator> tess;

    /**
     * Returns the color of a given Area, depending on its type.
     *
     * @param field the Area to color.
     * @param[out] dcolor the associated dcolor.
     * @param[out] stripeSize the associated stripeSize.
     * @param[out] stripeDir the associated stripeDir.
     */
    vec4f* getColor(AreaPtr field, mat3f **dcolor, vec3f **stripeSize, vec2f &stripeDir);

    /**
     * Matrix used to compute border color in the shader.
     */
    ptr<Uniform3f> fillOffsetU;

    ptr<Uniform1f> depthU;

    ptr<Uniform4f> fillColorU;

    ptr<Uniform3f> stripeSizeU;

    ptr<Uniform4f> stripeDirU;

    ptr<Uniform4f> colorU;

    ptr<Uniform3f> tileOffsetU;
};

}

#endif
