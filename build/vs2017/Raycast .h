#pragma once
#include <box2d\Dynamics\b2Fixture.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include "system/debug_log.h"
class Raycast : public b2Fixture
{
public:
	Raycast();
	void Cast(b2World* world, gef::Platform& platform, b2Body* player, float rightX, float rightY, float frameTime);
	~Raycast();

	float currentRayAngle = 0;
	bool validGrapple;
	bool raycastActive;

	b2Vec2 p1;
	b2Vec2 p2;

	b2RayCastInput input;
	b2Vec2 intersectionPoint;
	b2Body* validBody;
	float rayLength = 15; //long enough to hit the walls
	int distance = 5;

private:
	b2BodyDef raycastBodyDef;
	b2Body* currentBody;
	
	b2Fixture* currentFixture;
	

};

