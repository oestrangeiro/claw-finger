#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "../includes/camera.h"
#include "../includes/control_states.h"


/*
	Configura minha câmera customizada, a câmera padrão do Raylib é horrível
*/
void SetCustomCamera(Camera3D *cam){

	cam->position 	= (Vector3) {-0.42f, 37.15f, -35.24f};
	cam->target 	= (Vector3) {0.42f, 36.78f, -35.0f};
	cam->up 		= (Vector3)	{0.0f, 1.0f, 0.0f};
	cam->fovy		= 80.0f;
	cam->projection	= CAMERA_PERSPECTIVE;

}

void UpdateCustomCameraMovement(Camera3D *cam, ControlState input) {
    float speed = 35.0f;
    float delta = GetFrameTime();
    float velocity = speed * delta;

    static float yaw   = -90.0f; 
    static float pitch = 0.0f;
    float targetSensivity = 0.1f;
    
    Vector3 forward = Vector3Subtract(cam->target, cam->position);
    forward = Vector3Normalize(forward);
    
    // Movimento Lateral (Strafing)
    Vector3 right = Vector3CrossProduct(forward, cam->up);

    // Eixo X
    if (input.cameraMoveDirection.x != 0) {
        float factor = input.cameraMoveDirection.x * velocity;
        cam->position = Vector3Add(cam->position, Vector3Scale(right, factor));
        cam->target   = Vector3Add(cam->target, Vector3Scale(right, factor));
    }

    // Eixo Z 
    if (input.cameraMoveDirection.z != 0) {
        float factor = input.cameraMoveDirection.z * velocity;
        cam->position = Vector3Add(cam->position, Vector3Scale(forward, factor));
        cam->target   = Vector3Add(cam->target, Vector3Scale(forward, factor));
    }

    // Eixo Y (Cima/Baixo)
    if (input.cameraMoveDirection.y != 0) {
        float factor = input.cameraMoveDirection.y * velocity;
        cam->position.y += factor;
        cam->target.y   += factor;
    }

    /*
    	Rotação do mouse
    */
    yaw   += input.mouseDelta.x * targetSensivity;
    pitch -= input.mouseDelta.y * targetSensivity;

    if(pitch > 89.0f)  pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    Vector3 lookingForward;
    lookingForward.x = cosf(DEG2RAD * yaw) * cosf(DEG2RAD * pitch);    
    lookingForward.y = sinf(DEG2RAD * pitch);
    lookingForward.z = sinf(DEG2RAD * yaw) * cosf(DEG2RAD * pitch);

    cam->target = Vector3Add(cam->position, Vector3Normalize(lookingForward));
}