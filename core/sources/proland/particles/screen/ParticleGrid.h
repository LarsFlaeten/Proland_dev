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

#ifndef _PROLAND_PARTICLE_GRID_H
#define _PROLAND_PARTICLE_GRID_H

#include "ork/render/TextureRectangle.h"
#include "proland/particles/screen/ScreenParticleLayer.h"

using namespace ork;

namespace proland
{

/**
 * A 2D grid containing %particles, used to quickly find the neighbors of a
 * particle or the %particles covering a given point. Each cell of the grid
 * contains the %particles that cover this cell (based on the screen particle
 * position and a specified particle radius, all in pixels). The cell size is
 * set approximatively to the particle radius. The grid covers a specified
 * viewport (in pixels), and its cells are recomputed when this viewport
 * changes (so that the size of each cell stays approximatively equal to the
 * particle radius, in pixels). This class can also copy this grid in a GPU
 * texture.
 * @ingroup screen
 * @author Antoine Begault
 */
PROLAND_API class ParticleGrid : public Object
{
public:
    /**
     * Creates a new ParticleGrid.
     *
     * @param radius the radius of each particle. A particle is added to each
     *      cell covered by its radius.
     * @param maxParticlesPerCell maximum number of particles per grid cell.
     * @param gridFactor factor for the grid size, if required to be different
     *      to the specified one.
     */
    ParticleGrid(float radius, int maxParticlesPerCell, float gridFactor = 1.0f);

    /**
     * Deletes this ParticleGrid.
     */
    ~ParticleGrid();

    /**
     * Returns the radius of each particle.
     */
    float getParticleRadius() const;

    /**
     * Sets the radius of each particle. This clears the grid content.
     */
    void setParticleRadius(float radius);

    /**
     * Returns the maximum number of %particles per cell.
     */
    int getMaxParticlesPerCell() const;

    /**
     * Sets the maximum number of %particles per cell.
     */
    void setMaxParticlesPerCell(int maxParticlesPerCell);

    /**
     * Returns the viewport covered by this grid.
     */
    box2i getViewport() const;

    /**
     * Sets the viewport that should be covered by this grid.
     * This clears the grid content.
     */
    void setViewport(const box2i &viewport);

    /**
     * Returns the number of columns and rows of the grid.
     */
    vec2i getGridSize() const;

    /**
     * Returns the grid cell coordinates corresponding to the given point.
     *
     * @param p a point inside the viewport #getViewport().
     */
    vec2i getCell(const vec2f &p);

    /**
     * Returns the number of %particles in the given cell.
     *
     * @param cell a cell given by its x and y coordinates (in number of
     *      columns and rows).
     */
    int getCellSize(const vec2i &cell);

    /**
     * Returns the %particles in the given cell.
     *
     * @param cell a cell given by its x and y coordinates (in number of
     *      columns and rows).
     * @return an array of #getCellSize() %particles.
     */
    ScreenParticleLayer::ScreenParticle **getCellContent(const vec2i &cell);

    /**
     * Adds a particle to this grid. The particle is added to each cell
     * covered by the disk of radius #getParticleRadius() around the particle.
     *
     * @param p a particle.
     */
    void addParticle(ScreenParticleLayer::ScreenParticle *p, float intensity);

    /**
     * Removes all the %particles from the grid.
     */
    void clear();

    /**
     * Copies the content of this ParticleGrid to the given texture. Each
     * cell is represented with pixelsPerCell RGBA float values representing
     * particle indexes. The cell %particles are stored in the first pixels
     * of the cell, and -1 is used to mark the end of the particle list.
     *
     * @param l the layer managing the %particles stored in this grid. This
     *      layer is used to compute the index of each particle via
     *      ParticleStorage#getParticleIndex().
     * @param t the destination texture, or NULL to create a new one.
     * @param[out] pixelsPerCell the number of pixels used to represent each
     *      cell in t. Equals to ceil(getMaxParticlesPerCell() / 4).
     * @return the given texture, or a new one if t was not of correct size.
     *      The size of the texture must be gridSize.x * pixelsPerCell times
     *      gridSize.y pixels.
     */
    ptr<Texture2D> copyToTexture(ptr<ScreenParticleLayer> l, ptr<Texture2D> t, int &pixelsPerCell);

private:
    /**
     * The radius of each particle, in pixels.
     */
    float radius;

    /**
     * Maximum number of %particles per grid cell.
     */
    int maxParticlesPerCell;

    /**
     * The viewport covered by this particle grid, in pixels.
     */
    box2i viewport;

    /**
     * The number of columns and rows of the particle grid.
     */
    vec2i gridSize;

    /**
     * The number of columns and rows of the particle grid sent to the GPU.
     */
    vec2i cellGridSize;

    /**
     * The number of %particles in each grid cell. This array is of
     * size gridSize.x * gridSize.y.
     */
    int *cellSizes;

    /**
     * The %particles in each grid cell.
     * This array is of size gridSize.x * gridSize.y * maxParticlesPerCell.
     */
    ScreenParticleLayer::ScreenParticle **cellContents;

    float *intensities;

    /**
     * The indexes of the %particles in each gpu grid cell.
     * This array is of size gpuGridSize.x * gpuGridSize.y * maxParticlesPerGpuCell.
     */
    int *cellIndexes;

    /**
     * Factor for the grid size, if required to be different to the specified one.
     */
    float gridFactor;

    /**
     * Creates the grid for the current viewport and particle radius.
     */
    void createGrid();

    /**
     * Deletes the grid.
     */
    void deleteGrid();
};

}

#endif
