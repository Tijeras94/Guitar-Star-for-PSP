/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "adhoc.h"
#include <stdlib.h>
#include <stdio.h>
#include <psputils.h>
#include <time.h>
#include <string.h>

PSP_MODULE_INFO("", 0, 1, 1);

#include "../exceptionHandler/utility/exception.c"

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common);
/* Callback thread */
int CallbackThread(SceSize args, void *argp);
/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void);

typedef struct Packet{
	char type;
};

#define STRING_PACKET 1
typedef struct StringPacket{
	char type;
	char txt[128];
};

int test;

char clientLastMsg[128],     serverLastMsg[128],     virtualClientLastMsg[128],
     clientLastSendMsg[128], serverLastSendMsg[128], virtualClientLastSendMsg[128];

void clientCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *data){
	Packet* p = (Packet*) data;
	if(p->type == STRING_PACKET){
		StringPacket* sp = (StringPacket*) data;
		sprintf(clientLastMsg,"%s",sp->txt);
	}
}

void serverCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *data){
	Packet* p = (Packet*) data;
	if(p->type == STRING_PACKET){
		StringPacket* sp = (StringPacket*) data;
		sprintf(serverLastMsg,"%s (from %s)",sp->txt, (char*) psp->hellodata);
	}
}

void testCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *data){
	test++;
}

int main() {
	SetupCallbacks();
	pspDebugScreenInit();
	
	test = 0;
	
	initExceptionHandler();
	
	sprintf(clientLastMsg,"");
	sprintf(serverLastMsg,"");
	sprintf(virtualClientLastMsg,"");
	sprintf(clientLastSendMsg,"");
	sprintf(serverLastSendMsg,"");
	sprintf(virtualClientLastSendMsg,"");
	
	SceKernelUtilsMt19937Context ctx;
	sceKernelUtilsMt19937Init(&ctx, time(NULL));
	int rand_val = sceKernelUtilsMt19937UInt(&ctx);
	
	char hello[128];
	sprintf(hello, "PSP-%i", rand_val%1000);
	char* product = "UOOOOOOOO";
	
	SceCtrlData pad, lastpad;
	
	bool gotoserver;
	bool serverstarted;
	
	pspDebugScreenClear();
	pspDebugScreenSetXY(0,1);
	pspDebugScreenPrintf(" matching wifi test for psp\n");
	pspDebugScreenPrintf(" hello data: %s\n",hello);
	pspDebugScreenPrintf(" product ID: %s\n",product);
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf(" X start client\n");
	pspDebugScreenPrintf(" [] start server\n");
	
	while(true){
		sceCtrlReadBufferPositive(&pad, 1);
		if(pad.Buttons & PSP_CTRL_CROSS){
			gotoserver = false;
			serverstarted = false;
			
			//init client
			int error;
			error = adhoc::init(product);
			if(error>=0) error = adhoc::client::start(strlen(hello)+1,(void*)hello);
			if(error<0){
				pspDebugScreenClear();
				pspDebugScreenSetXY(0,1);
				pspDebugScreenPrintf(" error in adhoc initialization            \n");
				sceKernelSleepThread();
			}
			adhoc::client::setcallback(clientCallback,NULL);
			
			break;
		}else if(pad.Buttons & PSP_CTRL_SQUARE){
			gotoserver = true;
			serverstarted = true;
			
			// init server;
			int error;
			error = adhoc::init(product);
			if(error>=0) error = adhoc::server::start(strlen(hello)+1,(void*)hello);
			if(error<0){
				pspDebugScreenClear();
				pspDebugScreenSetXY(0,1);
				pspDebugScreenPrintf(" error in adhoc initialization\n");
				sceKernelSleepThread();
			}
			
			adhoc::server::setClientConnectedCallback(testCallback,NULL);
		
			adhoc::server::setcallback(serverCallback,NULL);
			adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_AUTO_ACCEPT);
			
			//init virtual client
			error = adhoc::client::startVirtual(strlen(hello)+1,(void*)hello);
			if(error<0){
				pspDebugScreenClear();
				pspDebugScreenSetXY(0,1);
				pspDebugScreenPrintf(" error in adhoc initialization            \n");
				sceKernelSleepThread();
			}
			adhoc::client::setcallback(clientCallback,NULL);
			
			break;
		}
	}
	
	sceCtrlReadBufferPositive(&lastpad, 1);
	
	int index = 0;
	while(true){
		if(gotoserver){
			int indexSize;
			while(true){
				pspDebugScreenClear();
				pspDebugScreenSetXY(0,1);
				pspDebugScreenSetTextColor(0x00ffffff);
				pspDebugScreenPrintf(" test: %i\n",test);
				pspDebugScreenPrintf(" matching wifi test for psp   SERVER (R to switch to virtual client)\n");
				pspDebugScreenPrintf(" hello data: %s   product ID: %s\n",hello,product );
			
				switch ( adhoc::server::getAcceptationMode() ){
	    				case adhoc::SERVER_MODE_AUTO_ACCEPT :
	    					pspDebugScreenPrintf(" auto accept mode\n");
	    					break;
	    				case adhoc::SERVER_MODE_AUTO_REFUSE :
	    					pspDebugScreenPrintf(" auto refuse mode\n");
	    					break;
	    				case adhoc::SERVER_MODE_MANUAL_ACCEPATION :
	    					pspDebugScreenPrintf(" manual acceptation mode\n");
	    					break;
	    			}
	    			pspDebugScreenPrintf(" last message: %s\n",serverLastMsg);
	    			pspDebugScreenPrintf(" last send message: %s\n",serverLastSendMsg);
				pspDebugScreenPrintf("\n");
			
				indexSize = 3;
			
				pspDebugScreenSetTextColor( (0==index) ? 0x0000ff00 : 0x00ffffff );
				pspDebugScreenPrintf(" switch in auto accept mode\n");
				pspDebugScreenSetTextColor( (1==index) ? 0x0000ff00 : 0x00ffffff );
				pspDebugScreenPrintf(" switch in auto refuse mode\n");
				pspDebugScreenSetTextColor( (2==index) ? 0x0000ff00 : 0x00ffffff );
				pspDebugScreenPrintf(" switch in manual acceptation mode\n");
			
				pspDebugScreenPrintf("\n");
			
				pspDebugScreenSetTextColor(0x00ffffff);
				pspDebugScreenPrintf(" connected psp list:\n");
				// ne marche pas avec le client virtuel
				/*for (std::vector<adhoc::PSPelement*>::iterator it = adhoc::server::getClientList().begin();
				     it!=adhoc::server::getClientList().end(); ++it) {
					adhoc::PSPelement* psp = *it;
					pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
					pspDebugScreenPrintf("   %s\n",(char*) psp->hellodata);
					indexSize++;
				}*/
				for(int i = 0; i < adhoc::server::getClientList().size(); i++){
					pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
					pspDebugScreenPrintf("   %s\n",(char*) adhoc::server::getClientList().at(i)->hellodata);
					indexSize++;
				}
				pspDebugScreenPrintf("\n");
			
				pspDebugScreenSetTextColor(0x00ffffff);
				pspDebugScreenPrintf(" request connection psp list:\n");
				// ne marche pas avec le client virtuel
				/*for (std::vector<adhoc::PSPelement*>::iterator it = adhoc::server::getClientRequestList().begin();
				     it!=adhoc::server::getClientRequestList().end(); ++it) {
					adhoc::PSPelement* psp = *it;
					pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
					pspDebugScreenPrintf("   %s\n",(char*) psp->hellodata);
					indexSize++;
				}*/
				for(int i = 0; i < adhoc::server::getClientRequestList().size(); i++){
					pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
					pspDebugScreenPrintf("   %s\n",(char*) adhoc::server::getClientRequestList().at(i)->hellodata);
					indexSize++;
				}
				pspDebugScreenSetTextColor(0x00ffffff);
				pspDebugScreenPrintf("\n");
			
				if(index < 3){
					pspDebugScreenPrintf(" X to switch mode\n");
				}else if(index-3 < adhoc::server::getClientList().size()){
					pspDebugScreenPrintf(" X to disconnect client\n");
					pspDebugScreenPrintf(" [] to send random data\n");
				}else{
					pspDebugScreenPrintf(" X to accept connection\n");
					pspDebugScreenPrintf(" O to refuse connection\n");
				}
			
				sceCtrlReadBufferPositive(&pad, 1);
				if(pad.Buttons != lastpad.Buttons){
					lastpad = pad;
			
					if(pad.Buttons & PSP_CTRL_UP){
						if(index>0)index--;
					}
					if(pad.Buttons & PSP_CTRL_DOWN){
						if(index<indexSize-1)index++;
					}
					if(pad.Buttons & PSP_CTRL_CROSS){
						if(index == 0){
							adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_AUTO_ACCEPT);
						}else if(index == 1){
							adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_AUTO_REFUSE);
						}else if(index == 2){
							adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_MANUAL_ACCEPATION);
						}else if(index-3 < adhoc::server::getClientList().size()){
							adhoc::server::disconnect(adhoc::server::getClientList().at(index-3));
						}else{
							adhoc::server::acceptConnection(adhoc::server::getClientRequestList().at( index-( 3 + adhoc::server::getClientList().size() ) ));
						}
					}
					if(pad.Buttons & PSP_CTRL_CIRCLE){
						if(index-3 >= adhoc::server::getClientList().size()){
							adhoc::server::rejectConnection(adhoc::server::getClientRequestList().at( index-( 3 + adhoc::server::getClientList().size() ) ));
						}
					}
					if(pad.Buttons & PSP_CTRL_SQUARE){
						if(index > 2 && index-3 < adhoc::server::getClientList().size()){
							adhoc::PSPelement* target = adhoc::server::getClientList().at(index-3);
						
							rand_val = sceKernelUtilsMt19937UInt(&ctx);
							sprintf(serverLastSendMsg, "random number: %i", rand_val%1000);
						
							StringPacket data;
							data.type = STRING_PACKET;
							sprintf(data.txt, "%s", serverLastSendMsg);
						
							adhoc::server::send(target, sizeof(StringPacket), &data);
						
							sprintf(serverLastSendMsg, "%s (to %s)", serverLastSendMsg, (char*) target->hellodata);
						}
					}
					if(pad.Buttons & PSP_CTRL_RTRIGGER){
						if (serverstarted){
							gotoserver = false;
							break;
						}
					}
				}
			
				sceKernelDelayThread(100000);
			}
		}else{
			int indexSize;
			while(true){
				pspDebugScreenClear();
				pspDebugScreenSetXY(0,1);
				pspDebugScreenSetTextColor(0x00ffffff);
				if (serverstarted){
					pspDebugScreenPrintf(" matching wifi test for psp   VRTUAL CLIENT (R to switch to server)\n");
				}else{
					pspDebugScreenPrintf(" matching wifi test for psp   CLIENT\n");
				}
				pspDebugScreenPrintf(" hello data: %s   product ID: %s\n",hello,product );
	    			pspDebugScreenPrintf(" last message: %s\n",clientLastMsg);
	    			pspDebugScreenPrintf(" last send message: %s\n",clientLastSendMsg);
				pspDebugScreenPrintf("\n");
			
				if(adhoc::client::connectionState() == adhoc::CLIENT_STATE_DISCONNECTED){
					pspDebugScreenPrintf(" selectServer:  X to choice\n" );
					indexSize = 0;
					// ne marche pas avec le client virtuel
					/*for (std::vector<adhoc::PSPelement*>::iterator it = adhoc::client::getServerList().begin();
					     it!=adhoc::client::getServerList().end(); ++it) {
						adhoc::PSPelement* psp = *it;
						pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
						pspDebugScreenPrintf("   %s\n",(char*) psp->hellodata);
						indexSize++;
					}*/
					for(int i = 0; i < adhoc::client::getServerList().size(); i++){
						pspDebugScreenSetTextColor( (indexSize==index) ? 0x0000ff00 : 0x00ffffff );
						pspDebugScreenPrintf("   %s\n",(char*) adhoc::client::getServerList().at(i)->hellodata);
						indexSize++;
					}
			
					sceCtrlReadBufferPositive(&pad, 1);
					if(pad.Buttons != lastpad.Buttons){
						lastpad = pad;
				
						if(pad.Buttons & PSP_CTRL_UP){
							if(index>0)index--;
						}
						if(pad.Buttons & PSP_CTRL_DOWN){
							if(index<indexSize-1)index++;
						}
						if(pad.Buttons & PSP_CTRL_CROSS){
							adhoc::client::requestConnection(adhoc::client::getServerList().at(index));
						}
						if(pad.Buttons & PSP_CTRL_RTRIGGER){
							if (serverstarted){
								gotoserver = true;
								break;
							}
						}
					}
				}else if(adhoc::client::connectionState() == adhoc::CLIENT_STATE_CONNECTING){
					pspDebugScreenPrintf(" is connecting to %s  /\\ to cancel\n",adhoc::client::getTargetPSP()->hellodata);
				
					sceCtrlReadBufferPositive(&pad, 1);
					if(pad.Buttons != lastpad.Buttons){
						lastpad = pad;
				
						if(pad.Buttons & PSP_CTRL_TRIANGLE){
							adhoc::client::cancelRequestedConnection();
						}
						if(pad.Buttons & PSP_CTRL_RTRIGGER){
							if (serverstarted){
								gotoserver = true;
								break;
							}
						}
					}
				}else if(adhoc::client::connectionState() == adhoc::CLIENT_STATE_CONNECTED){
					pspDebugScreenPrintf(" connected to %s  /\\ to disconnect\n",adhoc::client::getTargetPSP()->hellodata);
					pspDebugScreenPrintf(" [] to send random data\n");
				
					sceCtrlReadBufferPositive(&pad, 1);
					if(pad.Buttons != lastpad.Buttons){
						lastpad = pad;
				
						if(pad.Buttons & PSP_CTRL_TRIANGLE){
							adhoc::client::disconnect();
						}
					
						if(pad.Buttons & PSP_CTRL_SQUARE){
							rand_val = sceKernelUtilsMt19937UInt(&ctx);
							sprintf(clientLastSendMsg, "random number: %i", rand_val%1000);
						
							StringPacket data;
							data.type = STRING_PACKET;
							sprintf(data.txt, "%s", clientLastSendMsg);
						
							adhoc::client::send(sizeof(StringPacket), &data);
						}
						if(pad.Buttons & PSP_CTRL_RTRIGGER){
							if (serverstarted){
								gotoserver = true;
								break;
							}
						}
					}
				}
			
				sceKernelDelayThread(100000);
			}
		}
	}
	
	sceKernelSleepThread();
	
	return 0;
}

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
          sceKernelExitGame();
          return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
          int cbid;

          cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
          sceKernelRegisterExitCallback(cbid);

          sceKernelSleepThreadCB();

          return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
          int thid = 0;

          thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
          if(thid >= 0) {
                    sceKernelStartThread(thid, 0, 0);
          }

          return thid;
}
