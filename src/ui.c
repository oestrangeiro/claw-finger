#include <string.h>
#include "../includes/ui.h"



/*
	Função para desenhar um retângulo translucido. A ideia é simular
	uma textbox
*/

void DrawTextBox(TextBox textBox, char* textBoxInfo, Font font){

    strcpy(textBox.textInfo, textBoxInfo);

	DrawRectangle(
        textBox.textBoxRectangle.x,
        textBox.textBoxRectangle.y,
        textBox.textBoxRectangle.width,
        textBox.textBoxRectangle.height,
        textBox.textBoxColor
    );
	
	DrawTextEx(
				font,
				textBox.textInfo,
				(Vector2) {textBox.textBoxRectangle.x,textBox.textBoxRectangle.y},
				FONT_SIZE_OF_TEXTBOX,
				1.0f,
				BLUE
		);
	

}

void DrawButton(
        const char* buttonText,
        float buttonPositionX,
        float buttonPositionY,
        float buttonWidth,
        float buttonHeight,
        Color buttonColor
    ){


}

/*
	As linhas dos eixos X, Y e Z partindo da origem.
	Como o raylib não permite que ajustemos a "grossura" das linhas,
	a manha aqui é desenhar cilindros e colorir eles pra simular 3
	linhas partindo da origem
*/
void DrawOriginLineAxis(float thickness, int slices){

	float maxDistanceFromOriginPoint = (NUM_OF_TILES * SIZE_OF_TILES) / 2;

	// Ponto comum dos três eixos
	Vector3 originPoint = {0.0f, 0.0f, 0.0f};

	Vector3 limitPointXToLeft  		= {-maxDistanceFromOriginPoint, 0.0f, 0.0f};
	Vector3 limitPointXToRight  	= {maxDistanceFromOriginPoint, 0.0f, 0.0f};

	Vector3 limitPointYToUp			= {0.0f, maxDistanceFromOriginPoint, 0.0f};
	Vector3 limitPointYToBottom		= {0.0f, -maxDistanceFromOriginPoint, 0.0f};

	Vector3 limitPointZToRight 		= {0.0f, 0.0f, maxDistanceFromOriginPoint};
	Vector3 limitPointZToLeft 		= {0.0f, 0.0f, -maxDistanceFromOriginPoint};

	// Eixo X
	DrawCylinderEx(
		originPoint,
		limitPointXToRight,
		thickness,
		thickness, 
		slices,
		RED
	);
	DrawCylinderEx(
		originPoint,
		limitPointXToLeft,
		thickness,
		thickness,
		slices,
		RED
	);

	// EIXO Y
	DrawCylinderEx(
		originPoint,
		limitPointYToUp,
		thickness,
		thickness, 
		slices,
		BLUE
	);
	DrawCylinderEx(
		originPoint,
		limitPointYToBottom,
		thickness,
		thickness,
		slices,
		BLUE
	);

	// Eixo Z
	DrawCylinderEx(
		originPoint,
		limitPointZToRight,
		thickness,
		thickness, 
		slices,
		LIME
	);
	DrawCylinderEx(
		originPoint,
		limitPointZToLeft,
		thickness,
		thickness,
		slices,
		LIME
	);
}

