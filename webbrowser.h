/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef WEB_BROWSER_H
#define WEB_BROWSER_H

#define BROWSER_MEMORY (10*1024*1024) 

#include <pspsdk.h>
#include <pspuser.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <psputility.h>
#include <psputility_netmodules.h>
#include <psputility_htmlviewer.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <psphttp.h>
#include <pspssl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/unistd.h>

namespace webBrowser{
	
	int init();
	
	void uninit();
	
	int startBrowser(char* url);
	
	bool draw();
}

#endif
