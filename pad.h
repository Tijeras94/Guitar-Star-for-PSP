/**********************************************
 * This file is a part of GuitarStar by festi *
 *        http://flash.festi.free.fr/         *
 **********************************************/

#ifndef PAD_H
#define PAD_H

#define SCROLL_HOLD_TIME 500000

#include <pspctrl.h>
#include <time.h>

namespace pad{
	
	void init();	
	
	void updatePad();
	
	bool up();
	bool down();
	bool left();
	bool right();
	bool cross();
	bool circle();
	bool square();
	bool triangle();
	bool L();
	bool R();
	bool start();
	bool select();
	
	bool one_up();
	bool one_down();
	bool one_left();
	bool one_right();
	bool one_cross();
	bool one_circle();
	bool one_square();
	bool one_triangle();
	bool one_L();
	bool one_R();
	bool one_start();
	bool one_select();
	
	bool scroll_up();
	bool scroll_down();
}

#endif
