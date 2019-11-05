/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "webbrowser.h"


namespace webBrowser{
	
	SceUID vpl;
	void* memvlpaddr;
	pspUtilityHtmlViewerParam params;
	
	int init(){
		int res;

		if(sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON) < 0) return -1;
		if(sceUtilityLoadNetModule(PSP_NET_MODULE_INET) < 0) return -2;
		if(sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI) < 0) return -3;
		if(sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP) < 0) return -4;
		if(sceUtilityLoadNetModule(PSP_NET_MODULE_HTTP) < 0) return -5;
		if(sceUtilityLoadNetModule(PSP_NET_MODULE_SSL) < 0) return -6;

		res = sceNetInit(0x20000, 0x2A, 0, 0x2A, 0);
	
		if (res < 0) return res;

		res = sceNetInetInit();
	
		if (res < 0) 
		{
			uninit();
			return res;				
		}

		res = sceNetResolverInit();
	
		if (res < 0) 
		{
			uninit();
			return res;		
		}

		res = sceNetApctlInit(0x1800, 0x30);
	
		if (res < 0) 
		{
			uninit();
			return res;		
		}

		res = sceSslInit(0x28000);
	
		if (res < 0) 
		{
			uninit();
			return res;
		}

		res = sceHttpInit(0x25800);
	
		if (res < 0) 
		{
			uninit();
			return res;
		}

		res = sceHttpsInit(0, 0, 0, 0);
		if (res < 0) 
		{
			uninit();
			return res;
		}

		res = sceHttpsLoadDefaultCert(0, 0);
	
		if (res < 0) 
		{
			uninit();
			return res;
		}

		res = sceHttpLoadSystemCookie();
	
		if (res < 0) 
		{
			uninit();
			return res;
		}
		
		vpl = sceKernelCreateVpl("BrowserVpl", PSP_MEMORY_PARTITION_USER, 0, BROWSER_MEMORY + 256, NULL);
	
		if (vpl < 0) 
			//throwError(6000, "Error 0x%08X creating vpl.\n", vpl);
			return -7;
			
		res = sceKernelAllocateVpl(vpl, BROWSER_MEMORY, &memvlpaddr, NULL);
		
		if (res < 0) 
			//throwError(6000, "Error 0x%08X allocating browser memory.\n", res);
			return -8;
		
		return 0;
	}
	
	void uninit(){
		sceHttpSaveSystemCookie();
		sceHttpsEnd();
		sceHttpEnd();
		sceSslEnd();
		sceNetApctlTerm();
		sceNetInetTerm();
		sceNetTerm();
	
		sceUtilityUnloadNetModule(PSP_NET_MODULE_SSL);
		sceUtilityUnloadNetModule(PSP_NET_MODULE_HTTP);
		sceUtilityUnloadNetModule(PSP_NET_MODULE_PARSEHTTP);
		sceUtilityUnloadNetModule(PSP_NET_MODULE_PARSEURI);
		sceUtilityUnloadNetModule(PSP_NET_MODULE_INET);
		sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
	}
	
	int startBrowser(char* url){
		int res;
	
		/*vpl = sceKernelCreateVpl("BrowserVpl", PSP_MEMORY_PARTITION_USER, 0, BROWSER_MEMORY + 256, NULL);
	
		if (vpl < 0) 
			//throwError(6000, "Error 0x%08X creating vpl.\n", vpl);
			return -1;*/

		memset(&params, 0, sizeof(pspUtilityHtmlViewerParam));
	
		params.base.size = sizeof(pspUtilityHtmlViewerParam);
	
		sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
		sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
	
		params.base.graphicsThread = 17;
		params.base.accessThread = 19;
		params.base.fontThread = 18;
		params.base.soundThread = 16;	
		params.memsize = BROWSER_MEMORY;
		params.memaddr = memvlpaddr;
		params.initialurl = url;
		params.numtabs = 1;
		params.cookiemode = PSP_UTILITY_HTMLVIEWER_COOKIEMODE_DEFAULT;
		params.homeurl = url;
		params.textsize = PSP_UTILITY_HTMLVIEWER_TEXTSIZE_NORMAL;
		params.displaymode = PSP_UTILITY_HTMLVIEWER_DISPLAYMODE_SMART_FIT;
		params.options = PSP_UTILITY_HTMLVIEWER_DISABLE_STARTUP_LIMITS|PSP_UTILITY_HTMLVIEWER_DISABLE_LRTRIGGER|PSP_UTILITY_HTMLVIEWER_DISABLE_CURSOR|PSP_UTILITY_HTMLVIEWER_DISABLE_EXIT_DIALOG ;
		params.interfacemode = PSP_UTILITY_HTMLVIEWER_INTERFACEMODE_NONE;
		params.connectmode = PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ALL;
	
		// Note the lack of 'ms0:' on the paths	
		params.dldirname = "/PSP/PHOTO";
	
		/*res = sceKernelAllocateVpl(vpl, params.memsize, &params.memaddr, NULL);
	
		if (res < 0) 
			//throwError(6000, "Error 0x%08X allocating browser memory.\n", res);
			return -2;*/

		res = sceUtilityHtmlViewerInitStart(&params);
	
		if (res < 0)
			//throwError(6000, "Error 0x%08X initing browser.\n", res);
			return -3;
		return 0;
	}
	
	bool draw(){
		switch (sceUtilityHtmlViewerGetStatus()){	
			case PSP_UTILITY_DIALOG_VISIBLE:			
				sceUtilityHtmlViewerUpdate(1);
			break;
	
			case PSP_UTILITY_DIALOG_QUIT:		
				sceUtilityHtmlViewerShutdownStart();		
			break;
	
			case PSP_UTILITY_DIALOG_NONE:
				//sceKernelFreeVpl(vpl, params.memaddr);
				//sceKernelDeleteVpl(vpl);
				return false;
			break;
		
			default:
				break;
		}

		return true;
	}
}
