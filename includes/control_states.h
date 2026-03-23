#ifndef CONTROL_STATES_H
#define CONTROL_STATES_H

/*
	Um handler simples para implementar uma máquina de estados, pois meu código
	no arquivo main.c está uma porcaria com aquela macarronada de if's.
	A ideia é implementar tudo que o jogador pode "decidir" fazer em um frame
	do jogo em um lugar só (No caso, numa struct só).
*/ 

typedef struct {

	// Movimento da câmera (w, a, s, d)
	Vector3 cameraMoveDirection;

	/*
		Movimento do modelo 3D 
		Eixos:
			X: Setas pra esquerda ou direita
			Y: Seta pra cima ou para baixo
			Z: Q ou E

		[!] Ainda não implementei o deslocamento do modelo no espaço, ou seja,
		ele só se movimenta nos próprio eixos
	*/

	// Dados do mouse
	Vector2 mousePosition;
	Vector2 mouseDelta;

	// Flags
	bool resetCameraPosition; 	// Tecla C {0.0f, 0.0f, 0.0f}
	bool resetModelPosition;	// Tecla R {0.0f, 0.0f, 0.0f}

	/* 
		Dependendo do modo do cursor, troco o modo de câmera.
		No momento existem dois modos:
			- Free Cam: 
				Permite que o usuário navegue livremente pelo mundo

			- Ui Picking:
				Trava a posição e o target da câmera, mas permite que o usuario
				interaja com modelos no mundo. Até o momento, o usuário só pode
				selecionar e "desselecionar" o modelo.

	*/ 
	bool toggleCursor;	// Tecla Tab
	bool mouseLeftClick;
	bool toggleWireFrameMode; // F1
	bool isFileDropped; // Mecânica de Drag n Drop
	bool grabModel;		// Tecla G


	float getMouseWheelToScaleModel;

} ControlState;

#endif