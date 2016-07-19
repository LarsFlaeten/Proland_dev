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

#include "proland/terrain/ReadbackManager.h"

namespace proland
{

const size_t BUFFER_ALIGNMENT = 32;

ReadbackManager::ReadbackManager(int maxReadbackPerFrame, int readbackDelay, int bufferSize) :
    Object("ReadbackManager"),
    maxReadbackPerFrame(maxReadbackPerFrame),
    readbackDelay(readbackDelay),
    bufferSize(bufferSize)
{
    readCount = new int[readbackDelay];
    toRead = new ptr<GPUBuffer>*[readbackDelay];
    toReadCallbacks = new ptr<Callback>*[readbackDelay];

    for (int i = 0; i < readbackDelay; ++i) {
        readCount[i] = 0;
        toRead[i] = new ptr<GPUBuffer>[maxReadbackPerFrame];
        toReadCallbacks[i] = new ptr<Callback>[maxReadbackPerFrame];
        for (int j = 0; j < maxReadbackPerFrame; ++j) {
            toRead[i][j] = new GPUBuffer();
            toRead[i][j]->setData(bufferSize, NULL, STREAM_READ);
        }
    }
}

ReadbackManager::~ReadbackManager()
{
    for (int i = 0; i < readbackDelay; ++i) {
        delete[] toRead[i];
        delete[] toReadCallbacks[i];
    }
    delete[] toRead;
    delete[] toReadCallbacks;
    delete[] readCount;
}

bool ReadbackManager::canReadback()
{
    return readCount[0] < maxReadbackPerFrame;
}

bool ReadbackManager::readback(ptr<FrameBuffer> fb, int x, int y, int w, int h, TextureFormat f, PixelType t, ptr<Callback> cb)
{
    if (readCount[0] < maxReadbackPerFrame) {
        int index = readCount[0];
        fb->readPixels(x, y, w, h, f, t, Buffer::Parameters(), *(toRead[0][index]));
        toReadCallbacks[0][index] = cb;
        ++readCount[0];
        return true;
    } else {
        assert(false); // should not happen, call canReadback before
        return false;
    }
}

void ReadbackManager::newFrame()
{
    int lastIndex = readbackDelay - 1;
    for (int i = 0; i < readCount[lastIndex]; ++i) {
        BufferAccess a = READ_ONLY;
        volatile void *data = toRead[lastIndex][i]->map(a);
        toReadCallbacks[lastIndex][i]->dataRead(data);
        toReadCallbacks[lastIndex][i] = NULL;
        toRead[lastIndex][i]->unmap();
    }

    // rotate buffer to the left and clear readCount
    ptr<GPUBuffer> *bufs = toRead[lastIndex];
    ptr<Callback> *calls = toReadCallbacks[lastIndex];

    for (int i = readbackDelay - 1; i > 0; --i) {
        readCount[i] = readCount[i - 1];
        toRead[i] = toRead[i - 1];
        toReadCallbacks[i] = toReadCallbacks[i - 1];
    }
    readCount[0] = 0;
    toRead[0] = bufs;
    toReadCallbacks[0] = calls;
}

}
