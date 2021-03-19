#include "Player.h"



Player::Player()
{
}


Player::~Player()
{
}

void Player::initialisePlayer(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform)
{
	playerMaterial.set_colour(0xff404040);
	
	// Set the physical behaviours of the player body
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position = b2Vec2(0.5f, 0.5f);
	playerBodyDef.linearDamping = 0.25f;
	playerBodyDef.angularDamping = 2.0f;
	playerBody = world->CreateBody(&playerBodyDef); // Create the body within the world using the defines behaviours

	// Create the shape for the player
	playerShape.m_p.Set(0, 0);
	playerShape.m_radius = 1.0f;

	// Create the fixture on the rigid body
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1.0f;
	playerFixture = playerBody->CreateFixture(&playerFixtureDef);

	playerBody->SetFixedRotation(false);
	// Setup the mesh for the player	
	//set_mesh(primitiveBuilder->CreateSphereMesh(1.3f, 20, 20, gef::Vector4(0, 0, 0), &playerMaterial));
	//set_mesh(model);
	//set_mesh(playerModel.mesh());

	// Update visuals from simulation data
	UpdateFromSimulation(playerBody);

}
