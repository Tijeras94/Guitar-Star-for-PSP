/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef DATA_H
#define DATA_H

#include "intraFont.h"
#include "gfunctions.h"
#include <string.h>
#include <sys/unistd.h>
#include "typedef.h"
#include "config.h"

//textures
extern  Image* touchesImg[5];
extern  Image* notesImg[5];
extern  Image* flammeImg[5];
extern  Image* noteMiss;
extern  Image* lineImg;
extern  Image* etoileImg;
extern  Image* publicImg;
extern  Image* fond;
extern  Image* logo;
extern  Image* touchesListImg;

extern  intraFont* font;


extern unsigned int color0,
		    color1,
		    color2,
		    color3,
		    color4,
		    hightColor0,
		    hightColor1,
		    hightColor2,
		    hightColor3,
		    hightColor4,
		    missedColor;

//le chemin ou se situe le jeu
extern  char gameDir[256];

/**
 *charge les données partagé dans le jeu
 *
 *@returns 0 ssi le chargement est complet
 */
extern int loadData();

/**
 *décharge les données partagé dans le jeu
 */
extern void unloadData();
#endif
