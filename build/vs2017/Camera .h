#pragma once
#include <maths\matrix44.h>
#include "box2d\Box2D.h"
#include "maths\math_utils.h"
#include <vector>
class Camera
{
public:
	Camera();
	~Camera();

	void InitialiseCamera();
	void Update(b2Vec2 playerPosition, float frameTime, bool gameOver, bool gameWon);
	void changeDistance(float velocity, float frameTime);
	gef::Vector4 getCameraEye();
	gef::Vector4 getCameraLookAt();
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;
	float fov;
	float lerp = 3.8f;
	float deathLerp = 5.0f;
	float xOffset = 0;
	float distance = 10;
	float maxDistance = 45;
	float midDistance = 36;
	float minDistance = 30;
	float distanceAcceleration = 0;
	float maxDistanceAcceleration = 0.2f;
	float shakeSpeedx = 40.0f;
	float shakeSpeedy = 30.0f;
	float shakex, shakey;


private:
	gef::Vector4 cameraEye;
	gef::Vector4 cameraLookat;
	gef::Vector4 cameraUp;
};

