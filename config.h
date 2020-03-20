/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "pad.h"
#include "gfunctions.h"
#include "data.h"
#include <ctype.h> // contains tolower() function

int strlower(char *str);

namespace config{
	extern  char pseudo[64];
	extern  char skin[64];
	extern  bool touchesCircle[5],
		     touchesSquare[5],
		     touchesCross[5],
		     touchesTriangle[5],
		     touchesUp[5],
		     touchesDown[5],
		     touchesLeft[5],
		     touchesRight[5],
		     touchesR[5],
		     touchesL[5];
	
	void loadConfig();
	
	bool saveConfig();
	
	void setDefautConfig();
	
	void initConfigMenu();
	
	bool configMenu();//retourne false si la config est finie (le state de main de retourner a la valeur mainMenu)
}

#endif
