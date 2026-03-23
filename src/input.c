#include "raylib.h"
#include <stdio.h>

#include "../includes/control_states.h" // Struct ControlState
#include "../includes/input.h"


/*
	A minha ideia é capturar o estado de decisão do jogador
	a cada frame
*/

ControlState GetCurrentState(void){

	ControlState currentControlState = { 0 };


	// Movimento da câmera
	if(IsKeyDown(KEY_W)) { currentControlState.cameraMoveDirection.z += 1; }
	if(IsKeyDown(KEY_A)) { currentControlState.cameraMoveDirection.x -= 1; }
	if(IsKeyDown(KEY_S)) { currentControlState.cameraMoveDirection.z -= 1; }
	if(IsKeyDown(KEY_D)) { currentControlState.cameraMoveDirection.x += 1; }

	// Movimento de subir e descer no eixo Y
	if(IsKeyDown(KEY_SPACE)) { currentControlState.cameraMoveDirection.y += 1; }
	if(IsKeyDown(KEY_LEFT_CONTROL)) { currentControlState.cameraMoveDirection.y -= 1; }


	// As flags que vão me servir como gatilhos
	
	currentControlState.resetCameraPosition = IsKeyPressed(KEY_C);
	currentControlState.resetModelPosition 	= IsKeyPressed(KEY_R);

	currentControlState.mousePosition 		= GetMousePosition();
	currentControlState.mouseDelta			= GetMouseDelta();
	currentControlState.mouseLeftClick		= IsKeyPressed(MOUSE_LEFT_BUTTON);

	currentControlState.toggleCursor		= IsKeyPressed(KEY_TAB);

	currentControlState.toggleWireFrameMode	= IsKeyPressed(KEY_F1);

	currentControlState.isFileDropped		= IsFileDropped();

	/*
		Flags do modelo

		Vou me ""inspirar"" no Blender (i.e "Copiar").
		Se o modelo for selecionado E enquanto o usuário segurar a tecla G
		o modelo pode ser reposicionado na cena com base na posição do mouse.
		Espero que o Raylib tenha alguma função que me retorne um Vector3
		na cena com base no delta do mouse e na posição do mouse
	*/

	currentControlState.grabModel 					= IsKeyDown(KEY_G);
	currentControlState.getMouseWheelToScaleModel	= GetMouseWheelMove();


	return currentControlState;
}