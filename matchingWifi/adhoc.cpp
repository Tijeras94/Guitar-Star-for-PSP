/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "adhoc.h"
#include <pspnet_adhocctl.h>
#include <string.h>
#include <stdio.h>

#define STD_MSG 0
#define DISCONNECT_MSG 1

#define MAX_PEERS 0xf
#define PORT 0x22b
#define BUFFER_SIZE 0x800

namespace adhoc{
	int matchingID;//id retournée par sceNetAdhocMatchingCreate
	namespace server{
		std::vector<PSPelement*> requestList;// la liste des psp attente de validation de la connection
		std::vector<PSPelement*> clientList;// la liste des psp connecté
		AdhocDataCallback dataCallback;// callback appelé en cas de reception d'un message 'DATA'
		void* dataCallbackData;
		AdhocDataCallback onClientConnectingCallback, onClientConnectedCallback, onClientDisconectCallback;
		void* onClientConnectingData;
		void* onClientConnectedData;
		void* onClientDisconectData;
		ServerMode acceptationMode; // mode d'acception du serveur
		char hellodata[BUFFER_HELLODATA_SIZE]; //données envoyé lors du contact avec un client
		int hellolength; //taille du hellodata
		
		bool started = false;
	}
	namespace client{
		bool started = false; //vrai ssi le client réel est lancé
	}
	
	int init(char* productID){
		int error = 0;
		
		if(sceKernelDevkitVersion() < 0x02000010) return -1;
		
		error = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
		if(error<0) return error;
		
		error = sceUtilityLoadNetModule(PSP_NET_MODULE_ADHOC);
		if(error<0) return error;
		
		error = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000);
		if(error<0) return error;
		
		error = sceNetAdhocInit();
		if(error<0) return error;
		
		struct productStruct product;
		sprintf((char *)product.product, productID);
		product.unknown = 0;
		error = sceNetAdhocctlInit(0x2000, 0x20, &product);
		if(error<0) return error;
		return 0;
	}
	
	/**
	/* fonction interne
	/* arrete le serveur ou le client
	/*
	/* @return 0 en cas de succes sinon un code d'erreur
	 */
	int stop(){
		int error = 0;
		
		error = sceNetAdhocMatchingStop(matchingID);
		if(error<0) return error;
		error = sceNetAdhocMatchingDelete(matchingID);
		if(error<0) return error;
		error = sceNetAdhocMatchingTerm();
		if(error<0) return error;
		error = sceNetAdhocctlDisconnect();
		return error;
	}
	
	/**
	/* fonction interne
	/* demmare la librairie adhoc matching en mode serveur ou client
	/*
	/* @param helloDataLength la longueur des donneées 'hello'
	/* @param helloData un pointeur vers le bloc de données 'hello'
	/* @param callback le callback a utiliser
	/* @param matchingMode le mode a utiliser (PSP_ADHOC_MATCHING_MODE_CLIENT pour le client ou PSP_ADHOC_MATCHING_MODE_HOST pour le serveur)
	/* @return 0 en cas de succes sinon un code d'erreur
	 */
	int start(int helloDataLength, void* helloData, pspAdhocMatchingCallback callback, int matchingMode){
		int error = 0;
	
		error = sceNetAdhocctlConnect("");
		if(error<0) return error;
	
		int state = 0;
		do{
			error = sceNetAdhocctlGetState(&state);
			sceKernelDelayThread(10000);
		}while (state == 0);
		if(error<0) return error;

		error = sceNetAdhocMatchingInit(0x20000);
		if(error<0) return error;
	
		matchingID = sceNetAdhocMatchingCreate(matchingMode, MAX_PEERS, PORT, BUFFER_SIZE, HELLO_DELAY, PING_DELAY, 3, MESSAGE_DELAY, callback);
		return sceNetAdhocMatchingStart(matchingID,0x10,0x2000,0x10,0x2000,helloDataLength,helloData);
	}
	
	/**
	/* fonction interne
	/* cherche une psp dans une liste a partir de son adresse mac
	/* 
	/* @param mac l'adresse mec a chercher
	/* @param list la liste dans laquelle chercher
	/* @return un pointeur vers le PSPelement correspandant ou 0 si la psp n'a pas été trouvée
	 */
	PSPelement* findPSPinList(unsigned char* mac, std::vector<PSPelement*> list){
		for (std::vector<PSPelement*>::iterator it = list.begin(); it!=list.end(); ++it) {
			PSPelement* psp = *it;
			if(memcmp(psp->mac, mac, 6) == 0) return psp;	
		}
		return 0;
	}
	
	bool nullMac(unsigned char* mac){
		for(int i = 0; i < 6; i++){
			if(mac[i] != 0) return false;
		}
		return true;
	}
	
	namespace client{
		std::vector<PSPelement*> psplist; //liste des serveurs
		ClientConnectionState state; //etat du client
		PSPelement* targetPSP; //serveur sur lequel le client est connecté (sinon 0)
		AdhocDataCallback dataCallback; //callback appelé en cas de reception d'un message 'DATA'
		void* dataCallbackData;
		char hellodata[BUFFER_HELLODATA_SIZE]; //données envoyé lors du contact avec un serveur
		int hellolength; //taille du hellodata
		// ------------------------ VIRTUAL ------------------------
		bool virtualClient;
		// ---------------------- END VIRTUAL ----------------------
		
		//fonction appellée par la librairie adhoc matching a la récéption d'un message
		void matchingCallback(int matchingid, int event, unsigned char *mac, int optlen, void *optdata);
		
		int start(int helloDataLength, void* helloData){
		// ------------------------ VIRTUAL ------------------------
			if(started || adhoc::server::started || virtualClient) return -1; 
			virtualClient = false;
		// ---------------------- END VIRTUAL ----------------------
			targetPSP = 0;
			dataCallback = 0;
			state = CLIENT_STATE_DISCONNECTED;
			started = true;
			memcpy(hellodata, helloData, helloDataLength);
			hellolength = helloDataLength;
			return adhoc::start(helloDataLength, helloData, matchingCallback, PSP_ADHOC_MATCHING_MODE_CLIENT);
		}
		
		// ------------------------ VIRTUAL ------------------------
		int startVirtual(int helloDataLength, void* helloData){
			if(started || virtualClient) return -1; 
			targetPSP = 0;
			dataCallback = 0;
			state = CLIENT_STATE_DISCONNECTED;
			virtualClient = true;
			memcpy(hellodata, helloData, helloDataLength);
			hellolength = helloDataLength;
			
			if(adhoc::server::started){
				PSPelement* psp = new PSPelement();
				psp->hellodatasize = adhoc::server::hellolength;
				memcpy(psp->hellodata, adhoc::server::hellodata, adhoc::server::hellolength);
				memset(psp->mac, 0, 6);
				psp->userData = NULL;
				psp->userDataLen = 0;
				
				psplist.push_back(psp);
			}
			
			return 0;
		}
		// ---------------------- END VIRTUAL ----------------------
		
		int stop(){
			psplist.clear();
		// ------------------------ VIRTUAL ------------------------
			if(virtualClient){
				for (std::vector<PSPelement*>::iterator it = adhoc::server::clientList.begin(); it!=adhoc::server::clientList.end(); ++it) {
					PSPelement* psp = *it;
					if(adhoc::nullMac(psp->mac)){
						adhoc::server::clientList.erase(it);
						return true;
					}
				}
				for (std::vector<PSPelement*>::iterator it = adhoc::server::requestList.begin(); it!=adhoc::server::requestList.end(); ++it) {
					PSPelement* psp = *it;
					if(adhoc::nullMac(psp->mac)){
						adhoc::server::requestList.erase(it);
						return true;
					}
				}
				virtualClient = false;
				return 0;
			}
		// ---------------------- END VIRTUAL ----------------------
			started = false;
			return adhoc::stop();
		}
		
		void matchingCallback(int matchingid, int event, unsigned char *mac, int optlen, void *optdata){
			if(event == PSP_ADHOC_MATCHING_EVENT_HELLO){
				if(adhoc::findPSPinList(mac, psplist)) return;
				PSPelement* psp = new PSPelement();
				memcpy(psp->mac, mac, 6);
				psp->hellodatasize = optlen;
				memcpy(psp->hellodata, optdata, optlen);
				psp->userData = NULL;
				psp->userDataLen = 0;
				psplist.push_back(psp);
			}else if(event == PSP_ADHOC_MATCHING_EVENT_JOIN){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_REJECT){
				PSPelement* psp = adhoc::findPSPinList(mac, psplist);
				if(psp) cancelRequestedConnection();
			}else if(event == PSP_ADHOC_MATCHING_EVENT_COMPLETE){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_CANCEL){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_ACCEPT){
				state = CLIENT_STATE_CONNECTED;
			}else if(event == PSP_ADHOC_MATCHING_EVENT_DATA){
				if(memcmp(mac,targetPSP->mac,6)) return;
				char* buff = (char*) optdata;
				if(dataCallback && (buff[0] == STD_MSG)){
					dataCallback(dataCallbackData, targetPSP, optlen-1, &buff[1]);
				}else if(buff[0] == DISCONNECT_MSG){
					disconnect();
				}
			}else if(event == PSP_ADHOC_MATCHING_EVENT_DATA_CONFIRM){
			}else if((event == PSP_ADHOC_MATCHING_EVENT_TIMEOUT) || (event == PSP_ADHOC_MATCHING_EVENT_ERROR) || (event == PSP_ADHOC_MATCHING_EVENT_DISCONNECT)){
				if(state == CLIENT_STATE_CONNECTED){
					if(memcmp(targetPSP->mac, mac, 6) == 0){
						sceNetAdhocMatchingCancelTarget(adhoc::matchingID, mac);
						state = CLIENT_STATE_DISCONNECTED;
						targetPSP = 0;
					}
				}
				for (std::vector<PSPelement*>::iterator it = psplist.begin(); it!=psplist.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						psplist.erase(it);
						break;
					}
				}
			}
		}
		
		std::vector<PSPelement*> getServerList(){
			return psplist;
		}
	
		bool requestConnection(PSPelement* psp){
			if(state != CLIENT_STATE_DISCONNECTED)return false;
			state = CLIENT_STATE_CONNECTING;
			targetPSP = psp;
			
		// ------------------------ VIRTUAL ------------------------
			if(virtualClient){
				if(nullMac(psp->mac)){
					PSPelement* cpsp = adhoc::findPSPinList(psp->mac, adhoc::server::clientList);
					if(!cpsp)    cpsp = adhoc::findPSPinList(psp->mac, adhoc::server::requestList);
					if(cpsp) return false;
					
					cpsp = new PSPelement();
					memset(cpsp->mac, 0, 6);
					cpsp->hellodatasize = hellolength;
					memcpy(cpsp->hellodata, hellodata, hellolength);
					psp->userData = NULL;
					psp->userDataLen = 0;
					
					if(adhoc::server::acceptationMode == SERVER_MODE_AUTO_ACCEPT){
						adhoc::server::clientList.push_back(cpsp);
						state = CLIENT_STATE_CONNECTED;
						if(adhoc::server::onClientConnectedCallback)adhoc::server::onClientConnectedCallback(adhoc::server::onClientConnectedData, cpsp,0,NULL);
					}else if(adhoc::server::acceptationMode == SERVER_MODE_AUTO_REFUSE){
						state = CLIENT_STATE_DISCONNECTED;
						delete cpsp;
					}else {
						adhoc::server::requestList.push_back(cpsp);
						if(adhoc::server::onClientConnectingCallback)adhoc::server::onClientConnectingCallback(adhoc::server::onClientConnectingData, cpsp,0,NULL);
					}
					return true;
				}
				state = CLIENT_STATE_DISCONNECTED;
				return false;
			}
		// ---------------------- END VIRTUAL ----------------------
		
			return 0 == sceNetAdhocMatchingSelectTarget(adhoc::matchingID, psp->mac, hellolength,hellodata);
		}
	
		bool cancelRequestedConnection(){
			if(state != CLIENT_STATE_CONNECTING)return false;
			state = CLIENT_STATE_DISCONNECTED;
			
		// ------------------------ VIRTUAL ------------------------
			if(virtualClient){
				for (std::vector<PSPelement*>::iterator it = adhoc::server::requestList.begin(); it!=adhoc::server::requestList.end(); ++it) {
					PSPelement* psp = *it;
					if(adhoc::nullMac(psp->mac)){
						adhoc::server::requestList.erase(it);
						return true;
					}
				}
				return false;
			}
		// ---------------------- END VIRTUAL ----------------------
		
			return 0 == sceNetAdhocMatchingCancelTarget(adhoc::matchingID, targetPSP->mac);
		}
		
		bool disconnect(){
			if(state != CLIENT_STATE_CONNECTED)return false;
			state = CLIENT_STATE_DISCONNECTED;
			
		// ------------------------ VIRTUAL ------------------------
			if(virtualClient){
				for (std::vector<PSPelement*>::iterator it = adhoc::server::clientList.begin(); it!=adhoc::server::clientList.end(); ++it) {
					PSPelement* psp = *it;
					if(adhoc::nullMac(psp->mac)){
						adhoc::server::clientList.erase(it);
						return true;
					}
				}
				return false;
			}
		// ---------------------- END VIRTUAL ----------------------
			
			char msg = DISCONNECT_MSG;
			if(sceNetAdhocMatchingSendData( adhoc::matchingID, targetPSP->mac, 1,(void*) &msg )) return false;
			
			bool result = ( 0 == sceNetAdhocMatchingCancelTarget(adhoc::matchingID, targetPSP->mac) );
			targetPSP = 0;
			return result;
		}
		
		int send(int datalen, void *data){
			if(state != CLIENT_STATE_CONNECTED)return false;
		
		// ------------------------ VIRTUAL ------------------------
			if(virtualClient){
				PSPelement* psp = adhoc::findPSPinList(targetPSP->mac, adhoc::server::clientList);
				//TODO le faire dans un autre thread?
				if(adhoc::server::dataCallback) adhoc::server::dataCallback(adhoc::server::dataCallbackData, psp, datalen,data);
				return true;
			}
		// ---------------------- END VIRTUAL ----------------------
		
			char tmp[datalen+1];
			tmp[0] = STD_MSG;
			memcpy(tmp+1, data, datalen);
			
			return sceNetAdhocMatchingSendData( adhoc::matchingID, targetPSP->mac, datalen+1, tmp);
		}
	
		ClientConnectionState connectionState(){
			return state;
		}
	
		void setcallback(AdhocDataCallback callback, void* pointerData){
			dataCallback = callback;
			dataCallbackData = pointerData;
		}
	
		PSPelement* getTargetPSP(){
			return targetPSP;
		}
	}
	namespace server{
		//fonction appellée par la librairie adhoc matching a la récéption d'un message
		void matchingCallback(int matchingid, int event, unsigned char *mac, int optlen, void *optdata);
		
		int start(int helloDataLength, void* helloData){
			if(started || adhoc::client::started) return -1; 
			dataCallback = 0;
			acceptationMode = SERVER_MODE_MANUAL_ACCEPATION;
			started = true;
			
			onClientConnectingCallback = 0;
			onClientConnectedCallback = 0;
			onClientDisconectCallback = 0;
			
			memcpy(hellodata, helloData, helloDataLength);
			hellolength = helloDataLength;
			
			// ------------------------ VIRTUAL ------------------------
			//on ajoute ce serveur au client virtuel
			if(adhoc::client::virtualClient){
				PSPelement* psp = new PSPelement();
				psp->hellodatasize = helloDataLength;
				memcpy(psp->hellodata, helloData, helloDataLength);
				memset(psp->mac, 0, 6);
				psp->userData = NULL;
				psp->userDataLen = 0;
			
				adhoc::client::psplist.push_back(psp);
			}
			// ---------------------- END VIRTUAL ----------------------
			
			return adhoc::start(helloDataLength, helloData, matchingCallback, PSP_ADHOC_MATCHING_MODE_HOST);
		}
		
		int stop(){
			requestList.clear();
			clientList.clear();
			// ------------------------ VIRTUAL ------------------------
			//on supprime ce serveur au client virtuel
			if(adhoc::client::virtualClient){
				for (std::vector<PSPelement*>::iterator it = adhoc::client::psplist.begin(); it!=adhoc::client::psplist.end(); ++it) {
					PSPelement* psp = *it;
					if(nullMac(psp->mac)){
						adhoc::client::psplist.erase(it);
						break;
					}
				}
				adhoc::client::state = CLIENT_STATE_DISCONNECTED;
			}
			// ---------------------- END VIRTUAL ----------------------
			
			started = false;
			return adhoc::stop();
		}
		
		void matchingCallback(int matchingid, int event, unsigned char *mac, int optlen, void *optdata){
			if(event == PSP_ADHOC_MATCHING_EVENT_HELLO){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_DISCONNECT){
				for (std::vector<PSPelement*>::iterator it = clientList.begin(); it!=clientList.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						sceNetAdhocMatchingCancelTarget(adhoc::matchingID, mac);
						if(onClientDisconectCallback)onClientDisconectCallback(onClientDisconectData, psp,0,NULL);
						clientList.erase(it);
						break;
					}
				}
				for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						requestList.erase(it);
						break;
					}
				}
			}else if(event == PSP_ADHOC_MATCHING_EVENT_JOIN){
				PSPelement* psp = adhoc::findPSPinList(mac, clientList);
				if(!psp)    psp = adhoc::findPSPinList(mac, requestList);
				if(!psp){
					psp = new PSPelement();
					memcpy(psp->mac, mac, 6);
					psp->hellodatasize = optlen;
					memcpy(psp->hellodata, optdata, optlen);
					psp->userData = NULL;
					psp->userDataLen = 0;
				}
				if(acceptationMode == SERVER_MODE_AUTO_ACCEPT){
					clientList.push_back(psp);
					sceNetAdhocMatchingSelectTarget(adhoc::matchingID, psp->mac, 0, NULL);
					if(onClientConnectedCallback)onClientConnectedCallback(onClientConnectedData, psp,0,NULL);
				}else if(acceptationMode == SERVER_MODE_AUTO_REFUSE){
					sceNetAdhocMatchingCancelTarget(adhoc::matchingID, psp->mac);
				}else {
					requestList.push_back(psp);
					if(onClientConnectingCallback)onClientConnectingCallback(onClientConnectingData, psp,0,NULL);
				}
			}else if(event == PSP_ADHOC_MATCHING_EVENT_REJECT){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_COMPLETE){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_CANCEL){
				for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						requestList.erase(it);
						break;
					}
				}
			}else if(event == PSP_ADHOC_MATCHING_EVENT_ACCEPT){
			}else if(event == PSP_ADHOC_MATCHING_EVENT_DATA){
				PSPelement* psp = findPSPinList(mac, clientList);
				if(!psp) return;
				char* buff = (char*) optdata;
				
				if(dataCallback && (buff[0] == STD_MSG)){
					dataCallback(dataCallbackData, psp, optlen-1,&buff[1]);
				}else if(buff[0] == DISCONNECT_MSG){
					for (std::vector<PSPelement*>::iterator it = clientList.begin(); it!=clientList.end(); ++it) {
						PSPelement* psp = *it;
						if(memcmp(psp->mac, mac, 6) == 0){
							if(onClientDisconectCallback)onClientDisconectCallback(onClientDisconectData, psp,0,NULL);
							clientList.erase(it);
							break;
						}
					}
					for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
						PSPelement* psp = *it;
						if(memcmp(psp->mac, mac, 6) == 0){
							requestList.erase(it);
							break;
						}
					}
				}
			}else if(event == PSP_ADHOC_MATCHING_EVENT_DATA_CONFIRM){
			}else if((event == PSP_ADHOC_MATCHING_EVENT_TIMEOUT) || (event == PSP_ADHOC_MATCHING_EVENT_ERROR)){
				for (std::vector<PSPelement*>::iterator it = clientList.begin(); it!=clientList.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						sceNetAdhocMatchingCancelTarget(adhoc::matchingID, mac);
						if(onClientDisconectCallback)onClientDisconectCallback(onClientDisconectData, psp,0,NULL);
						clientList.erase(it);
						break;
					}
				}
				for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
					PSPelement* psp = *it;
					if(memcmp(psp->mac, mac, 6) == 0){
						requestList.erase(it);
						break;
					}
				}
			}
		}
	
		std::vector<PSPelement*> getClientRequestList(){
			return requestList;
		}
	
		std::vector<PSPelement*> getClientList(){
			return clientList;
		}
	
		bool acceptConnection(PSPelement* psp){
			PSPelement* newClient = new PSPelement();
			memcpy(newClient,psp,sizeof(PSPelement));
			bool found = false;
			for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
				PSPelement* pspE = *it;
				if(memcmp(pspE->mac, psp->mac, 6) == 0){
					requestList.erase(it);
					found = true;
					break;
				}
			}
			if(!found) return false;
			clientList.push_back(newClient);
			
			// ------------------------ VIRTUAL ------------------------
			if(nullMac(psp->mac)){
				adhoc::client::state = CLIENT_STATE_CONNECTED;
				if(onClientConnectedCallback)onClientConnectedCallback(onClientConnectedData, psp,0,NULL);
				return true;
			}
			// ---------------------- END VIRTUAL ----------------------
			
			if(onClientConnectedCallback)onClientConnectedCallback(onClientConnectedData, psp,0,NULL);
			
			return 0 == sceNetAdhocMatchingSelectTarget(adhoc::matchingID, psp->mac, 0, NULL);
		}
	
		bool rejectConnection(PSPelement* psp){
			bool found = false;
			for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
				PSPelement* pspE = *it;
				if(memcmp(pspE->mac, psp->mac, 6) == 0){
					requestList.erase(it);
					found = true;
					break;
				}
			}
			if(!found) return false;
			
			
			// ------------------------ VIRTUAL ------------------------
			if(nullMac(psp->mac)){
				adhoc::client::state = CLIENT_STATE_DISCONNECTED;
				return true;
			}
			// ---------------------- END VIRTUAL ----------------------
			
			return 0 == sceNetAdhocMatchingCancelTarget(adhoc::matchingID, psp->mac);
		}
		
		bool disconnect(PSPelement* psp){
			if(!adhoc::findPSPinList(psp->mac, clientList)) return false;
			
			// ------------------------ VIRTUAL ------------------------
			if(nullMac(psp->mac)){
				adhoc::client::state = CLIENT_STATE_DISCONNECTED;
				
				for (std::vector<PSPelement*>::iterator it = clientList.begin(); it!=clientList.end(); ++it) {
					PSPelement* psp2 = *it;
					if(memcmp(psp2->mac, psp->mac, 6) == 0){
						if(onClientDisconectCallback)onClientDisconectCallback(onClientDisconectData, psp,0,NULL);
						clientList.erase(it);
						break;
					}
				}
				for (std::vector<PSPelement*>::iterator it = requestList.begin(); it!=requestList.end(); ++it) {
					PSPelement* psp2 = *it;
					if(memcmp(psp2->mac, psp->mac, 6) == 0){
						requestList.erase(it);
						break;
					}
				}
				
				return true;
			}
			// ---------------------- END VIRTUAL ----------------------
			
			char msg = DISCONNECT_MSG;
			if(sceNetAdhocMatchingSendData( adhoc::matchingID, psp->mac, 1,(void*) &msg )) return false;
		}
	
		int send(PSPelement* psp, int datalen, void *data){
			
			// ------------------------ VIRTUAL ------------------------
			if(nullMac(psp->mac)){
				//TODO le faire dans un autre thread?
				if(adhoc::client::dataCallback) adhoc::client::dataCallback(adhoc::client::dataCallbackData, adhoc::client::targetPSP, datalen,data);
				return true;
			}
			// ---------------------- END VIRTUAL ----------------------
			
			char tmp[datalen+1];
			tmp[0] = STD_MSG;
			memcpy(tmp+1, data, datalen);
			return sceNetAdhocMatchingSendData( adhoc::matchingID, psp->mac, datalen+1, tmp);
		}
	
		void setcallback(AdhocDataCallback callback, void* pointerData){
			dataCallback = callback;
			dataCallbackData = pointerData;
		}
		
		void setAcceptationMode(ServerMode mode){
			if(acceptationMode == SERVER_MODE_MANUAL_ACCEPATION){
				if(mode == SERVER_MODE_AUTO_ACCEPT){
					while( requestList.size()>0 ){
						acceptConnection(requestList.front());
					}
				}else if(mode == SERVER_MODE_AUTO_REFUSE){
					while( requestList.size()>0 ){
						rejectConnection(requestList.front());
					}
				}
			}
			acceptationMode = mode;
		}
		
		ServerMode getAcceptationMode(){
			return acceptationMode;
		}
		
		void setClientConnectingCallback(AdhocDataCallback callback, void* pointerData){
			onClientConnectingCallback = callback;
			onClientConnectingData = pointerData;
		}
		void setClientConnectedCallback(AdhocDataCallback callback, void* pointerData){
			onClientConnectedCallback = callback;
			onClientConnectedData = pointerData;
		}
		void setClientDisconnectCallback(AdhocDataCallback callback, void* pointerData){
			onClientDisconectCallback = callback;
			onClientDisconectData = pointerData;
		}
		
		bool isStarted(){
			return started;
		}
	}
}
