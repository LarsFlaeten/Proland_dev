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

#include "proland/graph/producer/Tesselator.h"

#include <GL/glu.h>

#ifdef _WIN32
#define CALLBACK __stdcall
typedef void (CALLBACK *TessCallBack)();
#else
#define CALLBACK
typedef void (*TessCallBack)();
#endif

#ifdef _LP64
typedef long _int;
#else
typedef int _int;
#endif

extern "C"
{

int mode;
int prevprev;
int previous;

void CALLBACK errorCallback(GLenum code, ork::Mesh<vec2f, unsigned int> *m)
{
    assert(false);
}

void CALLBACK beginCallback(GLenum which, ork::Mesh<vec2f, unsigned int> *m)
{
    mode = which;
    prevprev = -1;
    previous = -1;
}

void CALLBACK vertexCallback(GLvoid *vertex, ork::Mesh<vec2f, unsigned int> *m)
{
    switch (mode) {
        case GL_TRIANGLES:
            m->addIndice((_int) vertex);
            break;
        case GL_TRIANGLE_STRIP:
            if (prevprev == -1) {
                prevprev = (_int) vertex;
            } else if (previous == -1) {
                previous = (_int) vertex;
            } else {
                m->addIndice(prevprev);
                m->addIndice(previous);
                m->addIndice((_int) vertex);
                prevprev = previous;
                previous = (_int) vertex;
            }
            break;
        case GL_TRIANGLE_FAN:
            if (prevprev == -1) {
                prevprev = (_int) vertex;
            } else if (previous == -1) {
                previous = (_int) vertex;
            } else {
                m->addIndice(prevprev);
                m->addIndice(previous);
                m->addIndice((_int) vertex);
                previous = (_int) vertex;
            }
            break;
    }
}

void CALLBACK combineCallback(GLdouble coords[3], GLvoid *vertex_data[4], GLfloat weight[4], GLvoid **outData, ork::Mesh<vec2f, unsigned int> *m)
{
    int v = m->getVertexCount();
    m->addVertex(vec2f((float) coords[0], (float) coords[1]));
    *outData = (GLvoid*) v;
}

}

namespace proland
{

Tesselator::Tesselator() : Object("Tesselator")
{
    tess = gluNewTess();
    gluTessCallback((GLUtesselator*) tess, GLU_TESS_ERROR_DATA, (TessCallBack) errorCallback);
    gluTessCallback((GLUtesselator*) tess, GLU_TESS_BEGIN_DATA, (TessCallBack) beginCallback);
    gluTessCallback((GLUtesselator*) tess, GLU_TESS_VERTEX_DATA, (TessCallBack) vertexCallback);
    gluTessCallback((GLUtesselator*) tess, GLU_TESS_COMBINE_DATA, (TessCallBack) combineCallback);
}

Tesselator::~Tesselator()
{
    gluDeleteTess((GLUtesselator*) tess);
}

void Tesselator::beginPolygon(ptr< Mesh<vec2f, unsigned int> > mesh)
{
    this->mesh = mesh;
    gluTessBeginPolygon((GLUtesselator*) tess, mesh.get());
}

void Tesselator::beginContour()
{
    gluTessBeginContour((GLUtesselator*) tess);
}

void Tesselator::newVertex(float x, float y)
{
    GLdouble coords[3];
    coords[0] = x;
    coords[1] = y;
    coords[2] = 0;
    int v = mesh->getVertexCount();
    mesh->addVertex(vec2f(x, y));
    gluTessVertex((GLUtesselator*) tess, coords, (GLvoid*) v);
}

void Tesselator::endContour()
{
    gluTessEndContour((GLUtesselator*) tess);
}

void Tesselator::endPolygon()
{
    gluTessEndPolygon((GLUtesselator*) tess);
    this->mesh = NULL;
}

}
