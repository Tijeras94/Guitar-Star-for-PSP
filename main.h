/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef MAIN_H
#define MAIN_H

//SDK INCLUDES
#include "midiloader.h"
#include <time.h>
#include <psptypes.h>
#include <psprtc.h>
#include <pspwlan.h>
#include "data.h"
#include "gfunctions.h"
#include "callback.h"
#include "player.h"
#include "packets.h"
#include "config.h"
#include <map>
#include "pad.h"
#ifdef AD_HOC
#include "matchingWifi/adhoc.h"
#include "server.h"
#endif

#define GU_VERTEX_TEX_COLOR_TRANSFORM ( GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D )

#define productID "GUITAR000"

/**
 * quitte le mode multi, termine les routines reseau, et renvoi le jeu au menu principal
 */
void quitMulti();

/**
 * quitte le mode multi avec envoi de sendQuitMessage, l'execution est asynchrone pour pouvoir etre appelé par le player
 */
void quitMultiAsynchronously();

/**
 * routine d'execution des packets
 * 
 * un pointer vers un packet comme defini dans adhoc.h
 */
void proccedPacket(char *buff);

/**
 * thread qui s'occupe de receptioner les messages adhoc et les envoyer vers proccedPacket()
 * 
 * @param args argument standard d'un thread
 * @param *argp argument standard d'un thread
 */
int recieveThread(SceSize args, void *argp);

/**
 * point de départ du programme
 */
int main();

#endif
