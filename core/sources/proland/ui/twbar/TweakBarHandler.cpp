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

#include "proland/ui/twbar/TweakBarHandler.h"

namespace proland
{

TweakBarHandler::TweakBarHandler() : Object("TweakBarHandler")
{
}

TweakBarHandler::TweakBarHandler(const char *name, ptr<EventHandler> eventHandler, bool active) : Object("TweakBarHandler")
{
    init(name, eventHandler, active);
}

void TweakBarHandler::init(const char *name, ptr<EventHandler> eventHandler, bool active)
{
    this->name = name;
    this->eventHandler = eventHandler;
    this->active = active;
    this->needUpdate = false;
}

TweakBarHandler::~TweakBarHandler()
{
}

const char* TweakBarHandler::getName()
{
    return name;
}

bool TweakBarHandler::isActive()
{
    return active;
}

void TweakBarHandler::setActive(bool active)
{
    this->active = active;
}

void TweakBarHandler::redisplay(double t, double dt, bool &needUpdate)
{
    needUpdate = this->needUpdate;
    this->needUpdate = false;
    if (eventHandler != NULL) {
        eventHandler->redisplay(t, dt);
    }
}

void TweakBarHandler::reshape(int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        eventHandler->reshape(x, y);
    }
}

void TweakBarHandler::idle(bool damaged, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        eventHandler->idle(damaged);
    }
}

bool TweakBarHandler::mouseClick(EventHandler::button b, EventHandler::state s, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->mouseClick(b, s, m, x, y);
    }
    return false;
}

bool TweakBarHandler::mouseWheel(EventHandler::wheel b, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->mouseWheel(b, m, x, y);
    }
    return false;
}

bool TweakBarHandler::mouseMotion(int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->mouseMotion(x, y);
    }
    return false;
}

bool TweakBarHandler::mousePassiveMotion(int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->mousePassiveMotion(x, y);
    }
    return false;
}

bool TweakBarHandler::keyTyped(unsigned char c, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->keyTyped(c, m, x, y);
    }
    return false;
}

bool TweakBarHandler::keyReleased(unsigned char c, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->keyReleased(c, m, x, y);
    }
    return false;
}

bool TweakBarHandler::specialKey(EventHandler::key  k, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->specialKey(k, m, x, y);
    }
    return false;
}

bool TweakBarHandler::specialKeyReleased(EventHandler::key  k, EventHandler::modifier m, int x, int y, bool &needUpdate)
{
    needUpdate = false;
    if (eventHandler != NULL) {
        return eventHandler->specialKeyReleased(k, m, x, y);
    }
    return false;
}

void TweakBarHandler::swap(ptr<TweakBarHandler> t)
{
    std::swap(eventHandler, t->eventHandler);
    std::swap(name, t->name);
    needUpdate = true;
}

}
