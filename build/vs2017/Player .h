#pragma once
#include "game_object.h"
#include "primitive_builder.h"
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>

class Player : public GameObject
{
public:
	Player();
	~Player();
	void initialisePlayer(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform);

	b2Body* playerBody;
	b2FixtureDef playerFixtureDef;
	b2BodyDef playerBodyDef;
	b2CircleShape playerShape;
	b2Fixture* playerFixture;
	gef::Material playerMaterial;
};

