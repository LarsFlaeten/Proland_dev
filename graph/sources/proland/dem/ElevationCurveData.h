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

#ifndef _PROLAND_ELEVATIONCURVEDATA_H_
#define _PROLAND_ELEVATIONCURVEDATA_H_

#include "proland/graph/producer/CurveData.h"

namespace proland
{

/**
 * A CurveData that contains the elevation profile of a curve. It can produce
 * monotonic (i.e. only increasing or only decreasing) profiles, or
 * unconstrained profiles (both increasing and decreasing). Monotonic profiles
 * are useful for %rivers, which can never flow uphill. An elevation profile is
 * created by taking %terrain height samples regularly spaced along the curve,
 * by making them monotonic if desired, and by smoothing the resulting samples.
 * For large curve widths the distance between samples is larger, the
 * quadtree level used to get their altitude is lower, and the smoothing is
 * more aggressive, than for narrower curves. This gives smoother elevation
 * profiles for wider curves (a highway has less "bumps" than a small road).
 * @ingroup dem
 * @authors Antoine Begault, Eric Bruneton, Guillaume Piolat
 */
PROLAND_API class ElevationCurveData : public CurveData
{
public:
    /**
     * Creates a new ElevationCurveData.
     *
     * @param id the id of the curve for which we need to store the data.
     * @param flattenCurve the flattened version of the curve for which we need
     *      to store the data.
     * @param elevations the %producer used to compute raw %terrain elevations,
     *      themselves used to compute the elevation profile.
     * @param monotonic true for only increasing or only decreasing profiles,
     *      false for profiles with both increasing and decreasing sections.
     */
    ElevationCurveData(CurveId id, CurvePtr flattenCurve, ptr<TileProducer> elevations, bool monotonic);

    /**
     * Deletes this ElevationCurveData.
     */
    virtual ~ElevationCurveData();

    /**
     * Returns the height of the starting point of the curve.
     */
    virtual float getStartHeight();

    /**
     * Returns the height of the ending point of the curve.
     */
    virtual float getEndHeight();

    /**
     * Returns the height corresponding to the given s coordinate.
     *
     * @param s a pseudo curvilinear coordinate (see Curve::Vertex#s).
     */
    virtual float getAltitude(float s);

    /**
     * Returns the distance to be used between samples for the elevation
     * profile of the given curve. This distance depends on the curve's
     * width (so that wider curves have a smoother profile).
     *
     * @param c a curve.
     */
    virtual float getSampleLength(CurvePtr c);

    /**
     * Returns the smoothing factor to get the smoothed elevation
     * profile for the given curve. This factor depends on the curve's
     * width (so that wider curves have a smoother profile). A smoothing
     * factor of n indicates that (2n+1) consecutive samples must be
     * averaged together to get a smooth sample.
     *
     * @param c a curve.
     */
    virtual int getSmoothFactor(CurvePtr c);

    /**
     * Returns the list of tiles intersected by the curve with which this
     * ElevationCurveData is associated.
     *
     * @param[out] tiles the list of usedTiles.
     * @param rootSampleLength Curve sample length at level 0.
     */
    virtual void getUsedTiles(set<TileCache::Tile::Id> &tiles, float rootSampleLength);

protected:
    /**
     * The %producer used to compute raw %terrain elevations, themselves used to compute
     * the elevation profile.
     */
    ptr<TileProducer> elevations;

    /**
     * true for an only increasing or only decreasing profile, false for a profile with
     * both increasing and decreasing sections.
     */
    bool monotonic;

    /**
     * The distance to be used between samples for this elevation profile.
     */
    float sampleLength;

    /**
     * Number of samples in this elevation profile.
     */
    int sampleCount;

    /**
     * The raw elevation samples of this elevation profile (before making
     * them monotonic and smoothing them).
     */
    float *samples;

    /**
     * The elevation samples after they have been made monotonic.
     */
    float *monotonicSamples;

    /**
     * The smoothing factor to get the smoothed elevation profile. A
     * smoothing factor of n indicates that (2n+1) consecutive monotonic
     * samples must be averaged together to get a smooth sample.
     */
    int smoothFactor;

    /**
     * The elevation samples after they have been made monotonic and smoothed.
     */
    float *smoothedSamples;

    /**
     * Computes a raw elevation sample of the curve.
     *
     * @param p the coordinates of the sample to compute.
     */
    float getSample(const vec2d &p);

    /**
     * Computes a raw elevation sample of the curve.
     *
     * @param i index of the point to compute.
     */
    float getSample(int i);

    /**
     * Computes a monotonic elevation sample of the curve.
     *
     * @param i index of the sample to compute.
     */
    float getMonotonicSample(int i);

    /**
     * Computes a symetric elevation sample of the curve. This function
     * computes additional samples before 0 or after sampleCount - 1, by
     * symmetry around the two endpoint samples. These extra samples are
     * needed to computed smoothed endpoint samples (since smoothFactor
     * samples are needed on both sides of a monotonic sample to get a
     * smoothed sample).
     *
     * @param i index of the sample to compute.
     */
    float getSymetricSample(int i);

    /**
     * Computes a smoothed elevation sample of the curve.
     *
     * @param i index of the sample to compute.
     */
    float getSmoothedSample(int i);

};

}

#endif
