#include "Raycast.h"



Raycast::Raycast()
{
}

Raycast::~Raycast()
{
}

void Raycast::Cast(b2World* world, gef::Platform& platform, b2Body* player, float rightX, float rightY, float frameTime)
{
	if (rightX > 0.7 || rightX < -0.7 || rightY > 0.7 || rightY < -0.7)
	{
		currentRayAngle = (atan2(rightY, -rightX));
		raycastActive = true;
	}
	else
		raycastActive = false;

		//calculate points of ray
		p1 = player->GetPosition(); //center of scene
		p2 = p1 - rayLength * b2Vec2(cosf(currentRayAngle), sinf(currentRayAngle) );

		// set up input
		b2RayCastInput input;
		input.p1 = p1;
		input.p2 = p2;
		input.maxFraction = 1;

		//check every fixture of every body to find closest
		float closestFraction = 1; //start with end of line as p2
		b2Vec2 intersectionNormal(0, 0);
		for (currentBody = world->GetBodyList(); currentBody; currentBody = currentBody->GetNext()) {
			for (currentFixture = currentBody->GetFixtureList(); currentFixture; currentFixture = currentFixture->GetNext())
			{
				b2RayCastOutput output;
				if (!currentFixture->RayCast(&output, input, 0))
					continue;
				if (currentBody == player)
					break;
				if (output.fraction < closestFraction) 
				{
					closestFraction = output.fraction;
					intersectionNormal = output.normal;
					validBody = currentBody;
					validGrapple = true;
				}
			}
		}

		intersectionPoint = p1 + closestFraction * (p2 - p1);

		if (closestFraction == 1)
			validGrapple = false;
		if (closestFraction == 0)
			validGrapple = false;

	
}
