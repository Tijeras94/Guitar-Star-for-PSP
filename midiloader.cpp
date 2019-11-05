/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "midiloader.h"
#define DIFFICULTY_BASE 60
#define DIFFICULTY_STEP 12


float getVelocity(char* filename){
	FILE * pFile;
	unsigned char buff = 0;
	unsigned char buff2[3];// un cache de 3 char
	for(int j = 0; j<3; j++) buff2[j] = 0;
	
	pFile = fopen (filename,"r");
	float timedivision = 1;
	float tempo = 0;
	if (pFile!=NULL){
		int i = 0;//position dans le buffer
		while (!feof(pFile)){
			fread(&buff,sizeof buff,1,pFile);
			
			//la variable de division temporelle se trouve sur les offsets 12 et 13
			if(i == 12){
				timedivision = buff*256;
			}else if (i == 13){
				timedivision += buff;
			}
			
			buff2[0] = buff2[1];
			buff2[1] = buff2[2];
			buff2[2] = buff;
			// meta event = 255; type = 81 et length = 3 : c'est un event de tempo qui est codé sur 3 bytes
			if(buff2[0] == 255 && buff2[1] == 81 && buff2[2] == 3){
				fread(&buff,sizeof buff,1,pFile);
				tempo = buff*256*256;
				fread(&buff,sizeof buff,1,pFile);
				tempo += buff*256;
				fread(&buff,sizeof buff,1,pFile);
				tempo += buff;
				i += 3;
			}
			
			i++;
		}
		fclose (pFile);
	}
	//free(buff2);
	
	return tempo/(timedivision*1000);
}

void loadmidifile(char* filename, std::vector<Note*>* notes,int difficulty){
	
	//difficultyCode = index de la note 0 a la difficulté 'difficulty' dans le fichier midi
	int difficultyCode = DIFFICULTY_BASE + difficulty*DIFFICULTY_STEP;

	jdkmidi::MIDIFileReadStreamFile rs( filename );
	jdkmidi::MIDIMultiTrack tracks;
	jdkmidi::MIDIFileReadMultiTrack track_loader( &tracks );
	jdkmidi::MIDIFileRead reader( &rs, &track_loader );
	reader.Parse();
	
	jdkmidi::MIDISequencer seq(&tracks, 0);
	
	jdkmidi::MIDIMultiTrackIterator i( &tracks );
	jdkmidi::MIDITimedBigMessage *msg;
	int trk_num;
	bool finished = false;
	i.GoToTime(0);
	i.GetCurEvent(&trk_num, &msg);
	seq.GetTrackState(trk_num)->Process(msg);
	{
		Note* t[200];
		for(int j = 0; j<200;j++){
			t[j] = 0;
		}
		while (!finished){
			if( i.GetCurEvent(&trk_num, &msg ) ){
				if( msg ){
					seq.GetTrackState(trk_num)->Process(msg);
					if((strcmp("PART GUITAR",seq.GetTrackState(trk_num)->track_name) == 0)||trk_num == 1){
						if(msg->IsNoteOn()){
							Note* note = new Note();
							note->debut = msg->GetTime();
							note->note = msg->GetNote();
							note->played = false;
							note->pressed = false;
							note->missed = false;
							note->released = false;
							note->releasetime = 0;
							if(t[msg->GetNote()]){
								delete t[msg->GetNote()];
							}
							t[msg->GetNote()] = note;
						}else if(msg->IsNoteOff()){
							if(t[msg->GetNote()]){
								Note* note = t[msg->GetNote()];
								note->fin = msg->GetTime();
								if(msg->GetNote() >=difficultyCode && msg->GetNote()<=difficultyCode+4){
									notes[msg->GetNote()-difficultyCode].push_back(note);
								}
								t[msg->GetNote()] = 0;
							}
						}
					}
				    
				    finished = !i.GoToNextEvent();
				}
			}
		}
		for(int j = 0; j<200;j++){
			if(t[j]){
				delete t[j];
			}
		}
	}
	
	// ----- correction de la vitesse -----
	
	float velocity = getVelocity(filename);
	for(int j = 0; j<5; j++){
		for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
			Note* note = *it;
			#ifdef DEBUG
			note->baseDebut = note->debut;
			note->baseFin = note->fin;
			#endif
			note->debut = (int) (note->debut*velocity);
			note->fin = (int) (note->fin*velocity);
		}
	}
}

MusicEntry* searchMusic(char* name, FolderEntry* list){
	for (std::vector<MenuEntry*>::iterator it = list->list->begin(); it!=list->list->end(); ++it){
		MenuEntry* entry = *it;
		if(entry->type == FOLDER_ENTRY){
			FolderEntry* fentry = (FolderEntry*) *it;
			MusicEntry* result = searchMusic(name, fentry);
			if(result) return result;
		}else if(entry->type == MUSIC_ENTRY){
			MusicEntry* mentry = (MusicEntry*) *it;
			if(strcmp(name, mentry->name) ==0){
				return mentry;
			}
		}
	}
	return 0;
}

int numberOfNotes(char* filename, int difficulty){
	int nbNotes = 0;
	
	//difficultyCode = index de la note 0 a la difficulté 'difficulty' dans le fichier midi
	int difficultyCode = DIFFICULTY_BASE + difficulty*DIFFICULTY_STEP;

	jdkmidi::MIDIFileReadStreamFile rs( filename );
	jdkmidi::MIDIMultiTrack tracks;
	jdkmidi::MIDIFileReadMultiTrack track_loader( &tracks );
	jdkmidi::MIDIFileRead reader( &rs, &track_loader );
	reader.Parse();
	
	jdkmidi::MIDIMultiTrackIterator i( &tracks );
	jdkmidi::MIDITimedBigMessage *msg;
	int trk_num;
	bool finished = false;
	i.GoToTime(0);
	i.GetCurEvent(&trk_num, &msg);
	{
		while (!finished){
			if( i.GetCurEvent(&trk_num, &msg ) ){
				if( msg ){
					if(msg->IsNoteOn()){
						
					}else if(msg->IsNoteOff()){
						if(msg->GetNote() >=difficultyCode && msg->GetNote()<=difficultyCode+4){
							nbNotes++;
						}
					}
				    
				    finished = !i.GoToNextEvent();
				}
			}
		}
	}
	return nbNotes;
}

int accuracyPerCent(std::vector<Note*>* notes){
	int nbnotes = 0;
	int total = 0;
	
	for(int j = 0; j<5; j++){
		for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
			Note* note = *it;
			if(note->played) nbnotes++;
			total++;
		}
	}
	
	return (nbnotes*100) / total;
}

void iniInfo(char* filename, char* name, char* artist){
	FILE* file = fopen(filename,"r");
	if(!file)return;
	
	char temp[100];
	for(int j = 0; j<100; j++)temp[j]=0;
	char c;
	int i = 0;
	int n = 0;
	int a = 0;
	while( fscanf(file,"%c",&c) > 0 ){
		if(i>100 || c == '\n'){
			for(int j = 0; j<100; j++)temp[j]=0;
			i = 0;
		}else{
			temp[i] = c;
			i++;
		}
		
		if(temp[0] == 'n' && 
		   temp[1] == 'a' && 
		   temp[2] == 'm' && 
		   temp[3] == 'e' && 
		   temp[4] == ' ' && 
		   temp[5] == '=' && 
		   temp[6] == ' ' &&
		   i > 7){
			name[n] = c;
			n++;
		}
		if(temp[0] == 'a' && 
		   temp[1] == 'r' && 
		   temp[2] == 't' && 
		   temp[3] == 'i' && 
		   temp[4] == 's' && 
		   temp[5] == 't' && 
		   temp[6] == ' ' && 
		   temp[7] == '=' && 
		   temp[8] == ' ' &&
		   i > 9){
			artist[a] = c;
			a++;
		} 
		   
	}
	

	fclose(file);

}

bool isMusicDir(char* dir){
	int dfd = sceIoDopen(dir);
	if(dfd > 0)
	{
		SceIoDirent dir;
		memset(&dir,0,sizeof(SceIoDirent));

		while(sceIoDread(dfd, &dir) > 0)
		{
			if(dir.d_stat.st_attr & FIO_SO_IFREG)
			{
				if(strcmp(dir.d_name,"song.ini")==0){
					sceIoDclose(dfd);
					return true;
				}
			}
		}
		sceIoDclose(dfd);
	}
	return false;
}

void loadFileList(std::vector<MenuEntry*>* musicList, char* folder){
	musicList->clear();
	
	char tmp[512];
	int dfd;
	dfd = sceIoDopen(folder);
	if(dfd > 0)
	{
		SceIoDirent dir;
		memset(&dir,0,sizeof(SceIoDirent));

		while(sceIoDread(dfd, &dir) > 0)
		{
			if(dir.d_stat.st_attr & FIO_SO_IFDIR)
			{
				if(dir.d_name[0] != '.')
				{	
					/*Music* music = new Music();
					sprintf(music->dir,"music/%s/",dir.d_name);
					sprintf(tmp, "%ssong.ini", music->dir);
					iniInfo(tmp,music->name,music->artist);
					musicList->push_back(music);*/
					sprintf(tmp,"%s/%s",folder,dir.d_name);
					if(isMusicDir(tmp)){
						MusicEntry* music = new MusicEntry();
						music->type = MUSIC_ENTRY;
						sprintf(music->dir,"%s/%s/",folder,dir.d_name);
						sprintf(tmp, "%ssong.ini", music->dir);
						iniInfo(tmp,music->name,music->artist);
						musicList->push_back((MenuEntry*) music);
					}else{
						std::vector<MenuEntry*>* subEntry = new std::vector<MenuEntry*>();
						loadFileList(subEntry,tmp);
						
						FolderEntry* entry = new FolderEntry();
						entry->type = FOLDER_ENTRY;
						sprintf(entry->name,dir.d_name);
						entry->list = subEntry;
						entry->selection = 0;
						
						musicList->push_back((MenuEntry*) entry);
					}
				}
			}
		}
		sceIoDclose(dfd);
	}
}

//TODO marche pas!!!!!!!!!!!!!!!!
void removeEmpty(FolderEntry* folder){
	for(std::vector<MenuEntry*>::iterator it = folder->list->begin() ; it < folder->list->end(); it++ ){
		MenuEntry* entry = (MenuEntry*) *it;
		if(entry->type == FOLDER_ENTRY){
			FolderEntry* fentry = (FolderEntry*) *it;
			
			if(fentry->list->empty()){
				folder->list->erase(it);
				it--;
			}else{
				removeEmpty(fentry);
			}
		}
	}
}

FolderEntry* loadFileList(){
	FolderEntry* entry = new FolderEntry();
	entry->type = FOLDER_ENTRY;
	sprintf(entry->name, "music list");
	
	std::vector<MenuEntry*>* subEntry = new std::vector<MenuEntry*>();
	loadFileList(subEntry, "music");
	entry->list = subEntry;
	entry->selection = 0;
	
	removeEmpty(entry);
	
	return entry;
}

#ifdef AD_HOC

int SendFileAvancement = -1;
char sendFileName[100];//nom de la musique qui est en cours d'envoi
char avancementTxt[100];

char* getSendFileState(){
	if(SendFileAvancement == -1) return "envoi de musique est fini ou n'a pas débuté";
  	sprintf(avancementTxt,"envoi de %s, %i pourcents",sendFileName,SendFileAvancement);
	return avancementTxt;
}

bool canSendNextFilePacket;//vrai ssi l'accusé de récéption du packet précédent a été recu

void notifySendFileNextPacket(){
	canSendNextFilePacket = true;
}

int clientSendFile(char* folder, char* file){
	char filename[200];
	sprintf(filename,"%s%s%s",gameDir,folder,file);
	
	//FILE* pfile = fopen(filename,"rb");
	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);
	//if (pfile==NULL) return;
	if(fd<0)return fd;
	
	//fseek(pfile , 0 , SEEK_END);
	SceOff size = sceIoLseek(fd,0,SEEK_END);
	//long size = ftell (pfile);
	//rewind (pfile);
	sceIoLseek(fd,0,SEEK_SET);
	
	bool start = true;
	canSendNextFilePacket = true;
	int sizeTotale = size;
	sprintf(sendFileName,file);
	
	while(size>SEND_FILE_BUFFER_SIZE){
		//ne marche qui si le thread reseau n'est pas séparé tu thread graphique
		/*stopRender();
  		startRender();
  		char txt[100];
  		sprintf(txt,"envoi de %s, %i pourcents",file,(100*(sizeTotale-s))/sizeTotale);
  		fontPrintRC(font, 10,30,460,18, 1, txt);*/
  		int s = size;
		SendFileAvancement = (100*(sizeTotale-s))/sizeTotale;
		
		while(!canSendNextFilePacket){
			sceKernelDelayThread(1000);
		}
		
		if(start){
			SendFilePacket packet;
			packet.type = SEND_FILE;
			packet.start = true;
			packet.end = false;
			packet.length = SEND_FILE_BUFFER_SIZE;
			sprintf(packet.dir, folder);
			sprintf(packet.file, file);
			//fread (packet.buff,1,SEND_FILE_BUFFER_SIZE,pfile);
			sceIoRead(fd, packet.buff, SEND_FILE_BUFFER_SIZE);
		
			adhoc::client::send(sizeof(SendFilePacket), &packet);
		}else{
			IntermediateSendFilePacket packet;
			packet.type = INTERMEDIATE_SEND_FILE;
			sceIoRead(fd, packet.buff, SEND_FILE_BUFFER_SIZE);
		
			adhoc::client::send(sizeof(SendFilePacket), &packet);
		}
		
		canSendNextFilePacket = false;
		
		size -= SEND_FILE_BUFFER_SIZE;
		start = false;
		
		return 0;
	}
	
	while(!canSendNextFilePacket){
		sceKernelDelayThread(1000);
	}
	
	SendFilePacket packet;
	packet.type = SEND_FILE;
	packet.start = start;
	packet.end = true;
	packet.length = size;
	sprintf(packet.dir, folder);
	sprintf(packet.file, file);
	//fread (packet.buff,1,size,pfile);
	sceIoRead(fd, packet.buff, size);
	
	adhoc::client::send(sizeof(SendFilePacket), &packet);
	canSendNextFilePacket = false;
	
	//fclose(pfile);
	sceIoDclose(fd);
	SendFileAvancement = -1;
	return fd;
}

void ClientSendMusic(char* folder){
	clientSendFile(folder,"song.ini");//doit etre en premier;
	clientSendFile(folder,"guitar.ogg");
	clientSendFile(folder,"notes.mid");
	clientSendFile(folder,"song.ogg");
}

void sendClientMusicListRecurcive(FolderEntry* folder){
	for (std::vector<MenuEntry*>::iterator it=folder->list->begin() ; it < folder->list->end(); it++){
		MenuEntry* entry = *it;
		if(entry->type == FOLDER_ENTRY){
			
			MusicListEnterDirPacket packet1;
			packet1.type = MUSIC_LIST_ENTER_DIR;
			sprintf(packet1.dir,"%s",((FolderEntry*) *it)->name);
			adhoc::client::send(sizeof(MusicListEnterDirPacket), &packet1);
			
			sendClientMusicListRecurcive((FolderEntry*) *it);
			
			MusicListQuitDirPacket packet2;
			packet2.type = MUSIC_LIST_QUIT_DIR;
			adhoc::client::send(sizeof(MusicListQuitDirPacket), &packet2);
			
		}else if(entry->type == MUSIC_ENTRY){
			MusicEntry* music = (MusicEntry*) *it;
			
			MusicListPacket packet;
			packet.type = MUSIC_LIST;
			sprintf(packet.name, music->name);
			sprintf(packet.artist, music->artist);
			//sprintf(packet.dir, music->dir);
			adhoc::client::send(sizeof(MusicListPacket), &packet);
			sceKernelDelayThread(100000);//pour pas qu'il y ai de perte de données
		}
	}
}

void sendClientMusicList(FolderEntry* folder){
	StartMusicListPacket packet1;
	packet1.type = START_MUSIC_LIST;
	adhoc::client::send(sizeof(StartMusicListPacket), &packet1);
	
	sendClientMusicListRecurcive(folder);
	
	EndMusicListPacket packet2;
	packet2.type = END_MUSIC_LIST;
	adhoc::client::send(sizeof(EndMusicListPacket), &packet2);
}
#endif

//se place dans l'arborésence dir en créant les dossiers si ils n'existent pas (dir de forme "truc/bidule/machin/")
void enterDir(char* dir){
	sceIoChdir(gameDir);
	char tmp[100];
	int i = 0;
	while(dir[0] != 0){
		if(dir[0] != '/'){
			tmp[i] = dir[0];
			tmp[i+1] = 0;
			i++;
			dir++;
		}else{
			sceIoMkdir(tmp,0777);
			sceIoChdir(tmp);
			i = 0;
			dir++;
		}
	}
}
