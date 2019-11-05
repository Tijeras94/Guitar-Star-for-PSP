/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef GFUNCTIONS_H
#define GFUNCTIONS_H

#define DEFAUT_FONT_SIZE 0.8f
#define FONT_HIGHLIGHT_ZOOM 1.5f
#define FONT_HEIGHT 18.0f
#define FONT_SPACE 2.0f
#define TEXT_MOTION_VELOCITY 0.8f

#include "midiloader.h"
#include <png.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <ft2build.h>
#include "shapes.h"
#include "intraFont.h"
#include <pspdisplay.h>
#include <pspgum.h>
#include "data.h"
#include "typedef.h"

/**
 * initialise le rendu graphique
 */
void InitGU( void );

/**
 * active le rendu des textures
 */
void texMode();

/**
 * active le rendu en perspective
 */
void perspectiveView();

/**
 * active le rendu en perspective
 */
void orthoView();
 
/**
 * initialise la projection
 */
void SetupProjection( void );

/**
 * demarre le rendu graphique
 */
void startRender();

/**
 * fini le rendu graphique
 */
void stopRender();

/**
 * affiche une image a l'écran
 * 
 * @param image l'image a afficher
 * @param x la position x de l'image a l'écran
 * @param y la position y de l'image a l'écran
 * @param width la largeur de l'image
 * @param height la hauteur de l'image
 * @param color la couleur a appliquer a l'image
 */
void imageRender(Image* image, int x, int y, int width, int height, unsigned int color);

/**
 * affiche une image a l'écran
 * 
 * @param image l'image a afficher
 * @param x la position x de l'image a l'écran
 * @param y la position y de l'image a l'écran
 * @param width la largeur de l'image
 * @param height la hauteur de l'image
 */
void imageRender(Image* image, int x, int y, int width, int height);

/**
 * affiche une image a l'écran
 * 
 * @param image l'image a afficher
 * @param x la position x de l'image a l'écran
 * @param y la position y de l'image a l'écran
 */
void imageRender(Image* image, int x, int y);

/**
 * affiche une image a l'écran
 * 
 * @param image l'image a afficher
 * @param x la position x de l'image a l'écran
 * @param y la position y de l'image a l'écran
 * @param color la couleur a appliquer a l'image
 */
void imageRender(Image* image, int x, int y, unsigned int color);

/**
 * affiche un texte à l'écran
 * 
 * @param font la font a utiliser
 * @param x la position x du texte a l'écran
 * @param y la position y du texte a l'écran
 * @param hightLight vrai si le texte doit etre en surbriance
 * @param text le texte a afficher
 */
void fontPrint(intraFont *font, float x, float y, bool hightLight, const char *text);

/**
 * affiche un texte à l'écran
 * 
 * @param font la font a utiliser
 * @param x la position x du texte a l'écran
 * @param y la position y du texte a l'écran
 * @param hightLight vrai si le texte doit etre en surbriance
 * @param text le texte a afficher
 */
void fontPrintf(intraFont *font, float x, float y, bool hightLight, const char *text, ...);

/**
 * affiche un texte à l'écran avec retour du chariot
 * 
 * @param font la font a utiliser
 * @param x la position x du texte a l'écran
 * @param y la position y du texte a l'écran
 * @param xmax la longeur maximale d'une ligne
 * @param yjump la hauteur d'un saut de ligne
 * @param hightLight vrai si le texte doit etre en surbriance
 * @param text le texte a afficher
 */
void fontPrintRC(intraFont *font, float x, float y,float xmax,float yjump, bool hightLight, const char *text);

/**
 * affiche un texte à l'écran avec retour du chariot dans la configuration standard
 * la police de data.h est utilisé et le texte est affiché sur tout l'écran
 * 
 * @param text le texte a afficher
 */
void printRC(const char *text);


/**
 * affiche un texte à l'écran avec retour du chariot dans la configuration standard
 * la police de data.h est utilisé et le texte est affiché sur tout l'écran
 * 
 * @param text le texte a afficher
 */
void printRCf(const char *text, ...);

void setFontZoom(float zoom);

void printMenu(std::vector<char*> menu,int index);

void printMenuF(FolderEntry* menu);

void resetMotionMenu();

void motionUp();

void motionDown();

/**
 * charge une image au format png
 * 
 * @param filename le chemin vers l'image a charger
 */
Image* loadImage(const char* filename);

/**
 * supprime une image chargée
 * 
 * @param l'image a decharger
 */
void freeImage(Image* image);

/**
 * applique une texture
 * 
 * @param image l'image a appliquer au contexte graphique
 */
void bindTexure(Image* image);

void swizzle_fast(u8* out, const u8* in, unsigned int width, unsigned int height);
#ifdef ANAGLYPH
void copyImage();

void putRed();

void clearRender();
#endif
#endif
