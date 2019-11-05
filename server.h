/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef SERVER_H
#define SERVER_H
#include "matchingWifi/adhoc.h"
#include "typedef.h"
#include "packets.h"
#include "config.h"
#include "gfunctions.h"
#include "data.h"
#include <stack>

class PspData{
	public:
	FolderEntry* adhocMusicList;
	std::stack<FolderEntry*> currMusicListTargetFolder;
	bool adhocMusicListComplete;
	bool musicSelectedResponseComplete;
	bool requestDownload;
	ScorePacket score;
};

enum SERVER_STATES{
	serverstate_gathering,
	serverstate_musicListSelection,
	serverstate_musicSelection,
	serverstate_waitingMusicSelectedResponse,
	serverstate_difficultySelection,
};

class Server{
	private:
	
	SERVER_STATES state;
	SceCtrlData pad, lastpad;
	bool _mustRender;
	bool difficultyList[4];
	int selectedTarget;
	int difficulty;
	std::vector<char*>  difficultyMenuTxt;
	
	std::stack<FolderEntry*> adhocSelectedMusicList;
	
	static void proccedPacket(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff);
	
	static void clientConnectionCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff);
	static void clientDisconnectCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff);
	
	public:
	
	int start();
	
	void render();
	
	bool mustRender();
};

#endif

