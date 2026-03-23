#include <string.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "../includes/entity.h"

#define ROTATION_SPEED 		1.0
#define DEFAULT_SCALE_MODEL 1.0
#define FOLDER_TO_MODELS 	"assets/models/"


Entity InitializeEntity( char* fileModel, char* name, Vector3 positionInWorld, float scaleModel){


	Entity entity = { 0 };

	strcpy(entity.pathToFileModel, fileModel);
	strcpy(entity.name, name);
	

	entity.rotation 		  		= 0;
	entity.rotationSpeed 	  		= ROTATION_SPEED;
	entity.positionInWorld    		= positionInWorld;
	entity.scale			  		= scaleModel;
	entity.isBoudingBoxActive 		= false;
	entity.isDeleted		  		= false;
	entity.isSelected		  		= false;

	entity.model 			  		= LoadModel(entity.pathToFileModel);
	BoundingBox currentBoundingBox 	= GetModelBoundingBox(entity.model);
	currentBoundingBox.min 			= Vector3Scale(currentBoundingBox.min, scaleModel);
	currentBoundingBox.max 			= Vector3Scale(currentBoundingBox.max, scaleModel);

	entity.triangles		  		= CalculateNumOfTrianglesInModel(entity);

	entity.bounds.min 		  		= Vector3Add(currentBoundingBox.min, entity.positionInWorld);
	entity.bounds.max 		  		= Vector3Add(currentBoundingBox.max, entity.positionInWorld);

	entity.centerPoint 				= CalculateTheCenterPointOfModel(entity.bounds.min, entity.bounds.max);

	return entity;
}

unsigned int CalculateNumOfTrianglesInModel(Entity entity){
	
	unsigned int trianglesInEntityModel = 0;

	for(int i = 0; i < entity.model.meshCount; i++){

		Mesh currentMeshInModel = entity.model.meshes[i];


		trianglesInEntityModel += currentMeshInModel.vertexCount / 3;
	}


	return trianglesInEntityModel;

}

Vector3 CalculateTheCenterPointOfModel(Vector3 entityBoundsMin, Vector3 entityBoundsMax){
	Vector3 centerPointOfModel = {0};

	centerPointOfModel 		= Vector3Add(entityBoundsMin, entityBoundsMax);
	centerPointOfModel.x 	= centerPointOfModel.x / 2.0f;
	centerPointOfModel.y 	= centerPointOfModel.y / 2.0f;
	centerPointOfModel.z 	= centerPointOfModel.z / 2.0f;

	// printf("Ponto Central do modelo: X: %.2f  Y: %.2f  Z: %.2f\n", centerPointOfModel.x, centerPointOfModel.y, centerPointOfModel.z);

	return centerPointOfModel;
}