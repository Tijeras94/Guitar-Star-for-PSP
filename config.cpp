/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#define CONFIG_FILE_NAME "config"

#include <config.h>

int strlower(char *str) {

  char *cptr=NULL;

  for (cptr=str; *cptr; cptr++) *cptr=tolower(*cptr);

  return 1;

}

namespace config{
	char pseudo[64];
	char skin[64];
	bool touchesCircle[5],
	     touchesSquare[5],
	     touchesCross[5],
	     touchesTriangle[5],
	     touchesUp[5],
	     touchesDown[5],
	     touchesLeft[5],
	     touchesRight[5],
	     touchesR[5],
	     touchesL[5];
	
	void str2touches(char* txt, bool* touches){
		for(int i = 0; i < 5; i++){
			touches[i] = txt[i] == '1' ? true : false;
		}
	}
	
	void loadConfig(){
		setDefautConfig();//pour prendre en compte l'absence du fichier de config ou de l'absence de certaines clés
		
		FILE * pFile;
		pFile = fopen (CONFIG_FILE_NAME,"r");
		
		if(pFile == NULL){
			saveConfig();
			return;
		}
		
		char tmp[128];
		char* key;
		char* value;
		while( fgets( tmp, 128, pFile ) != NULL ){
			if(strchr(tmp,'\n') != NULL)
				*strchr(tmp,'\n') = '\0';//on replace le saut de ligne par une fin de ligne (important pour la valeur de la clé)
			value = strchr(tmp,'=') + 2 ; //on prend seulement la valeur de la clé
			
			key = tmp;
			*strchr(key,' ') = '\0'; //on coupe la chaine au 1er espace pour avoir que la clé
			
			
			if( strcmp(key,"pseudo") == 0 ){
				strcpy(pseudo, value);
			}else if( strcmp(key,"skin") == 0 ){
				strcpy(skin, value);
			}else if( strcmp(key,"touchesCircle") == 0 ){
				str2touches(value,touchesCircle);
			}else if( strcmp(key,"touchesSquare") == 0 ){
				str2touches(value,touchesSquare);
			}else if( strcmp(key,"touchesCross") == 0 ){
				str2touches(value,touchesCross);
			}else if( strcmp(key,"touchesTriangle") == 0 ){
				str2touches(value,touchesTriangle);
			}else if( strcmp(key,"touchesUp") == 0 ){
				str2touches(value,touchesUp);
			}else if( strcmp(key,"touchesDown") == 0 ){
				str2touches(value,touchesDown);
			}else if( strcmp(key,"touchesLeft") == 0 ){
				str2touches(value,touchesLeft);
			}else if( strcmp(key,"touchesRight") == 0 ){
				str2touches(value,touchesRight);
			}else if( strcmp(key,"touchesR") == 0 ){
				str2touches(value,touchesR);
			}else if( strcmp(key,"touchesL") == 0 ){
				str2touches(value,touchesL);
			}
		}
		
		fclose (pFile);
	}
	
	void setDefautConfig(){
		sprintf(pseudo, "pseudo");
		sprintf(skin, "default");
		
		for(int i = 0; i < 5; i++){
			touchesCircle[i] = false;
			touchesSquare[i] = false;
			touchesCross[i] = false;
			touchesTriangle[i] = false;
			touchesUp[i] = false;
			touchesDown[i] = false;
			touchesLeft[i] = false;
			touchesRight[i] = false;
			touchesR[i] = false;
			touchesL[i] = false;
		}
		touchesCircle[4] = true;
		touchesSquare[2] = true;
		touchesCross[3] = true;
		touchesTriangle[2] = true;
		touchesTriangle[4] = true;
		touchesUp[0] = true;
		touchesUp[1] = true;
		touchesDown[1] = true;
		touchesLeft[0] = true;
	}
	
	char* touches2str(bool* touches, char* out){
		for(int i = 0; i < 5; i++){
			out[i] = touches[i] ? '1' : '0';
		}
		out[5] = '\0';
	}
	
	bool saveConfig(){
		FILE * pFile;
		pFile = fopen (CONFIG_FILE_NAME,"w");
		
		if (pFile == NULL) return false;
		
		char tmp[128], tmp2[6];;
		
		sprintf(tmp, "pseudo = %s\n", pseudo);
		fputs (tmp,pFile);
		
		sprintf(tmp, "skin = %s\n", skin);
		fputs (tmp,pFile);
		
		touches2str(touchesCircle, tmp2);
		sprintf(tmp, "touchesCircle = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesSquare, tmp2);
		sprintf(tmp, "touchesSquare = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesCross, tmp2);
		sprintf(tmp, "touchesCross = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesTriangle, tmp2);
		sprintf(tmp, "touchesTriangle = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesUp, tmp2);
		sprintf(tmp, "touchesUp = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesDown, tmp2);
		sprintf(tmp, "touchesDown = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesLeft, tmp2);
		sprintf(tmp, "touchesLeft = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesRight, tmp2);
		sprintf(tmp, "touchesRight = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesR, tmp2);
		sprintf(tmp, "touchesR = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		touches2str(touchesL, tmp2);
		sprintf(tmp, "touchesL = %s\n", tmp2 );
		fputs (tmp,pFile);
		
		fclose (pFile);
		return true;
	}
	
	enum CONFIG_STATES{
		mainConfig,
		pseudoConfig,
		skinConfig,
		touchesConfig,
	};
	CONFIG_STATES state;
	std::vector<char*> configMenuTxt;
	std::vector<char*> skinList;
	int menuSelection;
	
	char charList[128];
	int currChar;
	
	char editedPseudo[64];
	
	bool* touchesList[10];//pointe vers les touchesCircle[5], touchesSquare[5], ... dans l'ordre du menu
	int toucheSelected, toucheValueSelected;
	
	void initConfigMenu(){
		configMenuTxt.push_back("pseudo");
		configMenuTxt.push_back("skin");
		configMenuTxt.push_back("touches");
		state = mainConfig;
		menuSelection = 0;
		
		sprintf(charList,"abcdefghijklmnopqrstuvwxyz0123456789.,;:/?!_");
				
		int dfd;
		dfd = sceIoDopen("skins");
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
						char* dirname = (char*) malloc(64);
						strcpy(dirname,dir.d_name);
						skinList.push_back(dirname);
					}
				}
			}
			sceIoDclose(dfd);
		}
		
		touchesList[0] = touchesL;
		touchesList[1] = touchesUp;
		touchesList[2] = touchesLeft;
		touchesList[3] = touchesDown;
		touchesList[4] = touchesRight;
		touchesList[5] = touchesSquare;
		touchesList[6] = touchesCross;
		touchesList[7] = touchesCircle;
		touchesList[8] = touchesTriangle;
		touchesList[9] = touchesR;
		
		toucheSelected = 0;
		toucheValueSelected = 0;
	}
	
	bool configMenu(){
		if(state == mainConfig){
			if (pad::scroll_down()){
				if(menuSelection<2){
					menuSelection++;
					motionDown();
				}
			}
			if (pad::scroll_up()){
				if(menuSelection>0){
					menuSelection--;
					motionUp();
				}
			}
			if (pad::one_cross()){
				if(menuSelection == 0){
					resetMotionMenu();
					state = pseudoConfig;
					menuSelection = 0;
					
					strcpy(editedPseudo,pseudo);
					
				}else if(menuSelection == 1){
					resetMotionMenu();
					state = skinConfig;
					menuSelection = 0;
					
				}else if(menuSelection == 2){
					resetMotionMenu();
					state = touchesConfig;
					menuSelection = 0;
				}
			}
			if (pad::one_triangle()){
				return false;
			}
			
			printMenu(configMenuTxt,menuSelection);
		}else if(state == pseudoConfig){
			//la position dans la liste des caractères est stocké dans menuSelection
			
			if (pad::scroll_down()){
				if(charList[menuSelection+1] != '\0'){
					menuSelection++;
				}else{
					menuSelection = 0;
				}
			}
			if (pad::scroll_up()){
				if(menuSelection>0){
					menuSelection--;
				}else{
					while(charList[menuSelection+1] != '\0'){
						menuSelection++;
					}
				}
			}
			if (pad::one_right()){
				if(strlen(editedPseudo) < 64){
					int pos = strlen(editedPseudo);
					editedPseudo[pos] = charList[menuSelection];
					editedPseudo[pos+1] = '\0';
				}
			}
			if (pad::one_left()){
				if(strlen(editedPseudo) > 0){
					editedPseudo[strlen(editedPseudo)-1] = '\0';
				}
			}
			
			if (pad::one_triangle()){
				resetMotionMenu();
				state = mainConfig;
				menuSelection = 0;
			}
			
			if (pad::one_cross()){
				resetMotionMenu();
				state = mainConfig;
				menuSelection = 0;
				sprintf(pseudo, editedPseudo);
				saveConfig();
			}
			
			if( (clock()/200000)%2 == 0 ){
				fontPrintf(font, 10,144, true, editedPseudo);
			}else{
				fontPrintf(font, 10,144, true,"%s%c", editedPseudo, charList[menuSelection]);
			}
			
			fontPrintRC(font, 10, 200, 460,18, true, "haut/bas pour choisir un caractère, droite pour l'ajouter, gauche pour retirer le dernier caractère, croix pour valider, triangle pour annuler");
			
		}else if(state == skinConfig){
			if (pad::scroll_down()){
				if( menuSelection < (skinList.size()-1) ){
					menuSelection++;
					motionDown();
				}
			}
			if (pad::scroll_up()){
				if(menuSelection>0){
					menuSelection--;
					motionUp();
				}
			}
			
			if (pad::one_triangle()){
				resetMotionMenu();
				state = mainConfig;
				menuSelection = 0;
			}
			
			if (pad::one_cross()){
				strcpy(skin, skinList.at(menuSelection));
				unloadData();
				loadData();
				saveConfig();
				
				resetMotionMenu();
				state = mainConfig;
				menuSelection = 0;
			}
			
			printMenu(skinList,menuSelection);
		}else if(state == touchesConfig){
			if (pad::scroll_down()){
				if(toucheValueSelected>0){
					toucheValueSelected--;
				}
			}
			if (pad::scroll_up()){
				if(toucheValueSelected<4){
					toucheValueSelected++;
				}
			}
			if (pad::one_left()){
				if(toucheSelected>0){
					toucheSelected--;
				}
			}
			if (pad::one_right()){
				if(toucheSelected<9){
					toucheSelected++;
				}
			}
			
			if (pad::one_cross()){
				touchesList[toucheSelected][toucheValueSelected] = !touchesList[toucheSelected][toucheValueSelected];
			}
			
			if (pad::one_triangle()){
				resetMotionMenu();
				state = mainConfig;
				menuSelection = 0;
				saveConfig();
			}
			
			imageRender(touchesListImg,0,0);
			for(int i = 0; i < 5; i++){
				for(int j = 0; j < 10; j++){
					if(toucheSelected == j && toucheValueSelected == i){
						imageRender(touchesList[j][i] ? notesImg[i] : noteMiss ,48*j + 1, 47 + i*45, 45, 45);
					}else{
						imageRender(touchesList[j][i] ? notesImg[i] : noteMiss ,48*j + 6, 52 + i*45, 35, 35);
					}
				}
			}
		}
		
		return true;
	}
}
