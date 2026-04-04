/*
	Simples visualizador de modelos 3D em C com Raylib 5.5
	@author: Mateus 'oestrangeiro' Almeida
	@date: 18/03/2026
*/
/*----------------------------------------------------------------------------------------------------------------------------------------------------
	TODO:
		1) Implementar o sistema de rotação dos modelos; ao menos a rotação no próprio eixo. [X]
		2) Implementar uma câmera própria, pois a câmera padrão é uma porcaria [X]
		3) Após a adição da câmera nova, implementar os modos de câmera:
			3.1) Free Cam: Câmera Livre (WASD) + Mouse [X]
			3.2) Orbital: Câmera que gire em torno de um objeto/modelo (Aparentemente, mais complicada de implementar que a Free Cam)
		4) Seleção de modelos com o mouse [X]
			4.1) Ao selecionar o modelo com o mouse, dispor informações na tela, estas sendo:
				4.1.1) Nome do arquivo do modelo [X]
				4.1.2) Posição do modelo (X, Y, Z) [X]
				4.1.3) Número de polígonos do modelo [X]
			4.2) Alterar a posição do modelo no eixo Y (alguns modelos estão spawnando abaixo do chão) [X]
		5) Drag 'N' Drop de modelos para dentro do visualizador
		6) Abstração dos modelos para uma struct Entity [X]
		7) Implementar lista ligada simples para que seja possível adicionar N modelos na cena em tempo de execução
		8) Fazer um debug com as informações do modelo mais legível (vide itens 4.1.x)
		9) Fazer uma text box com informações sobre fps e posição da câmera no mundo
			9.1) Procurar uma fonte legalzinha pra enfiar na textbox
		10) Implementar aceleração ao deslocar modelos


		[BUG FIX]:
			@date: 23/03/2026
			@author: Mateus 'oestrangeiro' Almeida

			1) Era possível alterar o eixo Y do modelo sem que ele estivesse selecionado, bastava selecionar uma vez e desselecionar
			que o ponteiro apontava para o endereço do modelo e na hora de processar os inputs do usuário referentes à alteração
			do eixo Y do modelo (teclas Y e B), eu não checava se o campo isSelected estava true, apenas estava fazendo um
			null checking em ptrSelectedEntity. Antes de subir pro github, vou fazer mais alguns testes.
			
			2) A bounding box do modelo só está sendo atualizada no momento do grab mode com o deslocamento do modelo com o mouse.
			Se o usuário deslocar o modelo no eixo Y e/ou modificar a escala do mesmo, a BB não é atualizada, sendo necessário que
			eu ative o grab mode e desloque o modelo para atualizar a BB. Talvez seja necessário eu fazer uma função que recebe o
			ponteiro do modelo e altere a BB à cada frame

		[Bugs]:
			@date: 23/03/2026
			@author: Mateus 'oestrangeiro' Almeida

			1) Valor de mudança de escala do modelo e valor de distância percorrida no eixo Y estão, ambas, estáticas, sendo
			que elas devem se adequar ao tamanho do modelo. Talvez a nona ou décima parte?
			distance 		= ptrSelectedEntity->model->scale * 0.1;
			sensivityScale 	= ptrSelectedEntity->model->scale * 0.1;

			@date: 29/03/2026
			1) Não é possível alterar o eixo Y de um modelo sem antes alterar a escala do mesmo. Não faço a mínima ideia do por quê.


	[*] Nunca esquecer:
		Só programe aquilo que for usar e matenha as coisas estupidamente simples!
----------------------------------------------------------------------------------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#include "includes/window.h"
#include "includes/camera.h"
#include "includes/control_states.h"
#include "includes/input.h"
#include "includes/entity.h"
#include "includes/ui.h"

#define DEBUG_MODE 0

#define MAX_ENTITIES_IN_WORLD 		1
#define NUM_OF_TILES				100
#define	SIZE_OF_TILES				10
#define NUM_OF_FACES_ORIGIN_LINES 	20
#define THICKNESS_OF_ORIGIN_LINES	0.05f

typedef enum {
	DISABLE_CURSOR = 0,
	ENABLE_CURSOR
} Cursor;

typedef enum {
	DISABLE_WIREFRAME_MODE = 0,
	ENABLE_WIREFRAME_MODE
} Wireframe;

int main(void){

	int modelsLoaded					= 0;
	static int cursorState 				= DISABLE_CURSOR;
	static int wireFrameState			= DISABLE_WIREFRAME_MODE;
	static int modelHit 				= 0;
	static float rotationX 				= 0.0f, rotationY = 0.0f, rotationZ = 0.0f;
	int toggleGrabModelState 			= 0;
	static float sensivityScale			= 0.0f;
	

    Window window, *w;
    w = &window;
	Camera3D cam;

	/*
		Funções inicalizadoras.
		Em caso de curiosidade, veja:
			src/camera.c
			src/window.c
	*/ 
	SetCustomCamera(&cam);
	SetWindowAttributes(w);
	
    InitWindow(w->screenWidth, w->screenHeight, w->title);
    SetTargetFPS(w->framesPerSecond);



	/*
		Tudo o que for aparecer na tela deve ser carregado a partir daqui
	*/
   char bufferToTextBox[64];
   Font font = LoadFont("assets/fonts/cascadia.mono.ttf");

   TextBox textBox = {
		.textBoxColor 		= {
			0, 0, 0, 
			256 * 0.85 // 85% de opacidade (== 10% de transparência) do fundo da textbox (depois faço um código melhor)
		},
		.textBoxTextColor 	= LIME,
		.textBoxRectangle 	= { .x = 0, .y = SCREEN_H - 70, .height = 70, .width = 320}
   };


	Entity arrayEntity[MAX_ENTITIES_IN_WORLD];
	Entity e1 = InitializeEntity("assets/models/Creative_Commons/Lycaste_Virginalis/Lycaste_virginalis-150k.gltf", "plant_model", (Vector3) {10.0f, 10.0f, 0.0f}, 50.0f);
	arrayEntity[0] = e1;
	Entity *ptrSelectedEntity = NULL;

    DisableCursor();
    
    // Main game loop
    while (!WindowShouldClose()){

    	// Pega o estado atual de decisão à cada frame
    	ControlState currentInput = GetCurrentState();

    	/*
    		PROCESSAMENTO DOS INPUTS
    	*/
    	// Cursor
    	if(currentInput.toggleCursor){

    		cursorState = !cursorState;

    		if(cursorState == ENABLE_CURSOR){ EnableCursor(); GetMouseDelta(); }
    		else{
    			DisableCursor();
    		}

    	}

    	// Wireframe
    	if(currentInput.toggleWireFrameMode){
    		wireFrameState = !wireFrameState;
    	}

    	/*
    		RENDERIZAÇÃO
    	*/
        
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

		
		BeginMode3D(cam);
		
		// Grossura, Numero de faces
    	DrawOriginLineAxis(THICKNESS_OF_ORIGIN_LINES, NUM_OF_FACES_ORIGIN_LINES);

		// Desenhando o Modelo com ou sem wireframe
		if(wireFrameState == ENABLE_WIREFRAME_MODE){
			for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
				DrawModelWires(arrayEntity[i].model, arrayEntity[i].positionInWorld, arrayEntity[i].scale, GREEN);
				DrawBoundingBox(arrayEntity[i].bounds, RED);
				DrawSphere(arrayEntity[i].centerPoint, 0.5f, RED);
					
			}
		}
		else{
			for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
				DrawModel(
					arrayEntity[i].model,
					arrayEntity[i].positionInWorld,
					arrayEntity[i].scale,
					arrayEntity[i].isSelected == true ? RED : WHITE
				);
			}
		}


		/*
			Selecionando os modelos com o click esquerdo
		*/

		if(cursorState == DISABLE_CURSOR){ UpdateCustomCameraMovement(&cam, currentInput); }
		
		else{
			
			if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
				
				Ray r = GetMouseRay(currentInput.mousePosition,  cam);
				
				for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){

					RayCollision hit = GetRayCollisionBox(r, arrayEntity[i].bounds);

					// Acertei a bounding box "geral" do modelo
					if(hit.hit){

						ptrSelectedEntity = &arrayEntity[i];

						if(ptrSelectedEntity->isSelected == false){ 
							ptrSelectedEntity->isSelected = !ptrSelectedEntity->isSelected;
							break;
						}
						else{
						/*
							Se o modelo já tiver sido clicado, ele marca como "não
							acertado"
						*/ 
							ptrSelectedEntity->isSelected = false;
							break;
						}

					}

				}

			}

			// Se o modelo tiver selecionado, checo se o G está pressionado
			// for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
				if(ptrSelectedEntity != NULL){

					if(ptrSelectedEntity->isSelected && currentInput.grabModel){
						toggleGrabModelState = 1;

						Ray ray = GetMouseRay(GetMousePosition(), cam);

					    /*
							Se o raio (do mouse) acertou o chão enquanto seguro G,
							movimento o modelo para onde o mouse aponta
						*/
					    if(ray.direction.y != 0){
					    	float distance = -ray.position.y / ray.direction.y; 

					    	if(distance > 0){
					    		Vector3 hitPoint = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));

					    		ptrSelectedEntity->positionInWorld.x = hitPoint.x;
						        ptrSelectedEntity->positionInWorld.z = hitPoint.z;

								UpdateBoundingBoxOfModel(ptrSelectedEntity);
								
						        // Ajustando o center point
						        ptrSelectedEntity->centerPoint = CalculateTheCenterPointOfModel(ptrSelectedEntity->bounds.min, ptrSelectedEntity->bounds.max);
					    	}
					    }
						
					}else{
						#if DEBUG
							printf(":[Debug]: '%s' Grab model state: FALSE\n", ptrSelectedEntity->name);
						#endif
						toggleGrabModelState = 0;
					}
				}
			// }
		}

		/*
			Scale do modelo
		*/

		if((ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected) && (currentInput.getMouseWheelToScaleModel != 0)){
			// aumentando a escala do modelo
			sensivityScale = ptrSelectedEntity->scale * 0.1f;

			#if DEBUG_MODE
				printf("::[Debug]:: sensivityScale: %.5f\n", sensivityScale);
			#endif

			float newScaleToModel =  (currentInput.getMouseWheelToScaleModel * sensivityScale);

			// evitando que o modelo fique de cabeça pra baixo 
			if( ((ptrSelectedEntity->scale) + newScaleToModel) >= 0.0000f){
				ptrSelectedEntity->scale += newScaleToModel;
				UpdateBoundingBoxOfModel(ptrSelectedEntity);
			}
			
		}
		
		/*
			Reset da posição do modelo para a origem (0, 0, 0)
		*/
		if((ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected) && currentInput.resetModelPosition){
			ResetEntityPosition(ptrSelectedEntity);
			UpdateBoundingBoxOfModel(ptrSelectedEntity);
		}

		/*
			Movimento do modelo no eixo Y
		*/
		if((ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected) && (currentInput.moveModelInYAxisToUp || currentInput.moveModelInYAxisToDown)){

			/*
				Acho que seria interessante calcular a distância com base na escala atual do modelo,
				mas por hora, vai ficar assim mesmo
			*/
			float distance = sensivityScale;

			Vector3 movementInYAxis = {0.0f, 0.0f, 0.0f};

			if(currentInput.moveModelInYAxisToUp) {
				movementInYAxis.y += distance;
				ptrSelectedEntity->positionInWorld = Vector3Add(ptrSelectedEntity->positionInWorld, movementInYAxis);
				UpdateBoundingBoxOfModel(ptrSelectedEntity);
				// Ajustando o center point
				ptrSelectedEntity->centerPoint = CalculateTheCenterPointOfModel(ptrSelectedEntity->bounds.min, ptrSelectedEntity->bounds.max);
			}

			if(currentInput.moveModelInYAxisToDown) {
				movementInYAxis.y += distance;
				ptrSelectedEntity->positionInWorld = Vector3Subtract(ptrSelectedEntity->positionInWorld, movementInYAxis);
				UpdateBoundingBoxOfModel(ptrSelectedEntity);
				// Ajustando o center point
				ptrSelectedEntity->centerPoint = CalculateTheCenterPointOfModel(ptrSelectedEntity->bounds.min, ptrSelectedEntity->bounds.max);
			}

		}

		DrawGrid(NUM_OF_TILES, SIZE_OF_TILES);
		
		// FIM 
		EndMode3D();
		
		// Desenhando a fonte (debug)
		
		snprintf(
			bufferToTextBox,
			sizeof(bufferToTextBox),
			"FPS: %d\nCamera position: %.2f, %.2f, %.2f",
			GetFPS(),
			cam.position.x,
			cam.position.y,
			cam.position.z
		);

		DrawTextBox(textBox, bufferToTextBox, font);
   		EndDrawing();
   		
   	 }	 // Fim do loop while


   	for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
   		UnloadModel(arrayEntity[i].model);
   	}

	UnloadFont(font);

    CloseWindow();
    

    return 0;
}
