#ifndef __EXPRESS_H
#define __EXPRESS_H

#include "express_console_menu.h"
#include "express_wifi.h"

typedef void (*PuzzlePiece_t)();
// typedef class (*PuzzlePiece_t)();
typedef struct {
	PuzzlePiece_t action;
	// const bool *enabled;
} piece_t;

class express : public express_console_menu, public express_wifi {
public:



	const piece_t *_pieces;


	
	void setup(const express* e){
	  // void Joinerr  = &y;
	}

	 void printInfo(){
		// express_console_menu::v().pln("join file.");
		// #ifdef __EXPRESS_WIFI_H
		// express_wifi::printInfo();
		// #endif
		express_console_menu::printInfo();
	}

};

#endif // __PIECES_H