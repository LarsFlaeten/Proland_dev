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

#ifndef _PROLAND_SEG2_H_
#define _PROLAND_SEG2_H_

#include "ork/math/pmath.h"

#include "ork/math/vec2.h"
#include "ork/math/box2.h"
#include "proland/math/geometry.h"

using namespace ork;

namespace proland
{

/**
 * A 2D segment.
 * @ingroup proland_math
 * @authors Eric Bruneton, Antoine Begault
 */
template<typename type>
struct seg2
{
    /**
     * One of the segment extremity.
     */
    vec2<type> a;

    /**
     * The vector joining #a to the other segment extremity.
     */
    vec2<type> ab;

    /**
     * Creates a new segment with the given extremities.
     *
     * @param a a segment extremity.
     * @param b the other segment extremity.
     */
    seg2(const vec2<type> &a, const vec2<type> &b);

    /**
     * Returns the square distance between the given point and the line
     * defined by this segment.
     *
     * @param p a point.
     */
    type lineDistSq(const vec2<type> &p) const;

    /**
     * Returns the square distance between the given point and this segment.
     *
     * @param p a point.
     */
    type segmentDistSq(const vec2<type> &p) const;

    /**
     * Returns true if this segment intersects the given segment.
     *
     * @param s a segment.
     */
    bool intersects(const seg2 &s) const;

    bool intersects(const seg2 &s, type &t0) const;

    /**
     * Returns true if this segment intersects the given segment. If there
     * is an intersection it is returned in the vector.
     *
     * @param s a segment.
     * @param i where to store the intersection point, if any.
     */
    bool intersects(const seg2 &s, vec2<type> *i) const;

    /**
     * Returns true if this segment is inside or intersects the given
     * bounding box.
     *
     * @param r a bounding box.
     */
    bool intersects(const box2<type> &r) const;

    /**
     * Returns true if this segment, with the given width, contains the given
     * point. More precisely this method returns true if the stroked path
     * defined from this segment, with a cap "butt" style, contains the given
     * point.
     *
     * @param p a point.
     * @param w width of this segment.
     */
    bool contains(const vec2<type> &p, type w) const;
};

typedef seg2<float> seg2f;

typedef seg2<double> seg2d;

template<typename type>
inline seg2<type>::seg2(const vec2<type> &a, const vec2<type> &b) : a(a), ab(b - a)
{
}

template<typename type>
inline type seg2<type>::lineDistSq(const vec2<type> &p) const
{
    vec2<type> ap = p - a;
    type dotprod = ab.dot(ap);
    type projLenSq = dotprod * dotprod / ab.squaredLength();
    return ap.squaredLength() - projLenSq;
}

template<typename type>
inline type seg2<type>::segmentDistSq(const vec2<type> &p) const
{
    vec2<type> ap = p - a;
    type dotprod = ab.dot(ap);
    type projlenSq;

    if (dotprod <= 0.0) {
        projlenSq = 0.0;
    } else {
        ap = ab - ap;
        dotprod = ab.dot(ap);

        if (dotprod <= 0.0) {
            projlenSq = 0.0;
        } else {
            projlenSq = dotprod * dotprod / ab.squaredLength();
        }
    }

    return ap.squaredLength() - projlenSq;
}

template<typename type>
inline bool seg2<type>::intersects(const seg2 &s) const
{
    vec2<type> aa = s.a - a;
    type det = cross(ab, s.ab);
    type t0 = cross(aa, s.ab) / det;

    if (t0 > 0 && t0 < 1) {
        type t1 = cross(aa, ab) / det;
        return t1 > 0 && t1 < 1;
    }

    return false;
}

template<typename type>
inline bool seg2<type>::intersects(const seg2 &s, type &t0) const
{
    vec2<type> aa = s.a - a;
    type det = cross(ab, s.ab);
    t0 = cross(aa, s.ab) / det;

    if (t0 > 0 && t0 < 1) {
        type t1 = cross(aa, ab) / det;
        return t1 > 0 && t1 < 1;
    }

    return false;
}

template<typename type>
inline bool seg2<type>::intersects(const seg2 &s, vec2<type> *i) const
{
    vec2<type> aa = s.a - a;
    type det = cross(ab, s.ab);
    type t0 = cross(aa, s.ab) / det;

    if (t0 > 0 && t0 < 1) {
        *i = a + ab * t0;
        type t1 = cross(aa, ab) / det;
        return t1 > 0 && t1 < 1;
    }

    return false;
}

template<typename type>
inline bool seg2<type>::intersects(const box2<type> &r) const
{
    vec2<type> b = a + ab;
    if (r.contains(a) || r.contains(b)) {
        return true;
    }

    box2<type> t = box2<type>(a, b);
    if (t.xmin > r.xmax || t.xmax < r.xmin || t.ymin > r.ymax || t.ymax < r.ymin) {
        return false;
    }

    type p0 = cross(ab, vec2<type>(r.xmin, r.ymin) - a);
    type p1 = cross(ab, vec2<type>(r.xmax, r.ymin) - a);
    if (p1 * p0 <= 0) {
        return true;
    }
    type p2 = cross(ab, vec2<type>(r.xmin, r.ymax) - a);
    if (p2 * p0 <= 0) {
        return true;
    }
    type p3 = cross(ab, vec2<type>(r.xmax, r.ymax) - a);
    if (p3 * p0 <= 0) {
        return true;
    }

    return false;
}

template<typename type>
inline bool seg2<type>::contains(const vec2<type> &p, type w) const
{
    vec2<type> ap = p - a;
    type dotprod = ab.dot(ap);
    type projlenSq;

    if (dotprod <= 0.0) {
        return false;
    } else {
        ap = ab - ap;
        dotprod = ab.dot(ap);
        if (dotprod <= 0.0) {
            return false;
        } else {
            projlenSq = dotprod * dotprod / ab.squaredLength();
            return ap.squaredLength() - projlenSq < w*w;
        }
    }
}

}

#endif
