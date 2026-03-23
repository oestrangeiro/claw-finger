#ifndef ENTITY_H
#define ENTITY_H


/*
	Uma leve abstração pros modelos do raylib.
	Ao invés de eu ter que fazer no arquivo main.c:
		Model marioModel = LoadModel("Foo");
		Model gunModel	 = LoadModel("Bar")	;

	Faço um array (no futuro vou implementar listas ligadas) de Entity.
	Em outras palavras, vou guardar aqui os "meta-dados" das entidades
*/

typedef struct {

	/*
		O caminho padrão dos modelos, na minha máquina, é
		assets/models/PASTA_COM_NOME_DO_MODELO/ARQUIVO_MODELO.EXTENSÃO
	*/ 
	unsigned int triangles;
	char pathToFileModel[256];
	char name[32]; 				//apenas para debug
	float rotation;
	float rotationSpeed;
	float scale;
	bool isBoudingBoxActive; 	// condição para renderizar a bounding box
	bool isDeleted;
	bool isSelected;

	Model model;
	Vector3 positionInWorld; 	// Posição do modelo na cena
	Vector3 centerPoint;		// Posição do centro de massa do modelo
	BoundingBox bounds;

} Entity;

/*
	Uma função "construtora" da entidade (Beijos para POO)
*/
Entity InitializeEntity(char* fileModel, char* name, Vector3 positionInWorld, float scaleModel);

/*
	Não vou julgar sua inteligência...
*/
unsigned int CalculateNumOfTrianglesInModel(Entity entity);

Vector3 CalculateTheCenterPointOfModel(Vector3 entityBoundsMin, Vector3 entityBoundsMax);


#endif