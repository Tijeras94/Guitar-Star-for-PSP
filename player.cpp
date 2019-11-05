/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "player.h"

#define POINTS_PER_MS 0.1f
#define POINTS_PER_NOTE 50
#ifdef DEBUG
static int TOLERENCE = 100;
static float ZOOM = 25.0f;
#else
#define TOLERENCE 100 //la tolérence en ms pour jouer les notes
#define ZOOM 25.0f //diviseur de la longeur graphique des notes
#endif
#define GRID_RESOLUTION 2
#define TEXT_MOTION_VELOCITY 0.7f
#define STAR_ANIM_TIME 2000000
#define PING_FREQUENCY 1000000
#define SYCHRO_TOLERENCE 10
#define SHOWED_POINTS_STEP 29


void Player::ShowScore() {
	fontPrintf(font, 10,30, true, "points = %i",showedPoints);
	fontPrintf(font, 450,30, true, "X%i",mult);
}

#ifdef DEBUG
void Player::ShowDebug() {
     fontPrintf(font, 10,30, false, " debug:");
     fontPrintf(font, 10,50, (SelectedOption == 1), " ZOOM: %f", ZOOM);
     fontPrintf(font, 10,70, (SelectedOption == 2), " tolérence: %i", TOLERENCE);
     fontPrintf(font, 10,90, false, " vitesse: %f", velocity);
     fontPrintf(font, 10,110, (SelectedOption == 3), "vitesse:+-1");
     fontPrintf(font, 10,130, (SelectedOption == 4), "vitesse:+-0.1");
     fontPrintf(font, 10,150, (SelectedOption == 5), "vitesse:+-0.01");
     fontPrintf(font, 10,170, (SelectedOption == 6), "vitesse:+-0.001");
     fontPrintf(font, 10,190, (SelectedOption == 7), "decalage: %i", decalage);
}
#endif

void Player::erreur(){
	pspAudioSetVolume(2, 0, 0);
	suite = 0;
	mult = 1;
	
	for(int j = 0; j<5; j++){
		for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
			Note* note = *it;
			
			if(note->pressed && !note->released){
				note->released = true;
				note->missed = true;
			}
		}
	}
}

void Player::noerreur(int noteTime){
	pspAudioSetVolume(2, 0x8000, 0x8000);
	suite++;
	if(suite > longestStreak) longestStreak = suite;
	mult = suite/10 +1;
	if (mult < 1) mult = 1;
	if (mult > 4) mult = 4;
	points +=  ( (int)(noteTime*POINTS_PER_MS) + POINTS_PER_NOTE)*mult;
}

void Player::touchesValSet(){
	for(int i = 0; i < 5; i++){
		touchesValSet(i, (config::touchesCircle[i]   && pad::circle()   ) ||
				 (config::touchesSquare[i]   && pad::square()   ) ||
				 (config::touchesCross[i]    && pad::cross()    ) ||
				 (config::touchesTriangle[i] && pad::triangle() ) ||
				 (config::touchesUp[i]       && pad::up()       ) ||
				 (config::touchesDown[i]     && pad::down()     ) ||
				 (config::touchesLeft[i]     && pad::left()     ) ||
				 (config::touchesRight[i]    && pad::right()    ) ||
				 (config::touchesR[i]        && pad::R()        ) ||
				 (config::touchesL[i]        && pad::L()        ) );
	}
}

void Player::touchesValSet(int touchenb, bool pressed){
	if(pressed){
		if(!touches[touchenb]){
			touches[touchenb] = true;
			firetouches[touchenb] = true;
		}
	}else{
		touches[touchenb] = false;
	}
}

void Player::stop(){
	state = finished;
}

unsigned int Player::getColor(int i, float alpha){
	unsigned int color = GU_COLOR( 1.0f, 1.0f, 1.0f, 0.0f );
	if(i==0) color =  color0;
	if(i==1) color =  color1;
	if(i==2) color =  color2;
	if(i==3) color =  color3;
	if(i==4) color =  color4;
	return (((int)(alpha*255.0f)) << 24) | color;
}
unsigned int Player::getHightColor(int i, float alpha){
	unsigned int color = GU_COLOR( 1.0f, 1.0f, 1.0f, 0.0f );
	if(i==0) color =  hightColor0;
	if(i==1) color =  hightColor1;
	if(i==2) color =  hightColor2;
	if(i==3) color =  hightColor3;
	if(i==4) color =  hightColor4;
	return (((int)(alpha*255.0f)) << 24) | color;
}

void Player::renderScreen(){
	bool toucheActivated[5];
	for(int i = 0; i<5; i++)toucheActivated[i] = false;
	
	//rendu des étoiles
	for (std::deque<Etoile*>::iterator it = etoileList.begin(); it!=etoileList.end(); ++it) {
		Etoile* etoile = *it;
		float time = timeNow - etoile->timeStart;
		float alpha = (time < (STAR_ANIM_TIME/2)) ? time/1000000.0f : (2000000.0f - time)/1000000.0f;
		if(alpha < 0) alpha = 0.0f;
		alpha/= 5-mult;
		imageRender(etoileImg,etoile->x,etoile->y,32,32,GU_COLOR(1.0f, 1.0f, 1.0f, alpha));
	}
	
	{//rendu du public
		float level[5];
		for(int i = 0; i<5; i++)level[i] = 0;
		for(int j = 0; j<5; j++){
			for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
				Note* note = *it;
				int time = note->debut;
				if(time >= oggTime-3000 && time <= (oggTime+3000)){
					int delta = (oggTime>time)? oggTime-time : time-oggTime;
					level[j] += (5000/(delta+232) -1.55f)/3;
				}
			}
		}
		int t = ((int)oggTime)%500;
		imageRender(publicImg,-10,0,90,90-level[0]*((t<250)? (t)/250.0f :(500-(t))/250.0f ),GU_COLOR(1.0f, 1.0f, 1.0f, 0.75f));
		imageRender(publicImg,50,0,75,75-level[1]*((t<250)? (t)/250.0f :(500-(t))/250.0f ),GU_COLOR(1.0f, 1.0f, 1.0f, 0.35f));
		imageRender(publicImg,335,0,85,85-level[2]*((t<250)? (t)/250.0f :(500-(t))/250.0f ),GU_COLOR(1.0f, 1.0f, 1.0f, 0.2f));
		imageRender(publicImg,383,0,70,70-level[3]*((t<250)? (t)/250.0f :(500-(t))/250.0f ),GU_COLOR(1.0f, 1.0f, 1.0f, 0.8f));
		imageRender(publicImg,420,0,95,95-level[4]*((t<250)? (t)/250.0f :(500-(t))/250.0f ),GU_COLOR(1.0f, 1.0f, 1.0f, 0.3f));
	}
	
	{//rendu du manche
		sceGumLoadIdentity();
		sceGumDrawArray( GU_TRIANGLES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
			3*2, 0, manche);
		sceGumDrawArray( GU_LINES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
			2*5, 0, mancheLines );
	}
	
	sceGuEnable(GU_TEXTURE_2D);
	/*for(float x = -2; x<=2; x++){
		sceGumLoadIdentity();
		ScePspFVector3 move = {x/2.0f, 0.0f, 0.0f };
		sceGumTranslate( &move );
		sceGuTexImage( 0, lineImg->textureWidth, lineImg->textureHeight, lineImg->textureWidth,(void*)lineImg->data );
		sceGuColor( getColor(-1, 1.0f));
		sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
			3*2, 0, texLine2 );
	}*/
	for(float i = 0; i<14/GRID_RESOLUTION; i++){
		sceGumLoadIdentity();
		ScePspFVector3 move = { 0.0f, (-((float)( ((int)(oggTime*100.0f/ZOOM)) %(1000*GRID_RESOLUTION)))/1000.0f) +i*GRID_RESOLUTION, 0.0f };
		sceGumTranslate( &move );
		bindTexure( lineImg );
		sceGuColor( getColor(-1, 1.0f));
		sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
			3*2, 0, texLine );
	}
	sceGuDisable(GU_TEXTURE_2D);
	for(int j = 0; j<5; j++){
		for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
		Note* note = *it;
		    int time = note->debut;
		    int timeend = note->fin;
		    int timedrop = note->releasetime;
		    if(note-> pressed && !note->released) toucheActivated[j] = true;
		    if(timeend >= oggTime-1000 && time <= (oggTime+5000)){
		    	int x = j-2;
		    	float y = (time-oggTime)/ZOOM;
			ScePspFVector3 move = { ((float)x)/2.0f, y/10.0f, 0.0f };
		    	
			sceGumLoadIdentity();
			sceGumTranslate( &move );
			ScePspFVector3 scale;
			ScePspFVector3 scale1 = { 1.0f, (timeend-time)/(ZOOM*10), 1.0f };
			ScePspFVector3 scale2 = { 1.0f, (timedrop-time)/(ZOOM*10), 1.0f };
			if(!note->released){
				scale = scale1;
			}else{
				scale = scale2;
			}
			sceGumPushMatrix();
			sceGumScale(&scale);
			sceGuColor( getColor(j, 1.0f) );
			if(note->missed) sceGuColor( missedColor );
			sceGumDrawArray( GU_TRIANGLES, GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
						3*2, 0, squarel );
			sceGumPopMatrix();
		
			sceGuColor( getHightColor(j,0.1f) );
			if(note->pressed & !note->released){
				int randValue = sceKernelUtilsMt19937UInt(&ctx)%10;
				for(float u = -randValue/100.0f ; u <= randValue/100.0f; u += 0.02f){
					int randValue2 = sceKernelUtilsMt19937UInt(&ctx)%10;
					for(float v = -randValue2/100.0f ; v <= randValue2/100.0f; v += 0.02f){
						sceGumPushMatrix();
						ScePspFVector3 translate = { u,v, 0.0f };
						sceGumTranslate( &translate );
						sceGumScale(&scale);
						sceGumDrawArray( GU_TRIANGLES, GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
									3*2, 0, squarel );
						sceGumPopMatrix();
					}
				}
			}
			sceGuEnable(GU_TEXTURE_2D);
			sceGumLoadIdentity();
			sceGumTranslate( &move );
			if(note->missed){
				sceGuColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 0.5f ) );
				bindTexure( noteMiss );
			}else{
				bindTexure( notesImg[j] );
			}
			//sceGuColor( getColor(j, 1.0f));
			sceGuColor( getColor(-1,1.0f) );
			sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
				3*2, 0, texSquare );
			sceGuDisable(GU_TEXTURE_2D);
		     }
		    
		}
	}

	//rendu des touches
	sceGuEnable(GU_TEXTURE_2D);
	for(int j = 0; j<5; j++){
		if(touchesImg[j]!=0){
			sceGumLoadIdentity();
			ScePspFVector3 move = { ((float)(j-2))/2.0f, 0.0f, 0.0f };
			sceGumTranslate( &move );
			bindTexure( touchesImg[j] );
			if(!touches[j]){//rendu si la touche c'est pas pressée
				sceGuColor( getColor(-1,1.0f) );
				sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
					//114, 0, touche3D );
					3*2, 0, texSquare );
			}else{//si elle est préssée
				for(float u = 1.5f; u>1.0f; u-=0.1f){
					sceGumPushMatrix();
					ScePspFVector3 scale = { u, u, u };
					sceGumScale( &scale );
					sceGuColor( getHightColor(-1,0.1f) );
					sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
						3*2, 0, texSquare );
					sceGumPopMatrix();
				}
				
				sceGuColor( getColor(-1,1.0f) );
				sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
					//114, 0, touche3D );
					3*2, 0, texSquare );
				
				if(toucheActivated[j]){//la touche est préssée et correspond a une note
					
					clock_t delta = timeNow - timeLast;
					for( int i = 0; i < delta/20000 ; i++){
						Particule* newP = new Particule();
						int randValue = sceKernelUtilsMt19937UInt(&ctx);
						int randValue2 = sceKernelUtilsMt19937UInt(&ctx);
						newP->angle = (randValue%600)/1000.0f;
						newP->length = (randValue2%200) + 300;
						newP->timeStart = clock()/1000;
						particulesList[j].push_back(newP);
					}
					
					/*sceGuColor( getColor(-1,0.4f) );
					bindTexure( flammeImg[j] );
					int randValue2 = sceKernelUtilsMt19937UInt(&ctx)%5 +5;
					for(float v = -randValue2/100.0f ; v <= randValue2/100.0f; v += 0.05f){
						int randValue = sceKernelUtilsMt19937UInt(&ctx)%10;
						for(float u = -randValue/100.0f ; u <= randValue/100.0f; u += 0.05f){
							sceGumPushMatrix();
							ScePspFVector3 translate = { u, 0.25f, v };
							sceGumTranslate( &translate );
							ScePspFVector3 scale = { 1.0f, 2.0f, 1.0f };
							sceGumScale( &scale );
							sceGumRotateX(3.14f/4);
							sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
								3*2, 0, texSquare );
							sceGumPopMatrix();
						}
					}*/
				}else{//la touche est préssée mais ne correspond pas a une note (ou la note est finie
					
				}
			}
		}
	}
	for(int i = 0; i < 5; i++){//rendu des particules
		sceGumLoadIdentity();
		ScePspFVector3 move = { ((float)(i-2))/2.0f, 0.0f, 0.0f };
		sceGumTranslate( &move );
		
		bindTexure( flammeImg[i] );
		
		for (std::deque<Particule*>::iterator it = particulesList[i].begin(); it!=particulesList[i].end(); ++it) {
			Particule* p = *it;
			
			if(timeNow/1000-p->timeStart < p->length){
				sceGuColor( getColor( -1, 0.9f*(1-(timeNow/1000-p->timeStart)/((float) p->length)) ) );
				sceGumPushMatrix();
				sceGumRotateX(3.14f/6);
				sceGumRotateZ(p->angle);
				ScePspFVector3 translate = { 0.0f, p->length*(timeNow/1000-p->timeStart)/300000.0f, 0.0f };
				sceGumTranslate( &translate );
				ScePspFVector3 scale = { 0.4f, 0.8f, 0.4f };
					sceGumScale( &scale );
				sceGumDrawArray( GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,
									3*2, 0, texSquare );
				sceGumPopMatrix();
			}
		}
	}
	
	sceGuDisable(GU_TEXTURE_2D);
	sceGumLoadIdentity();
}

Player::Player(bool adhoc){
	//initialisation des variables
	#ifdef DEBUG
	SelectedOption = 1;
	decalage = 0;
	#endif
	#ifdef AD_HOC
	lastSendPing = 0;
	ping = 0;
	startMusicTime = 0;
	adhocActived = adhoc;
	#endif
	points = 0;
	showedPoints = 0;
	expectedPoints = 0;
	suite = 0;
	mult = 1;
	pauseSelection = 0;
	motion = 0;
	state = play;
	for(int j = 0; j<5; j++)touches[j] = false;
	for(int j = 0; j<5; j++)firetouches[j] = false;
	songPresent = false;
	rhythmPresent = false;
	
	sceKernelUtilsMt19937Init(&ctx, time(NULL));
}

Player::~Player(){
	if(songPresent)oggA::OGG_Stop();
	if(rhythmPresent)oggC::OGG_Stop();
	oggB::OGG_Stop();
	
	if(songPresent)oggA::OGG_FreeTune();
	if(rhythmPresent)oggC::OGG_FreeTune();
	oggB::OGG_FreeTune();
}

void Player::loadMusic(char* path, int difficulty){
//Chargement des ressources
	sprintf(songPath,"%s%s",path,"song.ogg");
	
	sprintf(rhythmPath,"%s%s",path,"rhythm.ogg");

	sprintf(guitarPath,"%s%s",path,"guitar.ogg");
	sprintf(notesPath,"%s%s",path,"notes.mid");
	loadmidifile(notesPath,notes,difficulty);
	velocity = getVelocity(notesPath);
}

void Player::reset(){
	#ifdef AD_HOC
	startMusicTime = clock();
	#endif
	
	if(songPresent)oggA::OGG_Stop();
	if(rhythmPresent)oggC::OGG_Stop();
	oggB::OGG_Stop();
	
	if(songPresent)oggA::OGG_FreeTune();
	if(rhythmPresent)oggC::OGG_FreeTune();
	oggB::OGG_FreeTune();

	oggB::OGG_Load(guitarPath);
	songPresent = oggA::OGG_Load(songPath);
	//rhythmPresent = oggC::OGG_Load(rhythmPath);
	
	if(songPresent)oggA::OGG_Play();
	if(rhythmPresent)oggC::OGG_Play();
	oggB::OGG_Play();
	pspAudioSetVolume(2, 0x8000, 0x8000);
	
	points = 0;//on remet les compteurs a 0
	showedPoints = 0;
	expectedPoints = 0;
	suite = 0;
	longestStreak = 0;
	state = play;
	for(int j = 0; j<5; j++){
		for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
			Note* note = *it;
			note->played = false;
			note->missed = false;
			note->pressed = false;
			note->released = false;
			note->releasetime = 0;
		}
	}
}

bool Player::running(){
	return state != finished;
}

#ifdef AD_HOC
/*void Player::adhocActivate(){
	adhocActived = true;
}*/

/*void Player::pongPacket(PingPacket *t){
	ping = clock() - t->t;
}*/

void Player::scorePacket(ScorePacket *p){
	/*pointsMulti = p->points;
	accuracyMulti = p->accuracy;
	longestStreakMulti = p->longestStreak;*/
	scoreList.push_back(p);
}
#endif

void Player::render(){// boucle
	
	timeNow = clock();

	if(oggB::OGG_EndOfStream()){
		if(songPresent)oggA::OGG_Stop();
		if(rhythmPresent)oggC::OGG_Stop();
		oggB::OGG_Stop();
		state = score;
		accuracy = accuracyPerCent(notes);
		
		#ifdef AD_HOC
		if(adhocActived){
			ScorePacket* packet= new ScorePacket();
			packet->type = SCORE_PACKET;
			packet->points = points;
			packet->accuracy = accuracy;
			packet->longestStreak = longestStreak;
			adhoc::client::send(sizeof(ScorePacket), packet);
			scoreList.push_back(packet);
		}
		#endif
	}
	
	oggTime = oggB::OGG_GetTime();
	
	#ifdef AD_HOC
	if(adhocActived){
		float delayTime = oggTime - (clock()-startMusicTime)/1000;
		
		if(delayTime < -SYCHRO_TOLERENCE){
			oggB::accelerate();
		}else if(delayTime >= 0){
			oggB::stopAccelerate();
		}
		
		{
			float a,b;
			a = oggA::OGG_GetTime();
			b = oggB::OGG_GetTime();
		
			if(a-b < -SYCHRO_TOLERENCE){
				oggA::accelerate();
			}else if(a-b >= 0){
				oggA::stopAccelerate();
			}
		}
		
		{
			float c,b;
			c = oggC::OGG_GetTime();
			b = oggB::OGG_GetTime();
		
			if(c-b < -SYCHRO_TOLERENCE){
				oggC::accelerate();
			}else if(c-b >= 0){
				oggC::stopAccelerate();
			}
		}
		
		/*if(clock() -lastSendPing > PING_FREQUENCY){
			PingPacket packet;
			packet.type = PING_PACKET;
			packet.t = clock();
			adhoc::client::send(sizeof(PingPacket), &packet);
			lastSendPing = clock();
		}*/
		
		#ifdef DEBUG
		//fontPrintf(font, 0,252, true, "ping = %i micros",ping);
		#endif
	}
	#endif
	
	if( (points + expectedPoints - showedPoints) > SHOWED_POINTS_STEP){
		showedPoints += SHOWED_POINTS_STEP;
	}else if( ( -points - expectedPoints + showedPoints) > SHOWED_POINTS_STEP){
		showedPoints -= SHOWED_POINTS_STEP;
	}else{
		showedPoints = points + expectedPoints;
	}
	
	if(state == play){
		touchesValSet();//on met a jour l'état des touches
		
		#ifdef AD_HOC
		if(!adhocActived){
		#endif
		if (pad::one_start()){
			if(songPresent)oggA::OGG_Pause();
			if(rhythmPresent)oggC::OGG_Pause();
			oggB::OGG_Pause();
			state = inPause;
			pauseSelection = 0;
			motion = 0;
		}
		#ifdef AD_HOC
		}else{
			if (pad::one_start()){
				state = inPauseMulti;
				pauseSelection = 0;
				motion = 0;
			}
		}
		#endif
		
		#ifdef DEBUG
		if (pad::one_select()){
			if(songPresent)oggA::OGG_Pause();
			if(rhythmPresent)oggC::OGG_Pause();
			oggB::OGG_Pause();
			state = inDebug;
		}
		#endif
		
		{// gestions des notes
			expectedPoints = 0;//on calcul dans cette variable les points qui vont etre gagné mais qui ne sont pas comptabilisé dans la variable points
			for(int j = 0; j<5; j++){
				for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
					Note* note = *it;
					int debut = note->debut;
					
					if(!note->missed && !note->played && note->pressed){
						expectedPoints += POINTS_PER_NOTE + ( (oggTime > debut) ? (oggTime - debut)*POINTS_PER_MS : 0) ;
					}
					
					if(oggTime < (debut + TOLERENCE) && oggTime > (debut - TOLERENCE)){// on est sur le début de la note
						if(!note->played && !note->missed && !note->pressed){
							if(firetouches[j]){
								note->pressed = true;
								firetouches[j] = false;
								pspAudioSetVolume(2, 0x8000, 0x8000);
							}
						}
					}
				
					if(oggTime > (debut - TOLERENCE)){ //on est sur ou apres le début de la note
						if(!note->played && !note->missed && note->pressed){
							if(!touches[j]){
								note->played = true;
								note->released = true;
								note->releasetime = (int) oggTime;
								if(note->releasetime > note->fin) note->releasetime = note->fin;
								noerreur(note->releasetime - note->debut);
							}
						}
					}
				
					if(oggTime > (debut + TOLERENCE)){//on est apres le début de la note
						if(!note->played && !note->missed && !note->pressed){
							note->missed = true;
							erreur();
						}
					}
					
					if(oggTime > (note->fin + TOLERENCE)){//la note est completement passée, 
									    //le +TOLERENCE sert a garder la note en mode pressée et non joué assez longtemps pour que ce soit affiché
						if(!note->played && !note->missed && note->pressed){
							note->played = true;
							note->released = true;
							note->releasetime = (int) oggTime;
							if(note->releasetime > note->fin) note->releasetime = note->fin;
							noerreur(note->releasetime - note->debut);
						}
					}
				}
			}
		
			for(int j = 0; j<5; j++){
				if(firetouches[j]) erreur();//une touche a été pressé mais elle ne correspond a aucune note
				firetouches[j] = false;
			}
		}
		//gestion de la liste des étoiles
		{
			clock_t delta = timeNow - timeLast;
			if( rand()%800000 < delta*mult){
				Etoile* newEtoile = new Etoile();
				newEtoile->x = rand()%(480 - 32);
				newEtoile->y = rand()%(272 - 32);
				newEtoile->timeStart = clock();
				etoileList.push_back(newEtoile);
			}
				
			while(!etoileList.empty()){
				clock_t time = clock() - etoileList.front()->timeStart;
				if( time > STAR_ANIM_TIME){
					etoileList.pop_front();
				}else{
					break;
				}
			}
		}
		//gestion de la liste des particules
		{
			for(int i = 0; i < 5; i++) {
				while(!particulesList[i].empty()){
					clock_t time = clock()/1000 - particulesList[i].front()->timeStart;
					if( time > particulesList[i].front()->length){
						particulesList[i].pop_front();
					}else{
						break;
					}
				}
			}
		}
		renderScreen();
		#ifdef ANAGLYPH
		copyImage();
		clearRender();
		renderScreen();
		putRed();
		#endif
		ShowScore();
	}else if(state == inPause){
		
		if (pad::one_start()){
			if(songPresent)oggA::OGG_Pause();
			if(rhythmPresent)oggC::OGG_Pause();
			oggB::OGG_Pause();
			state = play;
		}
		if (pad::scroll_up()){
			if(pauseSelection > 0){
				motion -= 18;
				pauseSelection--;
			}
		}
		if (pad::scroll_down()){
			if(pauseSelection < 2){
				motion += 18;
				pauseSelection++;
			}
		}
		if (pad::one_cross()){
			if(pauseSelection == 0){
				if(songPresent)oggA::OGG_Pause();
				if(rhythmPresent)oggC::OGG_Pause();
				oggB::OGG_Pause();
				state = play;
			}else if(pauseSelection == 1){
				reset();
			}else if(pauseSelection == 2){
				stop();
			}
		}
		
		motion = (int)(motion*TEXT_MOTION_VELOCITY);
		
		renderScreen();
		#ifdef ANAGLYPH
		copyImage();
		clearRender();
		renderScreen();
		putRed();
		#endif
		fontPrintf(font, 10,144 + 0*18 - pauseSelection*18 + motion, pauseSelection == 0, /*"reprendre"*/ "resume");
		fontPrintf(font, 10,144 + 1*18 - pauseSelection*18 + motion, pauseSelection == 1, /*"relancer la musique"*/ "restart music");
		fontPrintf(font, 10,144 + 2*18 - pauseSelection*18 + motion, pauseSelection == 2, /*"retourner au menu principal"*/ "go to main menu");
		
	}
	#ifdef AD_HOC
	else if(state == inPauseMulti){
			if (pad::one_start()){
				state = play;
			}
			if (pad::scroll_up()){
				if(pauseSelection > 0){
					motion -= 18;
					pauseSelection--;
				}
			}
			if (pad::scroll_down()){
				if(pauseSelection < (adhoc::server::isStarted() ? 2 : 1)){
					motion += 18;
					pauseSelection++;
				}
			}
			if (pad::one_cross()){
				if(pauseSelection == 0){
					state = play;
				}else if((pauseSelection == 1) && adhoc::server::isStarted()){
					//TODO
					/*stop();
					
					QuitMusicPacket quitMessage;
					quitMessage.type = QUIT_MUSIC_PACKET;
					adhoc::client::send(sizeof(QuitMusicPacket), &quitMessage);*/
				}else if((pauseSelection == 2) && adhoc::server::isStarted()){
					quitMultiAsynchronously();
				}else if(pauseSelection == 1){
					quitMultiAsynchronously();
				}
			}
		
		motion = (int)(motion*TEXT_MOTION_VELOCITY);
		
		renderScreen();
		fontPrintf(font, 10,144 + 0*18 - pauseSelection*18 + motion, pauseSelection == 0, "reprendre");
		if(adhoc::server::isStarted()) fontPrintf(font, 10,144 + 1*18 - pauseSelection*18 + motion, pauseSelection == 1, "quitter cette musique");
		fontPrintf(font, 10,144 + (adhoc::server::isStarted() ? 2 : 1)*18 - pauseSelection*18 + motion, pauseSelection == (adhoc::server::isStarted() ? 2 : 1), "quitter le mode multijoueur");
	}
	#endif
	#ifdef DEBUG
	else if(state == inDebug){
		
			if (pad::scroll_up()){
				SelectedOption--;
				if (SelectedOption < 1) SelectedOption = 7;
			}
				
			if (pad::scroll_down()){
				SelectedOption++;
				if (SelectedOption > 7) SelectedOption = 1;
			}
			float lastVelocity = velocity;
			int lastDecalage = decalage;
			if (pad::one_left()){
				if(SelectedOption == 1){
					ZOOM-=10.0f;
				}else if(SelectedOption == 2){
					TOLERENCE-=5;
				}else if(SelectedOption == 3){
					velocity-=1.0f;
				}else if(SelectedOption == 4){
					velocity-=0.1f;
				}else if(SelectedOption == 5){
					velocity-=0.01f;
				}else if(SelectedOption == 6){
					velocity-=0.001f;
				}else if(SelectedOption == 7){
					decalage-=10;
				}
			}
				    	
			if (pad::one_right()){
				if (SelectedOption == 1){
					ZOOM+=10.0f;
				}else if(SelectedOption == 2){
					TOLERENCE+=5;
				}else if(SelectedOption == 3){
					velocity+=1.0f;
				}else if(SelectedOption == 4){
					velocity+=0.1f;
				}else if(SelectedOption == 5){
					velocity+=0.01f;
				}else if(SelectedOption == 6){
					velocity+=0.001f;
				}else if(SelectedOption == 7){
					decalage+=10;
				}
			}
		
			if (pad::one_select()){
				if(songPresent)oggA::OGG_Pause();
				if(rhythmPresent)oggC::OGG_Pause();
				oggB::OGG_Pause();
				state = play;
			}
			
			if(velocity != lastVelocity || decalage != lastDecalage){
				for(int j = 0; j<5; j++){
					for (std::vector<Note*>::iterator it = notes[j].begin(); it!=notes[j].end(); ++it) {
						Note* note = *it;
						note->debut = (int) (note->baseDebut*velocity) + decalage;
						note->fin = (int) (note->baseFin*velocity) + decalage;
					}
				}
			}
		
	renderScreen();
	ShowDebug();
	}
	#endif
	else if(state == score){
		#ifdef AD_HOC
		if(adhocActived){
			fontPrintf(font, 10,30, 1, "vous:");
			fontPrintf(font, 10,55, 1, "points = %i",points);
			fontPrintf(font, 10,75, 1, "précision = %i pourcent",accuracy);
			fontPrintf(font, 10,95, 1, "plus longue suite de notes = %i",longestStreak);
			
			fontPrintf(font, 10,130, 1, "votre adversaire:");
			//TODO
			/*fontPrintf(font, 10,155, 1, "points = %i",pointsMulti);
			fontPrintf(font, 10,175, 1, "précision = %i pourcent",accuracyMulti);
			fontPrintf(font, 10,195, 1, "plus longue suite de notes = %i",longestStreakMulti);*/
			
			if(adhoc::server::isStarted()) fontPrintf(font, 10,235, 1, "appuyez sur X pour quitter");
		
			if (pad::one_cross() && adhoc::server::isStarted()){
				//TODO
				/*stop();
				
				QuitMusicPacket quitMessage;
				quitMessage.type = QUIT_MUSIC_PACKET;
				adhoc::client::send(sizeof(QuitMusicPacket), &quitMessage);*/
			}
		}else{
		#endif
			fontPrintf(font, 10,30, 1, "points = %i",points);
			fontPrintf(font, 10,48, 1, /*"précision = %i pourcent"*/ "accuracy = %i percent",accuracy);
			fontPrintf(font, 10,66, 1, /*"plus longue suite de notes = %i"*/ "longest streak = %i",longestStreak);
			fontPrintf(font, 10,84, 1, /*"appuyez sur X pour quitter"*/ "press cross to quit");
		
			if (pad::one_cross()){
				stop();
			}
		#ifdef AD_HOC
		}
		#endif
		
	}
	timeLast = timeNow;
}
