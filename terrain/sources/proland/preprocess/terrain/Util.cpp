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

#include "proland/preprocess/terrain/Util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

#include "ork/core/Object.h"

namespace proland
{

float id(float x)
{
    return x;
}

bool fexists(const string &name)
{
    int fd = open(name.c_str(), O_RDONLY);
    if (fd != -1) {
        close(fd);
        return true;
    }
    return false;
}

bool flog(const string &name)
{
    static char *lastGeneratedFile = NULL;
    if (lastGeneratedFile == NULL) {
        lastGeneratedFile = new char[512];
        lastGeneratedFile[0] = 0;
        FILE *f;
        fopen(&f, "log.txt", "r");
        if (f) {
            fgets(lastGeneratedFile, 512, f);
            fclose(f);
        }
    }
    if (strcmp(name.c_str(), lastGeneratedFile) == 0) {
        printf("GENERATING %s\n", name.c_str());
        return true;
    }
    int fd = open(name.c_str(), O_RDONLY);
    if (fd != -1) {
        // file already exists
        close(fd);
        return false;
    } else {
        printf("GENERATING %s\n", name.c_str());
        // file does not exist yet
        FILE *f;
        fopen(&f, "log.txt", "w");
        fputs(name.c_str(), f);
        fclose(f);
        return true;
    }
}

byte *globalOutData;

word ColorTo565( const byte *color ) {
	return ( ( color[ 0 ] >> 3 ) << 11 ) | ( ( color[ 1 ] >> 2 ) << 5 ) | ( color[ 2 ] >> 3 );
}

void ExtractBlock( const byte *inPtr, int width, byte *colorBlock ) {
	for ( int j = 0; j < 4; j++ ) {
		memcpy( &colorBlock[j*4*4], inPtr, 4*4 );
		inPtr += width * 4;
	}
}

void EmitByte( byte b ) {
	globalOutData[0] = b;
	globalOutData += 1;
}

void EmitWord( word s ) {
	globalOutData[0] = ( s >> 0 ) & 255;
	globalOutData[1] = ( s >> 8 ) & 255;
	globalOutData += 2;
}

void EmitDoubleWord( dword i ) {
	globalOutData[0] = ( i >> 0 ) & 255;
	globalOutData[1] = ( i >> 8 ) & 255;
	globalOutData[2] = ( i >> 16 ) & 255;
	globalOutData[3] = ( i >> 24 ) & 255;
	globalOutData += 4;
}

void SwapColors( byte *c1, byte *c2 ) {
	byte tm[3];
	memcpy( tm, c1, 3 );
	memcpy( c1, c2, 3 );
	memcpy( c2, tm, 3 );
}

#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )

void GetMinMaxColorsDXT1( const byte *colorBlock, byte *minColor, byte *maxColor ) {
	int i;
	byte inset[3];
	minColor[0] = minColor[1] = minColor[2] = 255;
	maxColor[0] = maxColor[1] = maxColor[2] = 0;
	for ( i = 0; i < 16; i++ ) {
		if ( colorBlock[i*4+0] < minColor[0] ) { minColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] < minColor[1] ) { minColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] < minColor[2] ) { minColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+0] > maxColor[0] ) { maxColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] > maxColor[1] ) { maxColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] > maxColor[2] ) { maxColor[2] = colorBlock[i*4+2]; }
	}
	inset[0] = ( maxColor[0] - minColor[0] ) >> INSET_SHIFT;
	inset[1] = ( maxColor[1] - minColor[1] ) >> INSET_SHIFT;
	inset[2] = ( maxColor[2] - minColor[2] ) >> INSET_SHIFT;
	minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
	minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
	minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
	maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
	maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
	maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
}

//#define ALIGN16( x ) __declspec(align(16)) x
#define ALIGN16( x ) x __attribute__ ((aligned (16)))

#define R_SHUFFLE_D( x, y, z, w ) (( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 ))

#define C565_5_MASK 0xF8 // 0xFF minus last three bits
#define C565_6_MASK 0xFC // 0xFF minus last two bits

void EmitColorIndices( const byte *colorBlock, const byte *minColor, const byte *maxColor ) {
	word colors[4][4];
	dword result = 0;
	colors[0][0] = ( maxColor[0] & C565_5_MASK ) | ( maxColor[0] >> 5 );
	colors[0][1] = ( maxColor[1] & C565_6_MASK ) | ( maxColor[1] >> 6 );
	colors[0][2] = ( maxColor[2] & C565_5_MASK ) | ( maxColor[2] >> 5 );
	colors[1][0] = ( minColor[0] & C565_5_MASK ) | ( minColor[0] >> 5 );
	colors[1][1] = ( minColor[1] & C565_6_MASK ) | ( minColor[1] >> 6 );
	colors[1][2] = ( minColor[2] & C565_5_MASK ) | ( minColor[2] >> 5 );
	colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
	colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
	colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
	colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
	colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
	colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;
	for ( int i = 15; i >= 0; i-- ) {
		int c0 = colorBlock[i*4+0];
		int c1 = colorBlock[i*4+1];
		int c2 = colorBlock[i*4+2];
		int d0 = abs( colors[0][0] - c0 ) + abs( colors[0][1] - c1 ) + abs( colors[0][2] - c2 );
		int d1 = abs( colors[1][0] - c0 ) + abs( colors[1][1] - c1 ) + abs( colors[1][2] - c2 );
		int d2 = abs( colors[2][0] - c0 ) + abs( colors[2][1] - c1 ) + abs( colors[2][2] - c2 );
		int d3 = abs( colors[3][0] - c0 ) + abs( colors[3][1] - c1 ) + abs( colors[3][2] - c2 );
		int b0 = d0 > d3;
		int b1 = d1 > d2;
		int b2 = d0 > d2;
		int b3 = d1 > d3;
		int b4 = d2 > d3;
		int x0 = b1 & b2;
		int x1 = b0 & b3;
		int x2 = b0 & b4;
		result |= ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
	}
	EmitDoubleWord( result );
}

void CompressImageDXT1( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes ) {
	ALIGN16( byte block[64] );
	ALIGN16( byte minColor[4] );
	ALIGN16( byte maxColor[4] );
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) {
		for ( int i = 0; i < width; i += 4 ) {
			ExtractBlock( inBuf + i * 4, width, block );
			GetMinMaxColorsDXT1( block, minColor, maxColor );
			EmitWord( ColorTo565( maxColor ) );
			EmitWord( ColorTo565( minColor ) );
			EmitColorIndices( block, minColor, maxColor );
		}
	}
	outputBytes = globalOutData - outBuf;
}

// ---------------------------------------------------------------------------------------------------

#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )

void GetMinMaxColors( const byte *colorBlock, byte *minColor, byte *maxColor ) {
	int i;
	byte inset[4];
	minColor[0] = minColor[1] = minColor[2] = minColor[3] = 255;
	maxColor[0] = maxColor[1] = maxColor[2] = maxColor[3] = 0;
	for ( i = 0; i < 16; i++ ) {
		if ( colorBlock[i*4+0] < minColor[0] ) { minColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] < minColor[1] ) { minColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] < minColor[2] ) { minColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+3] < minColor[3] ) { minColor[3] = colorBlock[i*4+3]; }
		if ( colorBlock[i*4+0] > maxColor[0] ) { maxColor[0] = colorBlock[i*4+0]; }
		if ( colorBlock[i*4+1] > maxColor[1] ) { maxColor[1] = colorBlock[i*4+1]; }
		if ( colorBlock[i*4+2] > maxColor[2] ) { maxColor[2] = colorBlock[i*4+2]; }
		if ( colorBlock[i*4+3] > maxColor[3] ) { maxColor[3] = colorBlock[i*4+3]; }
	}
	inset[0] = ( maxColor[0] - minColor[0] ) >> INSET_SHIFT;
	inset[1] = ( maxColor[1] - minColor[1] ) >> INSET_SHIFT;
	inset[2] = ( maxColor[2] - minColor[2] ) >> INSET_SHIFT;
	inset[3] = ( maxColor[3] - minColor[3] ) >> INSET_SHIFT;
	minColor[0] = ( minColor[0] + inset[0] <= 255 ) ? minColor[0] + inset[0] : 255;
	minColor[1] = ( minColor[1] + inset[1] <= 255 ) ? minColor[1] + inset[1] : 255;
	minColor[2] = ( minColor[2] + inset[2] <= 255 ) ? minColor[2] + inset[2] : 255;
	minColor[3] = ( minColor[3] + inset[3] <= 255 ) ? minColor[3] + inset[3] : 255;
	maxColor[0] = ( maxColor[0] >= inset[0] ) ? maxColor[0] - inset[0] : 0;
	maxColor[1] = ( maxColor[1] >= inset[1] ) ? maxColor[1] - inset[1] : 0;
	maxColor[2] = ( maxColor[2] >= inset[2] ) ? maxColor[2] - inset[2] : 0;
	maxColor[3] = ( maxColor[3] >= inset[3] ) ? maxColor[3] - inset[3] : 0;
}

void EmitAlphaIndices( const byte *colorBlock, const byte minAlpha, const byte maxAlpha ) {
	assert( maxAlpha >= minAlpha );
	byte indices[16];
	byte mid = ( maxAlpha - minAlpha ) / ( 2 * 7 );
	byte ab1 = minAlpha + mid;
	byte ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / 7 + mid;
	byte ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / 7 + mid;
	byte ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / 7 + mid;
	byte ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / 7 + mid;
	byte ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / 7 + mid;
	byte ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / 7 + mid;
	colorBlock += 3;
	for ( int i = 0; i < 16; i++ ) {
		byte a = colorBlock[i*4];
		int b1 = ( a <= ab1 );
		int b2 = ( a <= ab2 );
		int b3 = ( a <= ab3 );
		int b4 = ( a <= ab4 );
		int b5 = ( a <= ab5 );
		int b6 = ( a <= ab6 );
		int b7 = ( a <= ab7 );
		int index = ( b1 + b2 + b3 + b4 + b5 + b6 + b7 + 1 ) & 7;
		indices[i] = index ^ ( 2 > index );
	}
	EmitByte( (indices[ 0] >> 0) | (indices[ 1] << 3) | (indices[ 2] << 6) );
	EmitByte( (indices[ 2] >> 2) | (indices[ 3] << 1) | (indices[ 4] << 4) | (indices[ 5] << 7) );
	EmitByte( (indices[ 5] >> 1) | (indices[ 6] << 2) | (indices[ 7] << 5) );
	EmitByte( (indices[ 8] >> 0) | (indices[ 9] << 3) | (indices[10] << 6) );
	EmitByte( (indices[10] >> 2) | (indices[11] << 1) | (indices[12] << 4) | (indices[13] << 7) );
	EmitByte( (indices[13] >> 1) | (indices[14] << 2) | (indices[15] << 5) );
}

void CompressImageDXT5( const byte *inBuf, byte *outBuf, int width, int height, int &outputBytes ) {
	ALIGN16( byte block[64] );
	ALIGN16( byte minColor[4] );
	ALIGN16( byte maxColor[4] );
	globalOutData = outBuf;
	for ( int j = 0; j < height; j += 4, inBuf += width * 4*4 ) {
		for ( int i = 0; i < width; i += 4 ) {
			ExtractBlock( inBuf + i * 4, width, block );
			GetMinMaxColors( block, minColor, maxColor );
			EmitByte( maxColor[3] );
			EmitByte( minColor[3] );
			EmitAlphaIndices( block, minColor[3], maxColor[3] );
			EmitWord( ColorTo565( maxColor ) );
			EmitWord( ColorTo565( minColor ) );
			EmitColorIndices( block, minColor, maxColor );
		}
	}
	outputBytes = globalOutData - outBuf;
}

}
