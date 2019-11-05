/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef SHAPES_H
#define SHAPES_H

#include <pspgu.h>

typedef struct {
	float nx, ny, nz;      // Normal values
	float x, y, z;
} Vertex;

typedef struct {
	float u, v;
	float x, y, z;
} TexVertex;

typedef struct {
	u32 color;
	float x, y, z;
} ColorVertex;

extern Vertex __attribute__((aligned(16))) square[3*2];

extern Vertex __attribute__((aligned(16))) squarel[3*2];

extern ColorVertex __attribute__((aligned(16))) manche[3*2];

extern ColorVertex __attribute__((aligned(16))) mancheLines[2*5];

extern TexVertex __attribute__((aligned(16))) texSquare[3*2];

extern TexVertex __attribute__((aligned(16))) fireSquare[3*2];

extern TexVertex __attribute__((aligned(16))) texLine[3*2];

extern TexVertex __attribute__((aligned(16))) texLine2[3*2];

extern TexVertex __attribute__((aligned(16))) touche3D[114];

#endif
