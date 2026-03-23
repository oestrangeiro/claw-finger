#ifndef CAMERA_H
#define CAMERA_H

#include "control_states.h"

// Ainda não usado
typedef enum {
	CAMERA_INITIALIZED = 0
} CameraFlags;

void SetCustomCamera(Camera3D *cam);
void UpdateCustomCameraMovement(Camera3D *cam, ControlState input);

#endif
