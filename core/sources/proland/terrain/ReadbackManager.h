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

#ifndef _PROLAND_READBACK_MANAGER_H_
#define _PROLAND_READBACK_MANAGER_H_

#include "ork/render/FrameBuffer.h"

using namespace ork;

namespace proland
{

/**
 * A manager for asynchronous readbacks from a framebuffer. Asynchronous means
 * that readbacks are non blocking: a read operation returns immediately
 * with an empty result, and the actual result is passed via a callback
 * function when it becomes available (in practice n frames after the read was
 * started, where n is user defined).
 * @ingroup terrain
 * @authors Eric Bruneton, Antoine Begault
*/
PROLAND_API class ReadbackManager : public Object
{
public:
    /**
     * A callback function called when a readback is done; see ReadbackManager.
     */
    class Callback : public Object
    {
    public:
        /**
         * Creates a new callback object.
         */
        Callback() : Object("ReadbackManager::Callback")
        {
        }

        /**
         * Called when a readback is finished.
         *
         * @param data the data that has been read.
         */
        virtual void dataRead(volatile void *data) = 0;
    };

    /**
     * Creates a new readback manager.
     *
     * @param maxReadbackPerFrame maximum number of readbacks that can be
     * started per frame.
     * @param readbackDelay number of frames between the start of a readback
     * and its end.
     * @param bufferSize maximum number of bytes per readback.
     */
    ReadbackManager(int maxReadbackPerFrame, int readbackDelay, int bufferSize);

    /**
     * Destroys this readback manager.
     */
    virtual ~ReadbackManager();

    /**
     * Returns true if a new readback can be started for the current frame.
     */
    bool canReadback();

    /**
     * Starts a new readback and returns immediately. Returns true if the
     * readback has effectively started (see #canReadback()).
     *
     * @param fb the framebuffer from which the data must be read. Data will
     * be read from the last buffer specified with FrameBuffer#setReadBuffer
     * for this framebuffer.
     * @param x x coordinate of the lower left corner of the region to be read.
     * @param y x coordinate of the lower left corner of the region to be read.
     * @param w width of the region to be read.
     * @param h height the region to be read.
     * @param f the components to be read.
     * @param t the type to be used to store the read data.
     * @param cb the function to be called when the readback is finished.
     */
    bool readback(ptr<FrameBuffer> fb, int x, int y, int w, int h, TextureFormat f, PixelType t, ptr<Callback> cb);

    /**
     * Informs this manager that a new frame has started.
     */
    void newFrame();

private:
    int maxReadbackPerFrame;
    int readbackDelay;
    int *readCount;
    ptr<GPUBuffer> **toRead;
    ptr<Callback> **toReadCallbacks;
    int bufferSize;
};

}

#endif
