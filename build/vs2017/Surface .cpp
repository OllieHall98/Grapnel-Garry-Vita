#include "Surface.h"



Surface::Surface()
{
}


Surface::~Surface()
{
}

void Surface::initialiseSurface(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, gef::Vector4 size, b2Vec2 pos)
{
	// setup the mesh for the ground
	set_mesh(primitiveBuilder->CreateBoxMesh(size));

	// create a physics body
	surfaceBodyDef.type = b2_staticBody;
	surfaceBodyDef.position = pos;
	surfaceBody = world->CreateBody(&surfaceBodyDef);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(size.x(), size.y());

	// create the fixture
	surfaceFixtureDef.shape = &shape;

	// create the fixture on the rigid body
	surfaceBody->CreateFixture(&surfaceFixtureDef);

	// update visuals from simulation data
	UpdateFromSimulation(surfaceBody);
}

void Surface::initialiseSphereSurface(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, float radius, b2Vec2 pos)
{
	// create a physics body
	surfaceBodyDef.type = b2_staticBody;
	surfaceBodyDef.position = pos;
	surfaceBody = world->CreateBody(&surfaceBodyDef);

	// create the shape
	b2CircleShape shape;
	shape.m_radius = radius;
	//shape.SetAsBox(size.x(), size.y());

	// create the fixture
	surfaceFixtureDef.shape = &shape;

	// create the fixture on the rigid body
	surfaceBody->CreateFixture(&surfaceFixtureDef);

	// update visuals from simulation data
	UpdateFromSimulation(surfaceBody);
}

void Surface::elevator(float frameTime)
{
	elevatorSpeedAcceleration += (0.04f * frameTime);

	if (surfaceBody->GetPosition().y <= -42)
	{
		goDown = false;
		elevatorSpeedAcceleration = 0.06f;
	}
		//surfaceBody->SetTransform(b2Vec2(0, -20), 0);
	if (surfaceBody->GetPosition().y >= 14)
	{
		goDown = true;
		elevatorSpeedAcceleration = 0.06f;
	}
		//surfaceBody->SetTransform(b2Vec2(0, 50), 0);
	if (elevatorSpeedAcceleration > maxAcceleration)
		elevatorSpeedAcceleration = maxAcceleration;
	if (elevatorSpeedAcceleration < 0)
		elevatorSpeedAcceleration = 0;
	
	if (goDown)
	{
		surfaceBody->SetTransform(b2Vec2(surfaceBody->GetPosition().x, surfaceBody->GetPosition().y - elevatorSpeedAcceleration), 0);
	}
	if (!goDown)
	{
		surfaceBody->SetTransform(b2Vec2(surfaceBody->GetPosition().x, surfaceBody->GetPosition().y + elevatorSpeedAcceleration), 0);
	}
	
}

void Surface::InitialiseDefusalZone(b2World* world, PrimitiveBuilder* primitiveBuilder, b2Vec2 pos)
{
	//defusalZone.set_mesh(primitiveBuilder->CreateBoxMesh(gef::Vector4(9.75f, 12.0f, 2.0f, 0)));
	defusalZoneBodyDef.position = pos;
	defusalZoneBodyDef.type = b2_staticBody;
	defusalZoneBody = world->CreateBody(&defusalZoneBodyDef);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(9.75, 12.0f);

	// create the fixture
	defusalZoneFixtureDef.shape = &shape;
	defusalZoneFixtureDef.isSensor = true;

	defusalZoneBody->CreateFixture(&defusalZoneFixtureDef); 

	defusalZone.UpdateFromSimulation(defusalZoneBody);
}

void Surface::InitialiseBackground(b2World* world, PrimitiveBuilder* primitiveBuilder)
{
	// setup the mesh for the ground
	//set_mesh(primitiveBuilder->CreateBoxMesh(gef::Vector4(20.0f, 20.0f, 1.0f, 0)));

	// create a physics body
	surfaceBodyDef.type = b2_staticBody;
	surfaceBody = world->CreateBody(&surfaceBodyDef);

	// update visuals from simulation data
	UpdateFromSimulation(surfaceBody);
}

void Surface::InitialiseCrate(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform, gef::Vector4 size, b2Vec2 pos)
{
	// setup the mesh for the ground
	set_mesh(primitiveBuilder->CreateBoxMesh(size));

	// create a physics body
	surfaceBodyDef.type = b2_staticBody;
	surfaceBodyDef.position = pos;
	surfaceBody = world->CreateBody(&surfaceBodyDef);

	// update visuals from simulation data
	UpdateFromSimulation(surfaceBody);
}


