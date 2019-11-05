/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef CALLBACK_H
#define CALLBACK_H

#include <pspkerneltypes.h>
#include <psploadexec.h>
#include <pspthreadman.h>

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common);

/* Callback thread */
int CallbackThread(SceSize args, void *argp);

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void);

#endif
