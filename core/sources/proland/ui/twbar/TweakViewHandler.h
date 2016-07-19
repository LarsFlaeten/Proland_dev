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

#ifndef _PROLAND_TWEAKVIEWHANDLER_H_
#define _PROLAND_TWEAKVIEWHANDLER_H_

#include "proland/ui/BasicViewHandler.h"
#include "proland/ui/twbar/TweakBarHandler.h"

namespace proland
{

/**
 * A TweakBarHandler to control a BasicViewHandler. This class provides
 * tweak bar buttons corresponding to predefined positions, and allows the
 * users to go instantly or smoothly to one of these predefined positions.
 * Another button prints the current position.
 * @ingroup twbar
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class TweakViewHandler : public TweakBarHandler
{
public:
    /**
     * A BasicViewHandler::Position together with a name and a shortcut key.
     */
    class Position : public BasicViewHandler::Position {
    public:
        /**
         * The TweakViewHandler to which this predefined position belongs.
         */
        TweakViewHandler *owner;

        /**
         * The name of this predefined position.
         */
        const char *name;

        /**
         * The shortcut key for this predefined position, or 0.
         */
        char key;

        /**
         * Goes to this position instantly or smoothly, depending on
         * TweakViewHandler#animate.
         */
        void go();
    };

    /**
     * Creates a new TweakViewHandler.
     *
     * @param viewHandler the BasicViewHandler to be controlled by this
     *      TweakViewHandler.
     * @param views a list of predefined positions with names and
     *      shortcut keys.
     * @param animate true to go smoothly to target positions, false to
     *      move instantly to them.
     * @param active true if this TweakBarHandler must be initialy active.
     */
    TweakViewHandler(ptr<BasicViewHandler> viewHandler, const std::vector<Position> &views, bool animate, bool active);

    /**
     * Deletes this TweakViewHandler.
     */
    virtual ~TweakViewHandler();

protected:
    /**
     * Creates an uninitialized TweakViewHandler.
     */
    TweakViewHandler();

    /**
     * Initializes this TweakViewHandler.
     * See #TweakViewHandler.
     */
    virtual void init(ptr<BasicViewHandler> viewHandler, const std::vector<Position> &views, bool animate, bool active);

    virtual void updateBar(TwBar *bar);

    void swap(ptr<TweakViewHandler> o);

private:
    /**
     * The BasicViewHandler to be controlled by this TweakViewHandler.
     */
    ptr<BasicViewHandler> viewHandler;

    /**
     * A list of predefined positions with names and shortcut keys.
     */
    std::vector<Position> views;

    /**
     * True to go smoothly to target positions, false to move instantly to
     * them.
     */
    bool animate;

    friend class Position;
};

}

#endif
