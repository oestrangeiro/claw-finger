#include "../includes/window.h"
#include <string.h>

void SetWindowAttributes(Window *window){

	window->posX = 0;
	window->posY = 0;
	window->screenWidth = SCREEN_W;
	window->screenHeight = SCREEN_H;
	window->framesPerSecond = FPS;
	strcpy(window->title, "PROTOTYPE: ClawFinger (v. 0.1) - Oestrangeiro");
	
}
