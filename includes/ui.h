#ifndef UI_H
#define UI_H

#include "raylib.h"

#define FONT_SIZE_OF_TEXTBOX    15
#define FONT_SIZE_OF_BUTTON     12

// Tiles da grid desenhada no chão
#define NUM_OF_TILES			100
#define	SIZE_OF_TILES			10

typedef struct Textbox {
    char        textInfo[64];
    Rectangle   textBoxRectangle;
    Color       textBoxColor;
    Color       textBoxTextColor;
} TextBox;

typedef struct Button {
    char        textButton;
    bool        clicked;
    bool        mouseHover;
    Rectangle   buttonRectangle;
    Color       buttonColor;
    Color       buttonTextColor;
} Button;

void DrawTextBox(TextBox textBox, char* textBoxInfo);
void DrawButton(const char* buttonText, float buttonPositionX, float buttonPositionY, float buttonWidth, float buttonHeight, Color buttonColor);
void DrawOriginLineAxis(float thickness, int slices);

#endif
