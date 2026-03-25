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

		[BUG FIX]:
			@date: 23/03/2026
			@author: Mateus 'oestrangeiro' Almeida
			Era possível alterar o eixo Y do modelo sem que ele estivesse selecionado, bastava selecionar uma vez e desselecionar
			que o ponteiro apontava para o endereço do modelo e na hora de processar os inputs do usuário referentes à alteração
			do eixo Y do modelo (teclas Y e B), eu não checava se o campo isSelected estava true, apenas estava fazendo um
			null checking em ptrSelectedEntity. Antes de subir pro github, vou fazer mais alguns testes.

		[Bugs]:
			@date: 23/03/2026
			@author: Mateus 'oestrangeiro' Almeida

			1) Valor de mudança de escala do modelo e valor de distância percorrida no eixo Y estão, ambas, estáticas, sendo
			que elas devem se adequar ao tamanho do modelo. Talvez a nona ou décima parte?
			distance 		= ptrSelectedEntity->model->scale * 0.1;
			sensivityScale 	= ptrSelectedEntity->model->scale * 0.1;

			2) A bounding box do modelo só está sendo atualizada no momento do grab mode com o deslocamento do modelo com o mouse.
			Se o usuário deslocar o modelo no eixo Y e/ou modificar a escala do mesmo, a BB não é atualizada, sendo necessário que
			eu ative o grab mode e desloque o modelo para atualizar a BB. Talvez seja necessário eu fazer uma função que recebe o
			ponteiro do modelo e altere a BB à cada frame!


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


#define MAX_ENTITIES_IN_WORLD 	1
#define FONT_SIZE 				15

// Tiles da grid desenhada no chão
#define NUM_OF_TILES			100
#define	SIZE_OF_TILES			10

// Enum pra caralho kkkkkk
typedef enum {
	DISABLE_CURSOR = 0,
	ENABLE_CURSOR
} Cursor;

typedef enum {
	DISABLE_WIREFRAME_MODE = 0,
	ENABLE_WIREFRAME_MODE
} Wireframe;

typedef enum {
	MODEL_HIT = 1
} ModelFlags;

// Protótipo das funções
// TODO: Jogar num header depois
void SetWindowAttributes(Window *w);
void render_model(Model model, Vector3 *modelPosition, float rotation, Color color);
void reset_model_position(Vector3 *modelPosition);
// void camera_custom_movement(Camera3D *cam);
void UpdateCustomCameraMovement(Camera3D *cam, ControlState input);
void DrawOriginLineAxis(float thickness, int slices);


int main(void){

	int modelsLoaded					= 0;
	static int cursorState 				= DISABLE_CURSOR;
	static int wireFrameState			= DISABLE_WIREFRAME_MODE;
	static int modelHit 				= 0;
	static float rotationX 				= 0.0f, rotationY = 0.0f, rotationZ = 0.0f;
	unsigned int numOfTrianglesInModel  = 0;
	int toggleGrabModelState 			= 0;
	
	Vector2 mousePosition = { 0.0f, 0.0f };
	

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
    	DrawOriginLineAxis(0.05f, 20);

		// Desenhando o Modelo com ou sem wireframe
		if(wireFrameState == ENABLE_WIREFRAME_MODE){
			for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
				DrawModelWires(arrayEntity[i].model, arrayEntity[i].positionInWorld, arrayEntity[i].scale, GREEN);
				DrawBoundingBox(
					arrayEntity[i].bounds, RED
					);
				// Desenhando o ponto central do model
				DrawSphere(arrayEntity[i].centerPoint, 0.5f, RED);
					
			}
			// printf("[!] Wireframe ativado!\n");
		}else{
			for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
				DrawModel(
					arrayEntity[i].model,
					arrayEntity[i].positionInWorld,
					arrayEntity[i].scale,
					arrayEntity[i].isSelected == true ? RED : WHITE
				);
			}
			// printf("[!] Wireframe desativado!\n");
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
						printf(":[Debug]: '%s' Grab model state: TRUE\n", ptrSelectedEntity->name);
						Ray ray = GetMouseRay(GetMousePosition(), cam);

					    
					    if(ray.direction.y != 0){
					    	float distance = -ray.position.y / ray.direction.y; 

					    	if(distance > 0){
					    		Vector3 hitPoint = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));

					    		ptrSelectedEntity->positionInWorld.x = hitPoint.x;
						        ptrSelectedEntity->positionInWorld.z = hitPoint.z;

						        
						        BoundingBox modelBox 		  = GetModelBoundingBox(ptrSelectedEntity->model);

						        if(ptrSelectedEntity->scale != 1.0f){
							        modelBox.min 			      = Vector3Scale(modelBox.min, ptrSelectedEntity->scale);
							        modelBox.max 			      = Vector3Scale(modelBox.max, ptrSelectedEntity->scale);
						        }

						        ptrSelectedEntity->bounds.min = Vector3Add(modelBox.min, ptrSelectedEntity->positionInWorld);
						        ptrSelectedEntity->bounds.max = Vector3Add(modelBox.max, ptrSelectedEntity->positionInWorld);

						        // Ajustando o center point
						        ptrSelectedEntity->centerPoint = CalculateTheCenterPointOfModel(ptrSelectedEntity->bounds.min, ptrSelectedEntity->bounds.max);
					    	}
					    }
						
					}else{
						printf(":[Debug]: '%s' Grab model state: FALSE\n", ptrSelectedEntity->name);
						toggleGrabModelState = 0;
						// ptrSelectedEntity->isSelected = false;
					}
				}
			// }
		}

		/*
			Scale do modelo
		*/

		if((ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected) && (currentInput.getMouseWheelToScaleModel != 0)){
			// umentando a escala do modelo
			float sensivityScale = 1.0f;
			float newScaleToModel =  (currentInput.getMouseWheelToScaleModel * sensivityScale);

			// evitando que o modelo fique de cabeça pra baixo 
			if( ((ptrSelectedEntity->scale) + newScaleToModel) >= 0.0000f){
				ptrSelectedEntity->scale += newScaleToModel;
			}	
		}

		/*
			Movimento do modelo no eixo Y
			(Espero que funcione de primeira kkkk)
		*/
		if((ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected) && (currentInput.moveModelInYAxisToUp || currentInput.moveModelInYAxisToDown)){

			/*
				Acho que seria interessante calcular a distância com base na escala atual do modelo,
				mas por hora, vai ficar assim mesmo
			*/
			float distance = 1.15f;

			Vector3 movementInYAxis = {0.0f, 0.0f, 0.0f};

			if(currentInput.moveModelInYAxisToUp) {
				printf("::[Debug]:: Movimento para cima!\n");
				movementInYAxis.y += distance;
				ptrSelectedEntity->positionInWorld = Vector3Add(ptrSelectedEntity->positionInWorld, movementInYAxis);
			}

			if(currentInput.moveModelInYAxisToDown) {
				printf("::[Debug]:: Movimento para baixo!\n");
				movementInYAxis.y += distance;
				ptrSelectedEntity->positionInWorld = Vector3Subtract(ptrSelectedEntity->positionInWorld, movementInYAxis);
			}

		}

		DrawGrid(NUM_OF_TILES, SIZE_OF_TILES);
		
		// FIM 
		EndMode3D();
		
		int paddingBottom = 15;
		
		/*
			Desenhando texto pra debug na tela. Depois faço uma função LogInfo pra isso
		*/
		if(ptrSelectedEntity != NULL && ptrSelectedEntity->isSelected){
			
			DrawText(TextFormat("[!] Model '%s' selected! Filepath: '%s'", ptrSelectedEntity->name, ptrSelectedEntity->pathToFileModel), 5, (paddingBottom * 1) + FONT_SIZE, FONT_SIZE, LIME);
			DrawText(TextFormat("[!] Position: X: %.2f Y: %.2f Z: %.2f ", ptrSelectedEntity->positionInWorld.x, ptrSelectedEntity->positionInWorld.y, ptrSelectedEntity->positionInWorld.z ), 5, (paddingBottom * 2) + FONT_SIZE, FONT_SIZE, LIME);
			DrawText(TextFormat("[!] Number of polygons in model : %ld", ptrSelectedEntity->triangles), 5, (paddingBottom * 3) + FONT_SIZE, FONT_SIZE, LIME);
			DrawText(TextFormat("[!] Scale : %.2f", ptrSelectedEntity->scale), 5, (paddingBottom * 4) + FONT_SIZE, FONT_SIZE, LIME);
			DrawText(TextFormat("[!] GRAB MODE: %s", toggleGrabModelState == 1 ? "true" : "false"), 5, (paddingBottom * 5) + FONT_SIZE, FONT_SIZE, LIME);
		}
        
		DrawText(TextFormat("[!] FPS: %d", GetFPS()), 5, 150, 20, RED);
   		EndDrawing();
   		
   	 }	 // Fim do loop while


   	for(int i = 0; i < MAX_ENTITIES_IN_WORLD; i++){
   		UnloadModel(arrayEntity[i].model);
   	}

    CloseWindow();
    

    return 0;
}


// Não usada
void reset_model_position(Vector3 *modelPosition){
	// Origem
	Vector3 originCoordenates = { 0.0f, 0.0f, 0.0f };
	
	modelPosition->x = originCoordenates.x;
	modelPosition->y = originCoordenates.y;
	modelPosition->z = originCoordenates.z;

	printf("[!] Posição do modelo resetada para: X:%.2f  Y:%.2f  Z:  %.2f\n", modelPosition->x, modelPosition->y, modelPosition->z);
}	

// Não usada
void render_model(Model model, Vector3 *modelPosition, float rotation, Color color){

	DrawModelEx(
				model,
				(Vector3) { modelPosition->x, modelPosition->y, modelPosition->z },
				(Vector3) {0, 1, 0},
				rotation,
				(Vector3) {1, 1, 1},
				color
	);
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

	// DrawLine3D(originPoint, limitPointXToLeft,   BLUE);
	// DrawLine3D(originPoint, limitPointXToRight,  BLUE);
	// DrawLine3D(originPoint, limitPointYToLeft,   GREEN);
	// DrawLine3D(originPoint, limitPointYToRight,  GREEN);
	// DrawLine3D(originPoint, limitPointZToUp, 	 ORANGE);
	// DrawLine3D(originPoint, limitPointZToBottom, ORANGE);
}