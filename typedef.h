/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <vector>

/**
 * représente un pixel dans une image
 */
typedef u32 Color;

/**
 * représente une image
 */
typedef struct
{
	int textureWidth;  // the real width of data, 2^n with n>=0
	int textureHeight;  // the real height of data, 2^n with n>=0
	int imageWidth;  // the image width
	int imageHeight;
	Color* data;
} Image;

typedef struct MenuEntry{
	int type;
};

#define FOLDER_ENTRY 1
typedef struct FolderEntry{
	int type;
	int selection;
	char name[128];
	std::vector<MenuEntry*>* list;
};

#define MUSIC_ENTRY 2
typedef struct MusicEntry{
	int type;
	char dir[128];
	char name[128];
	char artist[128];
};

#endif
