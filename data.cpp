/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "data.h"

//textures
 Image* touchesImg[5];
 Image* notesImg[5];
 Image* noteMiss;
 Image* flammeImg[5];
 Image* lineImg;
 Image* etoileImg;
 Image* publicImg;
 Image* fond;
 Image* logo;
 Image* touchesListImg;
  
unsigned int color0,
	    color1,
	    color2,
	    color3,
	    color4,
	    hightColor0,
	    hightColor1,
	    hightColor2,
	    hightColor3,
	    hightColor4,
	    missedColor;

 intraFont* font;

//le chemin ou se situe le jeu
 char gameDir[256];

int loadData(){
	getcwd(gameDir,256);
	strcat(gameDir,"/");
	
	char  dir[128];
	char temp[128];
	sprintf(dir,"skins/%s/", config::skin);
	
	sprintf(temp,"%s%s",dir,"flammevert.png");
	if((flammeImg[0] = loadImage(temp))           == NULL) return -1;
	sprintf(temp,"%s%s", dir,"flammerouge.png");
	if((flammeImg[1] = loadImage(temp))           == NULL) return -2;
	sprintf(temp,"%s%s", dir,"flammejaune.png");
	if((flammeImg[2] = loadImage(temp))           == NULL) return -3;
	sprintf(temp,"%s%s", dir,"flammebleu.png");
	if((flammeImg[3] = loadImage(temp))           == NULL) return -4;
	sprintf(temp,"%s%s", dir,"flammerose.png");
	if((flammeImg[4] = loadImage(temp))           == NULL) return -5;
	
	sprintf(temp,"%s%s",dir,"boutonvert.png");
	if((touchesImg[0] = loadImage(temp))           == NULL) return -6;
	sprintf(temp,"%s%s", dir,"boutonrouge.png");
	if((touchesImg[1] = loadImage(temp))           == NULL) return -7;
	sprintf(temp,"%s%s", dir,"boutonjaune.png");
	if((touchesImg[2] = loadImage(temp))           == NULL) return -8;
	sprintf(temp,"%s%s", dir,"boutonbleu.png");
	if((touchesImg[3] = loadImage(temp))           == NULL) return -9;
	sprintf(temp,"%s%s", dir,"boutonrose.png");
	if((touchesImg[4] = loadImage(temp))           == NULL) return -10;
	
	sprintf(temp,"%s%s", dir,"notevert.png");
	if((notesImg[0] = loadImage(temp))           == NULL) return -11;
	sprintf(temp,"%s%s", dir,"noterouge.png");
	if((notesImg[1] = loadImage(temp))           == NULL) return -12;
	sprintf(temp,"%s%s", dir,"notejaune.png");
	if((notesImg[2] = loadImage(temp))           == NULL) return -13;
	sprintf(temp,"%s%s", dir,"notebleu.png");
	if((notesImg[3] = loadImage(temp))           == NULL) return -14;
	sprintf(temp,"%s%s", dir,"noterose.png");
	if((notesImg[4] = loadImage(temp))           == NULL) return -15;
	
	sprintf(temp,"%s%s", dir,"noteMissed.png");
	if((noteMiss = loadImage(temp))                == NULL) return -16;
	
	sprintf(temp,"%s%s", dir,"barrette.png");
	if((lineImg = loadImage(temp))                 == NULL) return -17;
	
	sprintf(temp,"%s%s", dir,"etoile.png");
	if((etoileImg = loadImage(temp))               == NULL) return -18;
	
	sprintf(temp,"%s%s", dir,"public.png");
	if((publicImg = loadImage(temp))               == NULL) return -19;
	
	sprintf(temp,"%s%s", dir,"fond.png");
	if((fond = loadImage(temp))                    == NULL) return -20;
	
	sprintf(temp,"%s%s", dir,"logoguitar.png");
	if((logo = loadImage(temp))                    == NULL) return -21;
	
	sprintf(temp,"%s%s", dir,"font.pgf");
	if((font = intraFontLoad(temp,INTRAFONT_STRING_UTF8)) == NULL) return -22;
	
	sprintf(temp,"%s%s", dir,"touchesList.png");
	if((touchesListImg = loadImage(temp))                    == NULL) return -23;
	
	color0 = GU_COLOR( 0.1f, 0.9f, 0.1f, 0.0f );
	color1 = GU_COLOR( 0.9f, 0.1f, 0.1f, 0.0f );
	color2 = GU_COLOR( 0.9f, 0.9f, 0.1f, 0.0f );
	color3 = GU_COLOR( 0.1f, 0.1f, 0.9f, 0.0f );
	color4 = GU_COLOR( 0.9f, 0.1f, 0.9f, 0.0f );
	
	hightColor0 = GU_COLOR( 0.0f, 1.0f, 0.0f, 0.0f );
	hightColor1 = GU_COLOR( 1.0f, 0.0f, 0.0f, 0.0f );
	hightColor2 = GU_COLOR( 1.0f, 1.0f, 0.0f, 0.0f );
	hightColor3 = GU_COLOR( 0.0f, 0.0f, 1.0f, 0.0f );
	hightColor4 = GU_COLOR( 1.0f, 0.0f, 1.0f, 0.0f );
	
	missedColor = GU_COLOR( 0.7f, 0.7f, 0.7f, 0.5f );
	
	return 0;
}

void unloadData(){
	for(int i = 0; i < 5; i++)
		freeImage(touchesImg[i]);
	for(int i = 0; i < 5; i++)
		freeImage(notesImg[i]);
	for(int i = 0; i < 5; i++)
		freeImage(flammeImg[i]);
	freeImage(noteMiss);
	freeImage(lineImg);
	freeImage(etoileImg);
	freeImage(publicImg);
	freeImage(fond);
	freeImage(logo);
	intraFontUnload(font);
	freeImage(touchesListImg);
}
