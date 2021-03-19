#include "Rope.h"
#include <box2d/Common/b2Math.h>
#include <maths/math_utils.h>


Rope::Rope()
{
}


Rope::~Rope()
{
}

void Rope::InitialiseRope(b2World* world, PrimitiveBuilder* primitiveBuilder, gef::Platform& platform)
{
	ropeMaterial.set_colour(0xff000000);
	jointDef.length = distance; // Set the length of the rope
	jointDef.collideConnected = true; // The player and the environment can collide
	jointDef.dampingRatio = 5000;
	ropeBodyDef.type = b2_staticBody;
	currentMesh = nullptr;
}

void Rope::ropePhysics(b2World* world, b2Body* playerBody, b2Body* grappleTarget, PrimitiveBuilder* primitiveBuilder, float dist, b2Vec2 precisePoint, bool ropeExists, float frameTime)
{
	if (needNewRope) // if the right bumper has been pressed but no rope exists
	{
		jointDef.localAnchorA.Set(0, 0);

		if (!firstGrapple) // if the player is initialised, attach them to the first surface
		{
			jointDef.localAnchorB.Set(-10, -10);
			firstGrapple = true; // means this loop is only played once
		}
		else
			jointDef.localAnchorB.Set(precisePoint.x - grappleTarget->GetTransform().p.x, precisePoint.y - grappleTarget->GetTransform().p.y); // set the anchor to the point that the raycast hits
		
		jointDef.bodyA = playerBody; // Set joint for player
		jointDef.bodyB = grappleTarget; // Set joint for surface
		
		ropeBody = world->CreateBody(&ropeBodyDef);
		joint = (b2DistanceJoint*)world->CreateJoint(&jointDef); //  We now have a joint between the player and the surface
		distance = dist;
		joint->SetLength(distance);

		needNewRope = false;
		ropeExists = true;
		ropeShot = false;
	}

	if (ropeExists)
	{
		if (currentDist < (distance / 2) && !ropeShot) // if rope is not as long as it should be
		{
			currentDist += (distance / 2) / 0.175f * frameTime; // increase the current length of the rope
			delete currentMesh; // delete the current rope mesh from memory
			currentMesh = primitiveBuilder->CreateBoxMesh(gef::Vector4(currentDist, 0.06f, 0.01f), gef::Vector4(-currentDist, 0, 0)); // set the current mesh, changing the length dependant on currentDist
			set_mesh(currentMesh);
		}
		else if (ropeShot) // otherwise if the rope has been fully extended to it's intended length
		{
			delete currentMesh; // delete the current rope mesh from memory
			currentMesh = primitiveBuilder->CreateBoxMesh(gef::Vector4(distance / 2, 0.06f, 0.01f), gef::Vector4(-distance / 2, 0, 0)); // set the current mesh with length matching distance
			set_mesh(currentMesh);
		}
	
		// set variables to record the anchor positions
		start.x = joint->GetAnchorA().x, start.y = joint->GetAnchorA().y;
		end.x = joint->GetAnchorB().x, end.y = joint->GetAnchorB().y;

		// set the rope's shape to be a line between anchors
		ropeShape.Set(b2Vec2(start.x, start.y), b2Vec2(end.x, end.y));
		
		// set the length of the rope joint to the current distance
		joint->SetLength(distance);

		b2Vec2 ropePos = b2Vec2((start.x + end.x) / 2, (start.y + end.y) / 2); // set the position of the rope to between the player and the surface anchors
		b2Vec2 ropeRotationCalc = b2Vec2(end.x - start.x, end.y - start.y); // calculate the rotation of the rope
		ropeBody->SetTransform(b2Vec2(start.x, start.y), atan2(-ropeRotationCalc.y, -ropeRotationCalc.x)); // transform the rope body by the calculated rope rotation and positions

		if (currentDist >= (distance / 2)) // if the current distance has reached the intended final distance
		{
			ropeShot = true; // the rope has been fully deployed so the bool is set to true
			currentDist = 0;
			delete currentMesh; // delete the current rope mesh from memory
			currentMesh = primitiveBuilder->CreateBoxMesh(gef::Vector4(distance / 2, 0.06f, 0.01f), gef::Vector4(-distance / 2, 0, 0));
			set_mesh(currentMesh);
		}
	}
}

void Rope::AdjustRope(float frameTime, float leftX, float leftY)
{
	//Make sure the values don't exceed their max or min
	if (distance < minDistance)
		distance = minDistance;
	if (distance > maxDistance)
		distance = maxDistance;
	if (ropeAcceleration > maxRopeAcceleration)
		ropeAcceleration = maxRopeAcceleration;
	if (ropeAcceleration < minRopeAcceleration)
		ropeAcceleration = minRopeAcceleration;

	//If the left joystick is moved up or down a certain amount
	if (distance >= 2 && distance <= maxDistance && (leftY < -0.5 || leftY > 0.5))// || (keyboard->IsKeyDown(keyboard->KC_W) || keyboard->IsKeyDown(keyboard->KC_S)) ))
	{
		still = false;

		if (ropeAcceleration <= maxRopeAcceleration)
			ropeAcceleration += (0.2f * frameTime); //accelerate the speed that the rope extends or retracts


		if (leftY > 0.5f)// || keyboard->IsKeyDown(keyboard->KC_W))
		{
			if (extending == false)
				ropeAcceleration = 0;
			distance += ropeAcceleration; // extend
			extending = true;
		}

		if (leftY < -0.5)// || keyboard->IsKeyDown(keyboard->KC_S))
		{
			if (extending == true)
				ropeAcceleration = 0;
			distance -= ropeAcceleration; // retract
			extending = false;
		}

		joint->SetLength(distance); //Set the length to the distance calculated
	}
	else
	{
		if (!still)
		{
			if (ropeAcceleration > 0) // If rope is accelerating 
			{
				ropeAcceleration -= (0.2f * frameTime); // Decelerate to a stop

				if (extending)
				{
					distance = distance + ropeAcceleration; // If rope is extending slow down in that direction
				}

				if (!extending)
				{
					distance = distance - ropeAcceleration; // Otherwise change the distance in the opposite direction
				}

				jointDef.length = distance; // Set length
			}

			if (ropeAcceleration == 0) // If rope isn't changing length
			{
				still = true;
			}
		}
	}
}

b2Vec2 Rope::getPositionA()
{
	return b2Vec2(distance / 2, 0.01f);
}

b2Vec2 Rope::getPositionB()
{
	return b2Vec2(-distance / 2, 0);
}
