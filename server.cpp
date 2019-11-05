/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "server.h"

int Server::start(){
	state = serverstate_gathering;
	sceCtrlReadBufferPositive(&lastpad, 1);
	_mustRender = true;
	for(int i = 0; i < 4; i++)difficultyList[i] = false;
	int err = adhoc::server::start(strlen(config::pseudo)+1, (void*) config::pseudo);
	if(err != 0) return err;
	
	adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_AUTO_ACCEPT);
	adhoc::server::setClientConnectedCallback(clientConnectionCallback, this);
	adhoc::server::setClientDisconnectCallback(clientDisconnectCallback, this);
	adhoc::server::setcallback(proccedPacket, this);
	
	return 0;
}

bool Server::mustRender(){
	return _mustRender;
}

void Server::render(){
	if(state == serverstate_gathering){
		fontPrint(font, 10,30, 1,"salon de rassemblement, psp connectées:");
		int y = 48;
		for(int i = 0; i < adhoc::server::getClientList().size(); i++){
			fontPrint(font, 10,y, 1,(char*) adhoc::server::getClientList().at(i)->hellodata);
			y += 18;
		}
		fontPrint(font, 10,y, 1,"X pour fermer le salon et selectionner la musique");
		
		sceCtrlReadBufferPositive(&pad, 1); //READ CONTROLS
		if (pad.Buttons != lastpad.Buttons){
			lastpad = pad;
			if (pad.Buttons & PSP_CTRL_CROSS){
				adhoc::server::setAcceptationMode(adhoc::SERVER_MODE_AUTO_REFUSE);
				state = serverstate_musicListSelection;
				selectedTarget = 0;
			}
		}
	}else if(state == serverstate_musicListSelection){
		fontPrint(font, 10,30, 1,"choisissez le repertoire musical:");
		
		std::vector<char*> collectionList;
		/*collectionList.push_back("ma collection de musiques");*/
		for(int i = 0; i < adhoc::server::getClientList().size(); i++)
			collectionList.push_back((char*) adhoc::server::getClientList().at(i)->hellodata);
		
		printMenu(collectionList,selectedTarget);
		
		sceCtrlReadBufferPositive(&pad, 1); //READ CONTROLS
		if (pad.Buttons != lastpad.Buttons){
			lastpad = pad;
			if (pad.Buttons & PSP_CTRL_DOWN){
				if(selectedTarget < collectionList.size()-1){
					selectedTarget++;
					motionDown();
				}
			}
			if (pad.Buttons & PSP_CTRL_UP){
				if(selectedTarget>0){
					selectedTarget--;
					motionUp();
				}
			}
			if (pad.Buttons & PSP_CTRL_CROSS){
				while(!adhocSelectedMusicList.empty()) adhocSelectedMusicList.pop();
				/*if(selectedTarget == 0){
					adhocSelectedMusicList.push(musicList);
					state = serverstate_musicSelection;
				}else*/{
					if(((PspData*)adhoc::server::getClientList().at(selectedTarget/*-1*/)->userData)->adhocMusicListComplete){
						adhocSelectedMusicList.push( ((PspData*)adhoc::server::getClientList().at(selectedTarget/*-1*/)->userData)->adhocMusicList );
						state = serverstate_musicSelection;
					}//TODO else: afficher un message d'erreur;
				}
			}
		}
	}else if(state == serverstate_musicSelection){
		sceCtrlReadBufferPositive(&pad, 1); //READ CONTROLS
		
		FolderEntry* fentry = adhocSelectedMusicList.top();

		if (pad.Buttons != lastpad.Buttons){
			    	lastpad = pad;
			if (pad.Buttons & PSP_CTRL_DOWN){
				if(fentry->selection < fentry->list->size()-1){
					fentry->selection++;
					motionDown();
				}
			}
			if (pad.Buttons & PSP_CTRL_UP){
				if(fentry->selection>0){
					fentry->selection--;
					motionUp();
				}
			}
			if (pad.Buttons & PSP_CTRL_TRIANGLE){
				if(adhocSelectedMusicList.size()>1){
					adhocSelectedMusicList.pop();
				}else{
					state = serverstate_musicListSelection;
					resetMotionMenu();
				}
			}
			if (pad.Buttons & PSP_CTRL_CROSS){
				if(fentry->list->at(fentry->selection)->type == FOLDER_ENTRY){
					FolderEntry* folder = (FolderEntry*) fentry->list->at(fentry->selection);
					adhocSelectedMusicList.push(folder);
				}else if(fentry->list->at(fentry->selection)->type == MUSIC_ENTRY){
					MusicEntry* music = (MusicEntry*) fentry->list->at(fentry->selection);
					
					MusicSelectedPacket packet;
					packet.type = MUSIC_SELECTED;
					sprintf(packet.name, music->name);
					sprintf(packet.artist, music->artist);
					//packet.downloadRequest = music->dir[0] == 0;
					for(int i = 0; i < adhoc::server::getClientList().size(); i++)
						adhoc::server::send(adhoc::server::getClientList().at(i),sizeof(MusicSelectedPacket), &packet);
					
					//selectedMusic = music;
					
					resetMotionMenu();
					
					state = serverstate_waitingMusicSelectedResponse;
				}
			}
		}
		
		printMenuF(fentry);
	}else if(state == serverstate_waitingMusicSelectedResponse){
		
		int musicSelectedResponseCount = 0;
		int downloadRequestCount = 0;
		
		for(int i = 0; i < adhoc::server::getClientList().size(); i++){
			PspData* pspData = (PspData*) adhoc::server::getClientList().at(i)->userData;
			if(pspData->musicSelectedResponseComplete) musicSelectedResponseCount++;
			if(pspData->requestDownload) downloadRequestCount++;
		}
		
		if(musicSelectedResponseCount == adhoc::server::getClientList().size()){
			if(downloadRequestCount == 0){
				state = serverstate_difficultySelection;
			}else{
				//TODO
			}
		}
		
		char txt[128];
		sprintf(txt,"En attente de réponse des client.\n%i clients on répondu sur %i clients, %i requete de téléchargement",musicSelectedResponseCount,adhoc::server::getClientList().size(),downloadRequestCount);
		fontPrintRC(font, 10,30, 262, 30, 1,txt);
		
	}else if(state == serverstate_difficultySelection){
		sceCtrlReadBufferPositive(&pad, 1); //READ CONTROLS
		
		if (pad.Buttons != lastpad.Buttons){
			lastpad = pad;
			if (pad.Buttons & PSP_CTRL_UP){
				int nextD = difficulty;
				do{
					nextD--;
				}while(!difficultyList[nextD] && nextD>=0);
				if(nextD >= 0){
					motionUp();
					difficulty = nextD;
				}
			}
		
			if (pad.Buttons & PSP_CTRL_DOWN){
				int nextD = difficulty;
				do{
					nextD++;
				}while(!difficultyList[nextD] && nextD<=4);
				if(nextD < 4){
					motionDown();
					difficulty = nextD;
				}
			}
			if (pad.Buttons & PSP_CTRL_TRIANGLE){
				state = serverstate_musicSelection;
				resetMotionMenu();
			}
			if (pad.Buttons & PSP_CTRL_CROSS){
				LoadMusicPacket packet;
				packet.type = LOAD_MUSIC;
				packet.difficulty = difficulty;
				for(int i = 0; i < adhoc::server::getClientList().size(); i++)
					adhoc::server::send(adhoc::server::getClientList().at(i), sizeof(LoadMusicPacket), &packet);
				
				sceKernelDelayThread(5000000);
				
				StartMusicPacket packet2;
				packet2.type = START_MUSIC;
				for(int i = 0; i < adhoc::server::getClientList().size(); i++)
				adhoc::server::send(adhoc::server::getClientList().at(i), sizeof(StartMusicPacket), &packet2);
				
				_mustRender = false;
			}
		}
		
		printMenu(difficultyMenuTxt,difficulty);
		
	}
	
	#ifdef DEBUG
	fontPrintf(font, 0,252, true, "server");
	fontPrintf(font, 200,252, true, "state: %i",state);
	#endif
}

void Server::clientConnectionCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff){
	PspData* data = new PspData();
	data->adhocMusicListComplete = false;
	data->requestDownload = false;
	data->musicSelectedResponseComplete = false;
	
	FolderEntry* fe = new FolderEntry();
	fe->type = FOLDER_ENTRY;
	sprintf(fe->name, "musiques de %s",(char*) psp->hellodata);
	fe->selection = 0;
	fe->list = new std::vector<MenuEntry*>();
	data->adhocMusicList = fe;
	
	data->currMusicListTargetFolder.push(fe);
	
	psp->userData = data;
	psp->userDataLen = sizeof(PspData);
}

void Server::clientDisconnectCallback(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff){
	delete psp->userData;
	psp->userData = 0;
}

void Server::proccedPacket(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff){
	Packet *data = (Packet*) buff;
	PspData* pspData = (PspData*) psp->userData;
	Server* server = (Server*) pointerData;
	
	if(data->type == MUSIC_LIST){
		MusicListPacket *databis = (MusicListPacket*) buff;
		
		MusicEntry* music = new MusicEntry();
		music->type = MUSIC_ENTRY;
		sprintf(music->name, databis->name);
		sprintf(music->artist, databis->artist);
		music->dir[0] = 0;
		pspData->currMusicListTargetFolder.top()->list->push_back((MenuEntry*) music);
		
	}else if(data->type == MUSIC_LIST_ENTER_DIR){
		MusicListEnterDirPacket *databis = (MusicListEnterDirPacket*) buff;
		
		FolderEntry* fe = new FolderEntry();
		fe->type = FOLDER_ENTRY;
		sprintf(fe->name, "%s",(char*) databis->dir);
		fe->selection = 0;
		fe->list = new std::vector<MenuEntry*>();
		
		pspData->currMusicListTargetFolder.top()->list->push_back((MenuEntry*) fe);
		pspData->currMusicListTargetFolder.push(fe);
	}else if(data->type == MUSIC_LIST_QUIT_DIR){
		pspData->currMusicListTargetFolder.pop();
	}else if(data->type == START_MUSIC_LIST){
		pspData->adhocMusicListComplete = false;
		
		pspData->adhocMusicList->list->clear();
		while(!pspData->currMusicListTargetFolder.empty())pspData->currMusicListTargetFolder.pop();
		
		pspData->adhocMusicList->selection = 0;
		
		pspData->currMusicListTargetFolder.push(pspData->adhocMusicList);
		
	}else if(data->type == END_MUSIC_LIST){
		pspData->adhocMusicListComplete = true;
	}else if(data->type == MUSIC_SELECTED_RESPONSE){
		MusicSelectedResponsePacket *databis = (MusicSelectedResponsePacket*) buff;
		
		pspData->requestDownload = databis->downloadRequest;
		pspData->musicSelectedResponseComplete = true;
		
		if(!databis->downloadRequest){
			for(int i = 0; i < 4; i++) server->difficultyList[i] = databis->d[i];
			server->difficultyMenuTxt.clear();
			server->difficultyList[0] ? server->difficultyMenuTxt.push_back("très facile") : server->difficultyMenuTxt.push_back(" ");
			server->difficultyList[1] ? server->difficultyMenuTxt.push_back("facile")      : server->difficultyMenuTxt.push_back(" ");
			server->difficultyList[2] ? server->difficultyMenuTxt.push_back("moyen")       : server->difficultyMenuTxt.push_back(" ");
			server->difficultyList[3] ? server->difficultyMenuTxt.push_back("expert")      : server->difficultyMenuTxt.push_back(" ");
			
			//mise a jour de l'entrée initiale du menu de difficulté
			server->difficulty = 0;
			while(!server->difficultyList[server->difficulty]){
				server->difficulty++;
			}
		}
		
		/*if(databis->downloadRequest){
			//if(selectedMusic->dir){
				state = sendFileState;
				sendMusic(selectedMusic->dir);
			//}//TODO else
		}
		
		state = error;
		nextState = error;
		sprintf(errtxt,"");
		
		char tmp[100];
		sprintf(tmp,"%snotes.mid",selectedMusic->dir);
		possiblesDifficulties[0] = numberOfNotes(tmp,0);
		possiblesDifficulties[1] = numberOfNotes(tmp,1);
		possiblesDifficulties[2] = numberOfNotes(tmp,2);
		possiblesDifficulties[3] = numberOfNotes(tmp,3);
		difficultyMenuTxt.clear();
		possiblesDifficulties[0] ? difficultyMenuTxt.push_back("très facile") : difficultyMenuTxt.push_back(" ");
		possiblesDifficulties[1] ? difficultyMenuTxt.push_back("facile") : difficultyMenuTxt.push_back(" ");
		possiblesDifficulties[2] ? difficultyMenuTxt.push_back("moyen") : difficultyMenuTxt.push_back(" ");
		possiblesDifficulties[3] ? difficultyMenuTxt.push_back("expert") : difficultyMenuTxt.push_back(" ");
		difficulty = 0;
		while(!possiblesDifficulties[difficulty]){
			difficulty++;
		}
		resetMotionMenu();
		state = difficultySelectionMulti;*/
	}else if(data->type == SEND_FILE){

		/*state = error;
		nextState = error;
		sprintf(errtxt,"récéption de musique en cours");
		
		SendFilePacket *databis = (SendFilePacket*) buff;
		
		if(databis->start){
			enterDir(databis->dir);
			
			fd = sceIoOpen(databis->file, PSP_O_WRONLY|PSP_O_CREAT, 0777);
			
			//si la musique séléctionné n'est pas sur la console
			if(selectedMusic->dir[0] == 0){
				
				//sprintf(selectedMusic->dir,databis->dir);
				
				//musicList.push_back(selectedMusic);
				
				delete musicList;
				musicList = loadFileList();
				
				while (!musicListMenu.empty()) musicListMenu.pop();
				musicListMenu.push(musicList);
				
				selectedMusic =  searchMusic(selectedMusic->name, musicList);
				
				//TODO remove selectedMusic in sharedMusicList
				/*for (std::vector<Music*>::iterator it = sharedMusicList.begin(); it!=sharedMusicList.end(); ++it) {
					Music* music = *it;
					if(strcmp(selectedMusic->name, music->name) ==0){
						sharedMusicList.erase(it);
					}
				}*//*
			}
		}
		
		sceIoWrite(fd, databis->buff, databis->length);
		
		if(databis->end){
			sceIoDclose(fd);
			sceIoChdir(gameDir);
		}
		
		SendFileNextPacket reponse;
		reponse.type = SEND_FILE_NEXT_PACKET;
		adhoc::send(sizeof(MusicSelectedResponsePacket), &reponse);*/
		
		for(int i = 0; i < adhoc::server::getClientList().size(); i++){
			if(( (PspData*) adhoc::server::getClientList().at(i)->userData)->requestDownload)
				adhoc::server::send(adhoc::server::getClientList().at(i), datalen, buff);
		}
		
	}else if(data->type == INTERMEDIATE_SEND_FILE){
		/*IntermediateSendFilePacket *databis = (IntermediateSendFilePacket*) buff;
		
		sceIoWrite(fd, databis->buff, SEND_FILE_BUFFER_SIZE);
		
		SendFileNextPacket reponse;
		reponse.type = SEND_FILE_NEXT_PACKET;
		adhoc::send(sizeof(MusicSelectedResponsePacket), &reponse);*/
		
		for(int i = 0; i < adhoc::server::getClientList().size(); i++){
			if(( (PspData*) adhoc::server::getClientList().at(i)->userData)->requestDownload)
				adhoc::server::send(adhoc::server::getClientList().at(i), datalen, buff);
		}
	}/*else if(data->type == LOAD_MUSIC){
		LoadMusicPacket *databis = (LoadMusicPacket*) buff;
		
		selectedDifficulty = databis->difficulty;
		state = playMulti;
	}*//*else if(data->type == START_MUSIC){
		mustReset = true;
	}*//*else if(data->type == PING_PACKET){
		PingPacket *databis = (PingPacket*) buff;
		
		PingPacket packet;
		packet.type = PONG_PACKET;
		packet.t = databis->t;
		adhoc::send(sizeof(PingPacket), &packet);
	}*//*else if(data->type == PONG_PACKET){
		PingPacket *databis = (PingPacket*) buff;
		if(player) player->pongPacket(databis);
	}*//*else if(data->type == QUIT_PACKET){
		state = quitMultiState;
	}*//*else if(data->type == QUIT_MUSIC_PACKET){
		mustQuitMusic = true;
	}*/else if(data->type == SCORE_PACKET){
		ScorePacket *databis = (ScorePacket*) buff;
		memcpy(&pspData->score, databis, sizeof(ScorePacket));
	}else if(data->type == SEND_FILE_NEXT_PACKET){
		//notifySendFileNextPacket();//TODO
	}
}

