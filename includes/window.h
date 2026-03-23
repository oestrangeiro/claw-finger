#ifndef WINDOW_H
#define WINDOW_H

#define SCREEN_H 		640
#define SCREEN_W 		780
#define FPS 			60

typedef struct {

	int posX, posY;
	int screenHeight, screenWidth;
	int	framesPerSecond;
	char title[60];
	
} Window;

void SetWindowAttributes(Window *w);

#endif