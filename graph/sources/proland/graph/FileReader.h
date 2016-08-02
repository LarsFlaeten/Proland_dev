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

#ifndef _PROLAND_FILEREADER_H_
#define _PROLAND_FILEREADER_H_

#include <fstream>
#include <iostream>

#include "ork/core/Object.h"

using namespace std;

namespace proland
{

/**
 * FileReader handles file inputs for graph loading.
 * Handles binary & ascii.
 * @ingroup graph
 * @author Antoine Begault
 */
PROLAND_API class FileReader
{
public:
    /**
     * Creates a new FileReader.
     *
     * @param file the path/name of the file to read.
     * @param isIndexed after function call, will be true if the magic number was 1. False otherwise.
     */
    FileReader(const string &file, bool &isIndexed);

    /**
     * Deletes this FileReader.
     */
    ~FileReader();

    /**
     * Templated read Method.
     * Example : read<int>().
     *
     * @return a T value read from the input file stream at get pointer.
     */
    template <typename T> T read()
    {
        T t;
        if (isBinary) {
            in.read((char*) &t, sizeof(T));
        } else {
            in >> t;
        }
        return t;
    }

    /**
     * Returns the position of the get pointer.
     */
    streampos tellg();

    /**
     * Sets the position of the get pointer.
     * The get pointer determines the next location to be read in the source
     * associated to the stream.
     *
     * @param off integral value of type streamoff representing the offset
     *     to be applied relative to an absolute position specified in the
     *     dir parameter.
     * @param dir seeking direction. It is an object of type ios_base::seekdir
     *     that specifies an absolute position from where the offset parameter
     *     off is applied. It can take any of the following member constant
     *     values : ios_base::beg | ios_base::cur | ios_base::end.
     */
    void seekg(streamoff off, ios_base::seekdir dir);

    /**
     * Returns true if an error occured while reading. The errors corresponds
     * to ifstream errors.
     */
    bool error();

private:
    /**
     * The input filestream.
     */
    ifstream in;

    /**
     * If true, file will be read as binary. Otherwise, ASCII.
     */
    bool isBinary;
};

}

#endif
