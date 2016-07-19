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

#ifndef _PROLAND_TWEAKRESOURCE_H_
#define _PROLAND_TWEAKRESOURCE_H_

#include <vector>
#include "tinyxml/tinyxml.h"

#include "ork/resource/ResourceManager.h"
#include "proland/ui/twbar/TweakBarHandler.h"

using namespace ork;

namespace proland
{

/**
 * A TweakBarHandler to %edit resources.
 * @ingroup twbar
 * @authors Eric Bruneton, Antoine Begault
 */
PROLAND_API class TweakResource : public TweakBarHandler
{
public:
    /**
     * Abstract data class used for tweak bar callbacks.
     */
    class Data
    {
    public:
        /**
         * Creates a new tweak bar data.
         */
        Data();

        /**
         * Deletes this tweak bar data.
         */
        virtual ~Data();
    };

    /**
     * Creates a new TweakResource.
     *
     * @param name the name of this TweakResource.
     * @param manager a resource manager to load the resources defined in e.
     * @param e an XML description of the controls that must be provided by
     *      this TweakResource.
     */
    TweakResource(std::string name, ptr<ResourceManager> manager, const TiXmlElement *e);

    /**
     * Deletes this TweakResource.
     */
    virtual ~TweakResource();

    virtual void updateBar(TwBar *bar);

protected:
    /**
     * Creates an uninitialized TweakResource.
     */
    TweakResource();

    /**
     * Initializes this TweakResource.
     * See #TweakResource.
     */
    void init(std::string name, ptr<ResourceManager> manager, const TiXmlElement *e);

    void swap(ptr<TweakResource> p);

private:
    /**
     * A resource manager to load the resources defined in #e.
     */
    ptr<ResourceManager> manager;

    /**
     * An XML description of the controls that must be provided by this
     * TweakResource.
     */
    TiXmlElement *e;

    /**
     * The tweak bar data managed by this TweakResource.
     */
    std::vector<Data*> datas;

    void clearData();

    void setParam(TwBar *bar, const TiXmlElement *el, const char* param);
};

}

#endif
