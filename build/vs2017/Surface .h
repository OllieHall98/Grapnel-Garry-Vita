#pragma once
#include "game_object.h"
#include "primitive_builder.h"
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <assets/png_loader.h>
#include "graphics/mesh.h"
#include <graphics/mesh_instance.h>



class Surface :
	public GameObject
{
public:
	Surface();
	~Surface();
	void initialiseSurface(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, gef::Vector4 size, b2Vec2 pos);
	void initialiseSphereSurface(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, float radius, b2Vec2 pos);
	void InitialiseDefusalZone(b2World* world, PrimitiveBuilder* primitiveBuilder, b2Vec2 pos);
	void InitialiseBackground(b2World* world, PrimitiveBuilder* primitiveBuilder);
	void InitialiseCrate(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, gef::Vector4 size, b2Vec2 pos);
	void elevator(float frameTime);
	float elevatorSpeed = 2;
	float elevatorSpeedAcceleration = 0.12f;
	float maxAcceleration = 6.0f;
	bool goDown = false;
	PrimitiveBuilder* primitiveBuilder;
	gef::Mesh* surfaceMesh;
	b2Body* surfaceBody;
	b2BodyDef surfaceBodyDef;
	b2FixtureDef surfaceFixtureDef;

	GameObject defusalZone;
	b2Body* defusalZoneBody;
	b2BodyDef defusalZoneBodyDef;
	b2FixtureDef defusalZoneFixtureDef;

	gef::Texture* tex;
};

