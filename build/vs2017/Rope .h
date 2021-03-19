#pragma once
#include "game_object.h"
#include "Player.h"
#include <box2d/Rope/b2Rope.h>
#include <array>
#include <graphics\sprite.h>
#include "loadTexture.h"

class Rope : public GameObject
{
public:
	Rope();
	~Rope();

	void InitialiseRope(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform);
	void ropePhysics(b2World* world, b2Body* player, b2Body* surface, PrimitiveBuilder* primitiveBuilder, float dist, b2Vec2 precisePoint, bool ropeExists, float frameTime);
	void AdjustRope(float frameTime, float leftY, float leftX);

	b2Vec2 getPositionA();
	b2Vec2 getPositionB();

	float distance = 6.0f;
	float maxDistance = 15.0f;
	float minDistance = 3.0f;
	float currentRayAngle = 0;
	float ropeAcceleration = 0.0f;
	float maxRopeAcceleration = 0.09f;
	float minRopeAcceleration = 0.0f;
	float currentDist = 0;
	
	gef::Texture* ropeTex;
	gef::Sprite ropeSprite;

	bool ropeShot = true;
	bool extending;
	bool still = true;
	bool needNewRope = true;
	bool firstGrapple = false;

	b2Vec2 start;
	b2Vec2 end;

	gef::Material ropeMaterial;

	gef::Mesh* currentMesh;

	b2DistanceJoint* joint;
	b2Body* ropeBody;

private:
	b2DistanceJointDef jointDef;
	
	b2BodyDef ropeBodyDef;
	b2EdgeShape ropeShape;
	b2FixtureDef ropeFixtureDef;
	b2Fixture* ropeFixture;
};

