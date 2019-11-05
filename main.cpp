/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "main.h"

PSP_MODULE_INFO("fretsOnPsp", 0, 1, 1);

#ifdef DEBUG
#include "exceptionHandler/utility/exception.c"
#endif

PSP_MAIN_THREAD_ATTR(0);

/**
 * liste des états possibles du menu principale
 */
enum GAME_STATES{
	intro,
	mainMenu,
	credits,
	musicSelection,
	difficultySelection,
	gaming,
	error,
	aide,
	configMenu,
#ifdef AD_HOC
	startMulti,
	startServerMulti,
	startClientMulti,
	selectTargetMulti,
	gatheringMulti,
	playMulti,
	quitMultiState,
	quitMultiStateSendQuitMessage,
#endif
};


FolderEntry* musicList;
std::vector<FolderEntry*> musicListMenu;
GAME_STATES state;
GAME_STATES nextState;

int difficulty;//numero de la difficulté séléctionné
bool run;//devient false pour que le programme quitte
Player* player;
int possiblesDifficulties[4];//possiblesDifficulties[i] = nombre de notes pour la difficulté i
int mainMenuSelection;//entrée de menu séléctionné
bool playGuitar, playSong;
clock_t onEntrySince;
char tmp[100];
#ifdef DEBUG
u32 tickResolution;
u64 fpsTickNow;
u64 fpsTickLast;
int fpsi;
float fps;
#endif
char errtxt[100];

std::vector<char*> mainMenuTxt;
std::vector<char*>  difficultyMenuTxt;

#ifdef AD_HOC
std::vector<char*>  clientServerMenuTxt;
MusicEntry* selectedMusic = 0; 
int selectedDifficulty = 0;
int selectedTarget = 0;
bool mustReset = false;
bool reseted = false;
bool mustQuitMusic = false;
SceUID fd;
//FolderEntry* sharedMusicList; passer a un map
//bool sharedMusicListAdded;
Server* server;


void quitMulti(){
	/*if(sendQuitMessage){
		QuitPacket quitMessage;
		quitMessage.type = QUIT_PACKET;
		adhoc::send(sizeof(QuitPacket), &quitMessage);
	}*/
	adhoc::client::disconnect();
	if(player){
		delete player;
		player = 0;
	}
	//sceKernelTerminateDeleteThread(recieveThreadId);
	//adhoc::stop();
	state = mainMenu;
}

void quitMultiAsynchronously(){
	state = quitMultiStateSendQuitMessage;
}

void proccedClientPacket(void* pointerData, adhoc::PSPelement* psp, int datalen, void *buff){
	Packet *data = (Packet*) buff;
	
	if(data->type == MUSIC_SELECTED){
		MusicSelectedPacket *databis = (MusicSelectedPacket*) buff;
		
		MusicSelectedResponsePacket packet;
		packet.type = MUSIC_SELECTED_RESPONSE;
		packet.downloadRequest = false;
		
		selectedMusic = searchMusic(databis->name,musicList);
		if(!selectedMusic){
			selectedMusic = new MusicEntry();
			sprintf(selectedMusic->name, databis->name);
			sprintf(selectedMusic->artist, databis->artist);
			selectedMusic->dir[0] = 0;
			
			packet.downloadRequest = true;
		}else{
			
			sprintf(tmp,"%snotes.mid",selectedMusic->dir);
			packet.d[0] = numberOfNotes(tmp,0);
			packet.d[1] = numberOfNotes(tmp,1);
			packet.d[2] = numberOfNotes(tmp,2);
			packet.d[3] = numberOfNotes(tmp,3);
		}
		
		/*if(databis->downloadRequest){
			//if(selectedMusic->dir){
				state = sendFileState;
				sendMusic(selectedMusic->dir);
			//}//TODO else
		}*/
		
		adhoc::client::send(sizeof(MusicSelectedResponsePacket), &packet);
	}else if(data->type == SEND_FILE){

		state = error;
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
				
				while (!musicListMenu.empty()) musicListMenu.pop_back();
				musicListMenu.push_back(musicList);
				
				selectedMusic =  searchMusic(selectedMusic->name, musicList);
				
				//TODO remove selectedMusic in sharedMusicList
				/*for (std::vector<Music*>::iterator it = sharedMusicList.begin(); it!=sharedMusicList.end(); ++it) {
					Music* music = *it;
					if(strcmp(selectedMusic->name, music->name) ==0){
						sharedMusicList.erase(it);
					}
				}*/
			}
		}
		
		sceIoWrite(fd, databis->buff, databis->length);
		
		if(databis->end){
			sceIoDclose(fd);
			sceIoChdir(gameDir);
		}
		
		SendFileNextPacket reponse;
		reponse.type = SEND_FILE_NEXT_PACKET;
		adhoc::client::send(sizeof(MusicSelectedResponsePacket), &reponse);
	}else if(data->type == INTERMEDIATE_SEND_FILE){
		IntermediateSendFilePacket *databis = (IntermediateSendFilePacket*) buff;
		
		sceIoWrite(fd, databis->buff, SEND_FILE_BUFFER_SIZE);
		
		SendFileNextPacket reponse;
		reponse.type = SEND_FILE_NEXT_PACKET;
		adhoc::client::send(sizeof(MusicSelectedResponsePacket), &reponse);
	}else if(data->type == LOAD_MUSIC){
		LoadMusicPacket *databis = (LoadMusicPacket*) buff;
		
		selectedDifficulty = databis->difficulty;
		state = playMulti;
	}else if(data->type == START_MUSIC){
		mustReset = true;
	}/*else if(data->type == PING_PACKET){
		PingPacket *databis = (PingPacket*) buff;
		
		PingPacket packet;
		packet.type = PONG_PACKET;
		packet.t = databis->t;
		adhoc::send(sizeof(PingPacket), &packet);
	}else if(data->type == PONG_PACKET){
		PingPacket *databis = (PingPacket*) buff;
		if(player) player->pongPacket(databis);
	}*/else if(data->type == QUIT_PACKET){
		state = quitMultiState;
	}else if(data->type == QUIT_MUSIC_PACKET){
		mustQuitMusic = true;
	}else if(data->type == SCORE_PACKET){
		ScorePacket *databis = (ScorePacket*) buff;
		if(player) player->scorePacket(databis);
	}else if(data->type == SEND_FILE_NEXT_PACKET){
		notifySendFileNextPacket();
	}
}
#endif

void startPreview(MusicEntry* music){
	/*if(playGuitar) return;
	
	char guitarPath[128];
	char songPath[128];
	sprintf(songPath,"%s%s",music->dir,"song.ogg");
	sprintf(guitarPath,"%s%s",music->dir,"guitar.ogg");
	
	if(!oggB::OGG_Load(guitarPath))return;
	if(oggA::OGG_Load(songPath)){
		playSong = true;
	}else{
		playSong = false;
	}
	playGuitar = true;
	if(playSong)oggA::OGG_Play();
	oggB::OGG_Play();*/
}

void stopPreview(){
	/*if(playGuitar){
		oggB::OGG_Stop();
		oggB::OGG_FreeTune();
	}
	if(playSong){
		oggA::OGG_Stop();
		oggA::OGG_FreeTune();
	}
	playSong = false;
	playGuitar = false;
	onEntrySince = clock();*/
}

int main() {
	#ifdef DEBUG
	initExceptionHandler();
	#endif
	
	SetupCallbacks();
  	
  	state = intro;
	
	musicList = loadFileList();
	musicListMenu.push_back(musicList);

	InitGU();
	
	SetupProjection();
	intraFontInit();
	resetMotionMenu();
	

	pspAudioInit();
	oggA::OGG_Init(1);
	oggB::OGG_Init(2);
	oggC::OGG_Init(3);
	oggD::OGG_Init(4);
	
	pad::init();
	
	config::initConfigMenu();
	config::loadConfig();
	{
		int err;
		if( (err = loadData()) != 0 ){
			pspDebugScreenInit();
			pspDebugScreenPrintf("error while loading data (code : %i)",err);
			sceKernelSleepThread();
			return 0;
		}
	}
	
	difficulty = 0;//numero de la difficulté séléctionné
	run = true;//devient false pour que le programme quitte
	player = 0;
	//possiblesDifficulties[4];//possiblesDifficulties[i] = nombre de notes pour la difficulté i
	mainMenuSelection = 0;//entrée de menu séléctionné
	playGuitar = false;
	playSong = false;
	
	mainMenuTxt.push_back(/*"jouer"*/ "play");
	#ifdef AD_HOC
	mainMenuTxt.push_back("multijoueurs");
	#endif
	mainMenuTxt.push_back("config");
	mainMenuTxt.push_back(/*"aide"*/ "help");
	mainMenuTxt.push_back("credits");
	mainMenuTxt.push_back(/*Cladil"quitter"*/ "quit");
	
	#ifdef DEBUG
	tickResolution = sceRtcGetTickResolution();
	sceRtcGetCurrentTick( &fpsTickLast );
	fpsi = 0;
	#endif
	
	//init adhoc
	#ifdef AD_HOC
	clientServerMenuTxt.push_back("demarer en mode client");
	clientServerMenuTxt.push_back("demarer en mode serveur");
	int err;
	err = adhoc::init("GUITAR000");
	if(err<0){
		state = error;
		nextState = intro;
		if(sceKernelDevkitVersion() < 0x02000010){
			sprintf(errtxt,/*"erreur a l'initiation du reseau le logiciel systeme est trop ancien veilliez le mettre a jour"*/
					 "error while starting network, please update the firmware");
		}else{
			sprintf(errtxt,/*"erreur a l'initiation du reseau (%i)"*/ "error while starting network (%i)",err);
		}
	}
	//adhoc::start(sizeof(config::pseudo)+1, config::pseudo);
	
	//sharedMusicList = new FolderEntry();
	//sprintf(sharedMusicList->name, "musiques partagées");
	//sharedMusicList->selection = 0;
	//sharedMusicList->type = FOLDER_ENTRY;
	//sharedMusicListAdded = false;
	//TODO vider sharedMusicList avant chaque récéption de la liste des musiques
	#endif
	
	
	//reservé à l'intro (apres l'init adhoc pour le temps)
	char tempdir[128];
	sprintf(tempdir, "skins/%s/start.png", config::skin);
	Image* pressStart = loadImage(tempdir);
	Image* spashScreen = loadImage("skins/concours-devsgen.png");
	clock_t introStart = clock();
	
	while(run){
		#ifdef DEBUG
		sceRtcGetCurrentTick( &fpsTickNow );
		#endif
		
		startRender();
		
		#ifdef AD_HOC_1_THREAD
		if(runRecieveThreadMethod){
			recieveThread();
		}
		#endif
		
		pad::updatePad();
		
		if(state == intro){
			clock_t t = clock()-introStart;
			if(t<1000000){
				sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
				imageRender(spashScreen,0,0,GU_COLOR(1.0f,1.0f,1.0f,t/1000000.0f));
			}else if(t<2500000){
				imageRender(spashScreen,0,0);
			}else if(t<3500000){
				imageRender(spashScreen,0,0,GU_COLOR(1.0f,1.0f,1.0f,(3500000-t)/1000000.0f));
			}else if(t<4500000){
				imageRender(logo,112,104,256,64, GU_COLOR(1.0f,1.0f,1.0f,(t-3500000)/1000000.0f));
			}else{
				imageRender(logo,112,104,256,64);
				clock_t t2 = (t-4500000)%3000000;
				imageRender(pressStart,112,20,256,64, GU_COLOR(1.0f,1.0f,1.0f,(t2<1500000)? t2/1500000.0f: (3000000-t2)/1500000.0f));
			}
			if(pad::start()){
				freeImage(pressStart);
				freeImage(spashScreen);
				state = mainMenu;
			}
		}else if(state == mainMenu){
				
			if (pad::scroll_down()){
				#ifdef AD_HOC
				if(mainMenuSelection<5){
				#else
				if(mainMenuSelection<4){
				#endif
					mainMenuSelection++;
					motionDown();
				}
			}
			if (pad::scroll_up()){
				if(mainMenuSelection>0){
					mainMenuSelection--;
					motionUp();
				}
			}
			if (pad::one_cross()){
				if(mainMenuSelection == 0){
					resetMotionMenu();
					state = musicSelection;
					onEntrySince = clock();
				#ifdef AD_HOC
				}else if(mainMenuSelection == 1){
					resetMotionMenu();
					state = startMulti;
					selectedTarget = 0;
				}else if(mainMenuSelection == 2){
					resetMotionMenu();
					state = configMenu;
				}else if(mainMenuSelection == 3){
					resetMotionMenu();
					state = aide;
				}else if(mainMenuSelection == 4){
					resetMotionMenu();
					state = credits;
				}else if(mainMenuSelection == 5){
					sceKernelExitGame();
				}
				#else
				}else if(mainMenuSelection == 1){
					resetMotionMenu();
					state = configMenu;
				}else if(mainMenuSelection == 2){
					resetMotionMenu();
					state = aide;
				}else if(mainMenuSelection == 3){
					resetMotionMenu();
					state = credits;
				}else if(mainMenuSelection == 4){
					sceKernelExitGame();
				}
				#endif
			}
			
			imageRender(logo,112,104,256,64);
			printMenu(mainMenuTxt,mainMenuSelection);
		}else if(state == credits){
			
			if (pad::one_cross()){
				state = mainMenu;
			}
			
			fontPrintf(font, 10,30, 1, "guitar star v1.99");
			fontPrintf(font, 10,70, 1, /*"programmé par Festi"*/ "programed by Festi");
			fontPrintf(font, 10,90, 1, "http://flash.festi.free.fr/");
			fontPrintf(font, 10,130, 1, /*"graphismes par Cladil"*/ "graphics by Cladil");
			
		}else if(state == configMenu){
			if(config::configMenu() == false) state = mainMenu;
		}else if(state == aide){
			
			if (pad::one_cross()){
				state = mainMenu;
			}
			
			fontPrintf(font, 10,30, 1, /*"gauche: vert"*/ "left: green");
			fontPrintf(font, 10,50, 1, /*"bas: rouge"*/ "down: red");
			fontPrintf(font, 10,70, 1, /*"haut: vert + rouge"*/ "up: green + red");
			fontPrintf(font, 10,90, 1, /*"carré: jaune"*/ "square: yellow");
			fontPrintf(font, 10,110, 1, /*"croix: bleu"*/ "cross: blue");
			fontPrintf(font, 10,130, 1, /*"rond: rose"*/ "circle: pink");
			fontPrintf(font, 10,150, 1, /*"triangle: jaune + rose"*/ "triangle: yellow + pink");
			
		}else if(state == musicSelection){
			
			FolderEntry* fentry = musicListMenu.back();
			
			if(fentry->list->at(fentry->selection)->type == MUSIC_ENTRY && onEntrySince + 1000000 < clock()){
				startPreview((MusicEntry*) fentry->list->at(fentry->selection));
			}
			
			if (pad::scroll_down()){
				if(fentry->selection < fentry->list->size()-1){
					fentry->selection++;
					motionDown();
					stopPreview();
				}
			}
			if (pad::scroll_up()){
				if(fentry->selection>0){
					fentry->selection--;
					motionUp();
					stopPreview();
				}
			}
			if (pad::one_cross()){
				if(fentry->list->at(fentry->selection)->type == MUSIC_ENTRY){
					MusicEntry* music = (MusicEntry*) fentry->list->at(fentry->selection);
					sprintf(tmp,"%snotes.mid",music->dir);
					possiblesDifficulties[0] = numberOfNotes(tmp,0);
					possiblesDifficulties[1] = numberOfNotes(tmp,1);
					possiblesDifficulties[2] = numberOfNotes(tmp,2);
					possiblesDifficulties[3] = numberOfNotes(tmp,3);
					difficultyMenuTxt.clear();
					possiblesDifficulties[0] ? difficultyMenuTxt.push_back(/*"très facile"*/ "super easy") : difficultyMenuTxt.push_back(" ");
					possiblesDifficulties[1] ? difficultyMenuTxt.push_back(/*"facile"*/ "easy") : difficultyMenuTxt.push_back(" ");
					possiblesDifficulties[2] ? difficultyMenuTxt.push_back(/*"moyen"*/ "medium") : difficultyMenuTxt.push_back(" ");
					possiblesDifficulties[3] ? difficultyMenuTxt.push_back(/*"expert"*/ "expert") : difficultyMenuTxt.push_back(" ");
					state = difficultySelection;
					difficulty = 0;
					while(!possiblesDifficulties[difficulty]){
						difficulty++;
					}
					resetMotionMenu();
				}else if(fentry->list->at(fentry->selection)->type == FOLDER_ENTRY){
					FolderEntry* folder = (FolderEntry*) fentry->list->at(fentry->selection);
					musicListMenu.push_back(folder);
				}
				stopPreview();
			}
			if (pad::one_triangle()){
				resetMotionMenu();
				if(musicListMenu.size()>1){
					musicListMenu.pop_back();
				}else{
					state = mainMenu;
				}
				stopPreview();
			}
			
			printMenuF(fentry);
		}else if(state == difficultySelection){
			
			if (pad::one_up()){
				int nextD = difficulty;
				do{
					nextD--;
				}while(!possiblesDifficulties[nextD] && nextD>=0);
				if(nextD >= 0){
					motionUp();
					difficulty = nextD;
				}
			}
		
			if (pad::one_down()){
				int nextD = difficulty;
				do{
					nextD++;
				}while(!possiblesDifficulties[nextD] && nextD<=4);
				if(nextD < 4){
					motionDown();
					difficulty = nextD;
				}
			}
			if (pad::one_triangle()){
				state = musicSelection;
				onEntrySince = clock();
				resetMotionMenu();
			}
			if (pad::one_cross()){
				player = new Player(false);
				
				FolderEntry* fentry = musicListMenu.back();
				MusicEntry* music = (MusicEntry*) fentry->list->at(fentry->selection);
				player->loadMusic(music->dir,difficulty);
				
				player->reset();
				state = gaming;
			}
			
			printMenu( difficultyMenuTxt,difficulty);
			
		}else if(state == gaming){
			player->render();

			if(!player->running()){
				delete player;
				player = 0;
				state = mainMenu;
			}
		}else if(state == error){
			
			if (pad::one_cross()){
				introStart = clock();
				state = nextState;
			}
				
			printRC(errtxt);
		}
		#ifdef AD_HOC
		else if(state == startMulti){
			
			if (pad::scroll_down()){
				if(selectedTarget<1){
					selectedTarget++;
					motionDown();
				}
			}
			if (pad::scroll_up()){
				if(selectedTarget>0){
					selectedTarget--;
					motionUp();
				}
			}
			if (pad::one_cross()){
				if(selectedTarget == 0){
					state = startClientMulti;
					selectedTarget = 0;
					resetMotionMenu();
				}else if(selectedTarget == 1){
					state = startServerMulti;
					selectedTarget = 0;
					resetMotionMenu();
				}
			}
			
			if (pad::one_triangle()){
				selectedTarget = 0;
				resetMotionMenu();
				state = mainMenu;
			}
				
			printMenu(clientServerMenuTxt,selectedTarget);
		}else if(state == startServerMulti){
			server = new Server();
			
			int err;
			if(!sceWlanGetSwitchState()){
				nextState = mainMenu;
				state = error;
				sprintf(errtxt,"l'interupteur wlan est desactivé");
			}else if( (err = server->start()) <0){
				nextState = mainMenu;
				state = error;
				sprintf(errtxt,"erreur au demmarage du module server (%i)",err);
			}else{
				state = startClientMulti;
			}
		}else if(state == startClientMulti){
			mustReset = false;
			reseted = false;
			mustQuitMusic = false;
			selectedTarget = 0;
			
			int err;
			
			if(!sceWlanGetSwitchState()){
				nextState = mainMenu;
				state = error;
				sprintf(errtxt,"l'interupteur wlan est desactivé");
			}else if(adhoc::server::isStarted()){
				if( (err = adhoc::client::startVirtual(strlen(config::pseudo)+1, (void*) config::pseudo)) <0){
					nextState = mainMenu;
					state = error;
					sprintf(errtxt,"erreur au demmarage du module client (%i)",err);
				}else{
					adhoc::client::setcallback(proccedClientPacket, NULL);
					state = selectTargetMulti;
				}
			}else{
				if( (err = adhoc::client::start(strlen(config::pseudo)+1, (void*) config::pseudo)) <0){
					nextState = mainMenu;
					state = error;
					sprintf(errtxt,"erreur au demmarage du module client (%i)",err);
				}else{
					adhoc::client::setcallback(proccedClientPacket, NULL);
					state = selectTargetMulti;
				}
			}
		}else if(!sceWlanGetSwitchState()){
			quitMulti();
			nextState = mainMenu;
			state = error;
			sprintf(errtxt,"l'interupteur wlan est desactivé");
		}else if(state == selectTargetMulti){
			if(adhoc::client::connectionState() == adhoc::CLIENT_STATE_CONNECTING){
				printRCf("En attente que %s accepte la connection (triangle pour annuler)",(char*) adhoc::client::getTargetPSP()->hellodata);
				
				if(adhoc::server::isStarted()){
					for(int i = 0; i< adhoc::server::getClientRequestList().size(); i++){
						unsigned char* mac = adhoc::server::getClientRequestList().at(i)->mac;
						if(mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && mac[3] == 0 && mac[4] == 0 && mac[5] == 0){
							adhoc::server::acceptConnection(adhoc::server::getClientRequestList().at(i));
						}
					}
				}
				
				
				if (pad::one_triangle()){
					adhoc::client::cancelRequestedConnection();
				}
			}else if(adhoc::client::connectionState() == adhoc::CLIENT_STATE_CONNECTED){
				
				sendClientMusicList(musicList);
			
				state = gatheringMulti;
				//if(adhoc::server::isStarted()) state = serverMulti;
			}else{
				if(adhoc::server::isStarted()){
					adhoc::client::requestConnection(adhoc::client::getServerList().at(0));
				}else{
					printRCf("selectionnez un serveur",(char*) adhoc::client::getTargetPSP()->hellodata);
					
					std::vector<char*> nameList;
					for(int i = 0; i<adhoc::client::getServerList().size(); i++) nameList.push_back((char*) adhoc::client::getServerList().at(i)->hellodata);
					printMenu(nameList,selectedTarget);
				
					
						if (pad::scroll_down()){
							if(selectedTarget < adhoc::client::getServerList().size()-1){
								selectedTarget++;
								motionDown();
							}
						}
						if (pad::scroll_up()){
							if(selectedTarget>0){
								selectedTarget--;
								motionUp();
							}
						}
						if (pad::one_triangle()){
							quitMulti();
							resetMotionMenu();
						}
						if (pad::one_cross()){
							adhoc::client::requestConnection(adhoc::client::getServerList().at(selectedTarget));
						}
				}
			}
		}else if(adhoc::client::connectionState() != adhoc::CLIENT_STATE_CONNECTED){
			quitMulti();
			nextState = mainMenu;
			state = error;
			sprintf(errtxt,"deconnecté du server");
		}else if(server && server->mustRender()){
			server->render();
			state = playMulti;
		}else if(state == gatheringMulti){//etat reservé au client
			printRC("connecté au serveur, en attente\ntriangle pour quitter");
			
			
			if (pad::one_triangle()){
				quitMulti();
				resetMotionMenu();
			}
		}else if(state == playMulti){
			if(!player){
				player = new Player(true);
				player->loadMusic(selectedMusic->dir,selectedDifficulty);
				//player->adhocActivate();
			}
			if(mustReset){
				player->reset();
				mustReset = false;
				reseted = true;
			}
			if(reseted)player->render();

			if(!player->running() || mustQuitMusic){
				reseted = false;
				delete player;
				player = 0;
				
				mustReset = false;
				reseted = false;
				mustQuitMusic = false;
				state = musicSelection;
			}
		}else if(state == quitMultiState){
			quitMulti();
			state = error;
			nextState = mainMenu;
			sprintf(errtxt,"déconnecté de la partie");
		}else if(state == quitMultiStateSendQuitMessage){
			quitMulti();
		}
		#endif
		
		#ifdef DEBUG
		fpsi++;
		if(fpsi%10 == 0) fps = 1/((fpsTickNow - fpsTickLast)/((float)tickResolution));
		fontPrintf(font, 0,272, true, "fps: %f",fps);
		fpsTickLast = fpsTickNow;
		
		fontPrintf(font, 200,272, true, "state: %i",state);
		#endif
		
		stopRender();
	}

	sceGuTerm();
	//free( dList );

	sceKernelExitGame();

	
  return 0;
}
