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

#ifndef _PROLAND_MOUSEPOSITIONHANDLER_H_
#define _PROLAND_MOUSEPOSITIONHANDLER_H_

#include <vector>

#include "ork/scenegraph/SceneNode.h"
#include "ork/ui/EventHandler.h"

#include "proland/terrain/TerrainNode.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * An EventHandler that can determine the position of the mouse in world space.
 * It can determine on which TerrainNode the cursor is, and the position inside it.
 * This EventHandler is only for debug purpose, since it requires costly operations.
 * (DepthBuffer read...). It then uses the ShowInfoTask to display the mouse position.
 * @ingroup proland_ui
 * @author Antoine Begault
 */
PROLAND_API class MousePositionHandler : public EventHandler
{
public:
    /**
     * Creates a new MousePositionhandler.
     *
     * @param terrains the list of SceneNodes contained in the scene mapped to their TerrainNodes.
     *      These SceneNodes are used to determine the transformation matrices, and the TerrainNodes
            are used to determine if the point is inside the terrain.
     * @param next the EventHandler that must handle the events.
     */
    MousePositionHandler(map<ptr<SceneNode>, ptr<TerrainNode> > terrains, ptr<EventHandler> next);

    /**
     * Deletes this MousePositionHandler.
     */
    virtual ~MousePositionHandler();

    virtual void redisplay(double t, double dt);

    virtual void reshape(int x, int y);

    virtual void idle(bool damaged);

    /**
     * Finds the TerrainQuad that contains the given coordinates.
     */
    ptr<TerrainQuad> findTile(float x, float y, ptr<TerrainQuad> quad);

    /**
     * Determines the terrain and the terrain tile that contains the given coordinates.
     * It will set #mousePosition, #currentTerrain, #terrainPosition, and #tile.
     */
    void getWorldCoordinates(int x, int y);

    virtual bool mouseClick(button b, state s, modifier m, int x, int y);

    virtual bool mouseMotion(int x, int y);

    virtual bool mousePassiveMotion(int x, int y);

    virtual bool mouseWheel(wheel b, modifier m, int x, int y);

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y);

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y);

    virtual bool specialKey(key k, modifier m, int x, int y);

    virtual bool specialKeyReleased(key k, modifier m, int x, int y);

protected:
    /**
     * The list of SceneNodes contained in the scene mapped to their TerrainNodes.
     * These SceneNodes are used to determine the transformation matrices, and the TerrainNodes
     * are used to determine if the point is inside the terrain.
     */
    map<ptr<SceneNode>, ptr<TerrainNode> > terrains;

    /**
     *Creates a new MousePositionHandler.
     */
    MousePositionHandler();

    /**
     * Initializes this MousePositionHandler.
     * See #MousePositionHandler.
     */
    void init(map<ptr<SceneNode>, ptr<TerrainNode> > terrains, ptr<EventHandler> next);

    void swap(ptr<MousePositionHandler> mousePositionHandler);

private:
    /**
     * Displayed mouse position.
     * Retrieved when the mouse moves.
     */
    vec2i mousePosition;

    float mousePositionZ;

    /**
     * The terrain pointed by the cursor.
     * If no terrain is under the cursor, it will be set to -1 and no terrain coordinates
     * will be displayed, only mouse coordinates.
     */
    int currentTerrain;

    /**
     * Local position inside the terrain pointer by the cursor.
     */
    vec3d terrainPosition;

    /**
     * Coordinates of the tile (level, tx, ty) pointed inside the terrain.
     */
    vec3i tile;

    /**
     * The EventHandler that must handle the events.
     */
    ptr<EventHandler> next;

};

}

#endif
