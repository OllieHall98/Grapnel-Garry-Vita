#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::InitialiseCamera()
{
	// setup camera
	fov = gef::DegToRad(45.0f);

	cameraEye = gef::Vector4(-2.0f, 2.0f, 10.0f);
	cameraLookat = gef::Vector4(0.0f, 0.0f, 0.0f);
	cameraUp = gef::Vector4(0.0f, 1.0f, 0.0f);
}

void Camera::Update(b2Vec2 playerPosition, float frameTime, bool gameOver, bool gameWon)
{
	gef::Vector4 position = cameraLookat;
	if (!gameOver && !gameWon)
	{
		if (xOffset < 6)
			xOffset += 4 * frameTime;

		position.set_x(position.x() + (playerPosition.x + xOffset - position.x()) * lerp * frameTime);
		position.set_y(position.y() + (playerPosition.y - position.y()) * lerp * frameTime);
		cameraEye.set_value(position.x() + 2, position.y(), distance);
		cameraLookat.set_value(position.x(), position.y(), 0.f);
		view_matrix.LookAt(cameraEye, cameraLookat, cameraUp);
	}
	else if ((gameOver || gameWon) && distance > 7.5f)
	{
		position.set_x(position.x() + (playerPosition.x - position.x()) * deathLerp * frameTime);
		position.set_y(position.y() + (playerPosition.y - position.y()) * deathLerp* frameTime);
		cameraEye.set_value(playerPosition.x, playerPosition.y, distance);
		cameraLookat.set_value(position.x(), position.y(), 0.f);
		distance -= distanceAcceleration;
		view_matrix.LookAt(cameraEye, cameraLookat, cameraUp);
	}

}

void Camera::changeDistance(float velocity, float frameTime)
{
	if (distanceAcceleration > maxDistanceAcceleration)
		distanceAcceleration = maxDistanceAcceleration;

	distanceAcceleration += (0.1f * frameTime);

	if (distance > maxDistance)
	{
		distance -= distanceAcceleration;
	}

	if (distance < minDistance)
	{
		distance += (distanceAcceleration / 2);
	}

	if (velocity < 16 && distance > minDistance)
	{
		distance -= distanceAcceleration;
		distanceAcceleration -= (0.1f * frameTime);
	}

	else if (velocity < 28 && velocity > 16)
	{
		if (distance > midDistance)
			distance -= distanceAcceleration;
		if (distance < midDistance)
			distance += distanceAcceleration;

		distanceAcceleration -= (0.1f * frameTime);
	}

	else if (velocity >= 28 && distance < maxDistance)
	{
		distance += distanceAcceleration;
		distanceAcceleration -= (0.1f * frameTime);
	}
}

gef::Vector4 Camera::getCameraEye()
{
	return cameraEye;
}

gef::Vector4 Camera::getCameraLookAt()
{
	return cameraLookat;
}
