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

#ifndef _PROLAND_TWEAKSCENEGRAPH_H_
#define _PROLAND_TWEAKSCENEGRAPH_H_

#include "ork/math/box2.h"
#include "ork/render/Mesh.h"
#include "ork/render/Uniform.h"
#include "ork/scenegraph/SceneNode.h"
#include "proland/ui/twbar/TweakBarHandler.h"

using namespace ork;

namespace proland
{

/**
 * A TweakBarHandler to control the scene graph.
 * @ingroup twbar
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class TweakSceneGraph : public TweakBarHandler
{
public:
    struct TextureInfo
    {
    public:
        ptr<Texture> tex;

        int level;

        int mode;

        vec4f norm;

        box2f area;

        TextureInfo(ptr<Texture> t) : tex(t), level(0), mode(1),
            norm(1.0f, 1.0f, 1.0f, 1.0f), area(0.0f, 1.0f, 0.0f, 1.0f)
        {
        }
    };

    /**
     * Creates a new TweakSceneGraph.
     *
     * @param scene the root of the scene graph to control.
     * @param active true if this TweakBarHandler must be initialy active.
     */
    TweakSceneGraph(ptr<SceneNode> scene, bool active);

    /**
     * Deletes this TweakSceneGraph.
     */
    virtual ~TweakSceneGraph();

    int getCurrentTexture() const;

    int getCurrentLevel() const;

    int getCurrentMode() const;

    box2f getCurrentArea() const;

    vec4f getCurrentNorm() const;

    void setCurrentTexture(int tex);

    void setCurrentLevel(int level);

    void setCurrentMode(int mode);

    void setCurrentArea(const box2f &area);

    void setCurrentNorm(const vec4f &n);

    virtual void redisplay(double t, double dt, bool &needUpdate);

    virtual bool mouseClick(EventHandler::button b, EventHandler::state s, EventHandler::modifier m, int x, int y, bool &needUpdate);

    virtual bool mouseWheel(EventHandler::wheel b, EventHandler::modifier m, int x, int y, bool &needUpdate);

    virtual bool mouseMotion(int x, int y, bool &needUpdate);

    virtual bool mousePassiveMotion(int x, int y, bool &needUpdate);

protected:
    /**
     * Creates an uninitialized TweakSceneGraph.
     */
    TweakSceneGraph();

    /**
     * Initializes this TweakSceneGraph.
     * See #TweakSceneGraph.
     */
    virtual void init(ptr<SceneNode> scene, bool active);

    virtual void updateBar(TwBar *bar);

    void swap(ptr<TweakSceneGraph> o);

private:
    enum MODE
    {
        NONE,
        MOVING,
        MOVING_UV,
        ZOOMING
    };

    /**
     * The root of the scene graph to control.
     */
    ptr<SceneNode> scene;

    std::map<std::string, TextureInfo> textures;

    int currentTexture;

    TextureInfo *currentInfo;

    box2f displayBox;

    ptr<Program> renderProg;
    ptr<UniformSampler> renderTexture1DU;
    ptr<UniformSampler> renderTexture2DU;
    ptr<UniformSampler> renderTexture2DArrayU;
    ptr<UniformSampler> renderTexture3DU;
    ptr<Uniform1f> renderTypeU;
    ptr<Uniform1f> renderLevelU;
    ptr<Uniform1f> renderModeU;
    ptr<Uniform4f> renderNormU;
    ptr<Uniform4f> renderPositionU;
    ptr<Uniform4f> renderCoordsU;
    ptr<Uniform3i> renderGridU;

    ptr<Program> selectProg;
    ptr<Uniform4f> selectPositionU;

    MODE mode;

    vec2f lastPos;

    vec2f newPos;
};

}

#endif
