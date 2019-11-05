/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#include "pad.h"

namespace pad{
	SceCtrlData pad, lastpad; //VARIABLES FOR CONTROLS
	clock_t t_up, t_down;
	
	void init(){
		sceCtrlReadBufferPositive(&pad, 1); //READ STARTUP CONTROLS
	}
	
	void updatePad(){
		lastpad = pad;
		sceCtrlReadBufferPositive(&pad, 1);
		
		if(!up()){
			t_up = clock();
		}
		if(!down()){
			t_down = clock();
		}
	}
	
	bool up(){
		return pad.Buttons & PSP_CTRL_UP;
	}
	bool down(){
		return pad.Buttons & PSP_CTRL_DOWN;
	}
	bool left(){
		return pad.Buttons & PSP_CTRL_LEFT;
	}
	bool right(){
		return pad.Buttons & PSP_CTRL_RIGHT;
	}
	bool cross(){
		return pad.Buttons & PSP_CTRL_CROSS;
	}
	bool circle(){
		return pad.Buttons & PSP_CTRL_CIRCLE;
	}
	bool square(){
		return pad.Buttons & PSP_CTRL_SQUARE;
	}
	bool triangle(){
		return pad.Buttons & PSP_CTRL_TRIANGLE;
	}
	bool L(){
		return pad.Buttons & PSP_CTRL_LTRIGGER;
	}
	bool R(){
		return pad.Buttons & PSP_CTRL_RTRIGGER;
	}
	bool start(){
		return pad.Buttons & PSP_CTRL_START;
	}
	bool select(){
		return pad.Buttons & PSP_CTRL_SELECT;
	}
	
	bool one_up(){
		return (pad.Buttons & PSP_CTRL_UP) && !(lastpad.Buttons & PSP_CTRL_UP);
	}
	bool one_down(){
		return (pad.Buttons & PSP_CTRL_DOWN) && !(lastpad.Buttons & PSP_CTRL_DOWN);
	}
	bool one_left(){
		return (pad.Buttons & PSP_CTRL_LEFT) && !(lastpad.Buttons & PSP_CTRL_LEFT);
	}
	bool one_right(){
		return (pad.Buttons & PSP_CTRL_RIGHT) && !(lastpad.Buttons & PSP_CTRL_RIGHT);
	}
	bool one_cross(){
		return (pad.Buttons & PSP_CTRL_CROSS) && !(lastpad.Buttons & PSP_CTRL_CROSS);
	}
	bool one_circle(){
		return (pad.Buttons & PSP_CTRL_CIRCLE) && !(lastpad.Buttons & PSP_CTRL_CIRCLE);
	}
	bool one_square(){
		return (pad.Buttons & PSP_CTRL_SQUARE) && !(lastpad.Buttons & PSP_CTRL_SQUARE);
	}
	bool one_triangle(){
		return (pad.Buttons & PSP_CTRL_TRIANGLE) && !(lastpad.Buttons & PSP_CTRL_TRIANGLE);
	}
	bool one_L(){
		return (pad.Buttons & PSP_CTRL_LTRIGGER) && !(lastpad.Buttons & PSP_CTRL_LTRIGGER);
	}
	bool one_R(){
		return (pad.Buttons & PSP_CTRL_RTRIGGER) && !(lastpad.Buttons & PSP_CTRL_RTRIGGER);
	}
	bool one_start(){
		return (pad.Buttons & PSP_CTRL_START) && !(lastpad.Buttons & PSP_CTRL_START);
	}
	bool one_select(){
		return (pad.Buttons & PSP_CTRL_SELECT) && !(lastpad.Buttons & PSP_CTRL_SELECT);
	}
	
	bool scroll_up(){
		if(one_up()){
			return true;
		}
		return up() && ( clock() - t_up > SCROLL_HOLD_TIME);
	}
	bool scroll_down(){
		if(one_down()){
			return true;
		}
		return down() && ( clock() - t_down > SCROLL_HOLD_TIME);
	}
}
