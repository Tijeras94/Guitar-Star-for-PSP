/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "framebuffer.h"

Color* g_vram_base = (Color*) (0x40000000 | 0x04000000);
