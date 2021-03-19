#include "Level.h"
#include <input/input_manager.h>
#include <input\sony_controller_input_manager.h>
#include <input\keyboard.h>
#include <maths/vector2.h>
#include <box2d/Box2D.h>
#include <box2d/Common/b2Draw.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <maths/math_utils.h>
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/renderer_3d.h>
#include <graphics/sprite.h>
#include <graphics/texture.h>
#include <graphics/font.h>
#include "loadTexture.h"
#include <ctime>
#include "menu.h"

Level::Level(gef::Platform& plat) :
	platform(plat),
	spriteRenderer(NULL),
	renderer3d(NULL),
	primitiveBuilder(NULL),
	input(NULL),
	audioManager(NULL),
	font(NULL),
	world(NULL)
{
}

Level::~Level()
{
}

void Level::onEnter()
{
	spriteRenderer = gef::SpriteRenderer::Create(platform);
	renderer3d = gef::Renderer3D::Create(platform); // create the renderer for draw 3D geometry
	primitiveBuilder = new PrimitiveBuilder(platform); // initialise primitive builder to make create some 3D geometry easier
	input = gef::InputManager::Create(platform);
	audioManager = gef::AudioManager::Create(); // initialise audio manager

	// set defaults
	timeScale = 1.0;
	currentRayAngle = 0;
	distance = 3;
	duration = 0;
	winDuration = 0;
	gamePaused = false;
	ropeExists = true;
	gameOver = false;
	gameOver2 = false;
	gameEnded = false;
	gameWon = false;
	gameOverClockStarted = false;
	gameWonClockStarted = false;
	gameInitiated = false;
	slowTime = false;
	hasGasped = false;

	// set textures
	defuserTexture = CreateTextureFromPNG("surfaceTexture.png", platform);
	surfaceTexture = CreateTextureFromPNG("surface.png", platform);
	controlsTexture = CreateTextureFromPNG("controls.png", platform);
	controlsTextureCross = CreateTextureFromPNG("controlsCross.png", platform);
	controlsTextureLB = CreateTextureFromPNG("controlsLB.png", platform);
	controlsTextureRB = CreateTextureFromPNG("controlsRB.png", platform);
	controlsTextureLBCross = CreateTextureFromPNG("controlsLBCross.png", platform);
	controlsTextureRBCross = CreateTextureFromPNG("controlsRBCross.png", platform);
	controlsTextureLBRBCross = CreateTextureFromPNG("controlsLBRBCross.png", platform);
	controlsTextureLBRB = CreateTextureFromPNG("controlsLBRB.png", platform);

	InitialiseFont();
	SetupLights();
	InitialiseModels();

	// set the meshes for the signs in the background
	dangerSignSurface.set_mesh(GetMeshFromSceneAssets(dangerSign));
	dangerSignSurface2.set_mesh(GetMeshFromSceneAssets(dangerSign));
	elevatorSignSurface.set_mesh(GetMeshFromSceneAssets(elevatorSign));

	// initialise the physics world
	b2Vec2 gravity(0.0f, -9.81f * 2.6);
	world = new b2World(gravity);

	// initialise the camera that follows the player
	camera.InitialiseCamera();
	aspectRatio = (float)platform.width() / (float)platform.height();
	camera.projection_matrix = platform.PerspectiveProjectionFov(camera.fov, aspectRatio, 0.1f, 100.0f);

	// initialise the player and the rope values
	player.initialisePlayer(world, primitiveBuilder, platform);
	player.set_mesh(GetMeshFromSceneAssets(garryModel));
	rope.InitialiseRope(world, primitiveBuilder, platform);

	// initialise all the surfaces the player can attach to
	InitialiseSurfaces();

	// initialise the representations of the raycast
	InitialiseRaycastVisualisations();

	// create a grappling hoot
	grappleHookBody = world->CreateBody(&grappleHookBodyDef);
	grappleHook.set_mesh(primitiveBuilder->CreateSphereMesh(0.3f, 20, 20, gef::Vector4(0, 0, 0)));
	grappleHookBody->SetTransform(b2Vec2(area1[0].surfaceBody->GetPosition().x - 10, area1[0].surfaceBody->GetPosition().y - 10), 0);

	// set the starting grapple point
	grappleTarget = area1[0].surfaceBody;

	// set the sprite that shows the controls
	controlsSprite.set_texture(controlsTexture);
	controlsSprite.set_width(320);
	controlsSprite.set_height(163);
	controlsSprite.set_position(800, 90, 0);

	// set the sprite that displays the loading screen
	loadingTexture = CreateTextureFromPNG("menuLoading.png", platform);
	loading.set_texture(loadingTexture);
	loading.set_width(platform.width());
	loading.set_height(platform.height());
	loading.set_position(platform.width() / 2.0f, platform.height() / 2.0f, 0);

	// set the sprite that displays the background
	backgroundTexture = CreateTextureFromPNG("background.png", platform);
	backgroundSprite.set_texture(backgroundTexture);
	backgroundSprite.set_width(platform.width() * 2.5f);
	backgroundSprite.set_height(platform.height());
	backgroundSprite.set_position(platform.width() / 2.0f, platform.height() / 2.0f, 0);

	// set the sprite that displays the text when the game is won
	garryDefusedText = CreateTextureFromPNG("garryDefused.png", platform);
	garryDefusedSprite.set_texture(garryDefusedText);
	garryDefusedSprite.set_width(960);
	garryDefusedSprite.set_height(48);
	garryDefusedSprite.set_position(platform.width() / 2.0f, platform.height() / 1.25f, 0);

	// set the sprite that displays the text when the game is lost
	garryDeadText = CreateTextureFromPNG("garryIsDead.png", platform);
	garryDeadSprite.set_texture(garryDeadText);
	garryDeadSprite.set_width(960);
	garryDeadSprite.set_height(48);
	garryDeadSprite.set_position(platform.width() / 2.0f, platform.height() / 2.0f, 0);

	// set the sprite that displays the text when the game is paused
	pauseTexture = CreateTextureFromPNG("paused.png", platform);
	pauseText.set_texture(pauseTexture);
	pauseText.set_width(960);
	pauseText.set_height(48);
	pauseText.set_position(platform.width() / 2.0f, platform.height() / 2.0f, 0);

	// push all of the surface objects into an array - this allows us to texture them much easlier when in onRender()
	for (int i = 0; i < area1.size(); i++)
		objectArray.push_back(area1[i]);
	for (int i = 0; i < area2.size(); i++)
		objectArray.push_back(area2[i]);
	for (int i = 0; i < area4.size(); i++)
		objectArray.push_back(area4[i]);
	for (int i = 0; i < area3.size() - 1; i++)
		objectArray.push_back(area3[i]);
	for (int i = 0; i < crates.size(); i++)
		objectArray.push_back(crates[i]);
	objectArray.push_back(dangerSignSurface);
	objectArray.push_back(dangerSignSurface2);
	objectArray.push_back(elevatorSignSurface);

	// load in the sound and music
	if (audioManager) // if the audio manager exists
	{
		grapple = audioManager->LoadSample("sound/grapple.wav", platform);
		unGrapple = audioManager->LoadSample("sound/unGrapple.wav", platform);
		hitSurface = audioManager->LoadSample("sound/hitSurface.wav", platform);
		tickTock = audioManager->LoadSample("sound/tickTock.wav", platform);
		death = audioManager->LoadSample("sound/explosion.wav", platform);
		wahwah = audioManager->LoadSample("sound/wahwah.wav", platform);
		gasp = audioManager->LoadSample("sound/gasp.wav", platform);
		win = audioManager->LoadSample("sound/win.wav", platform);
		music = audioManager->LoadMusic("sound/music.wav", platform);
		audioManager->PlayMusic();
	}

	start = std::clock(); // start the game's clock
}

void Level::onExit()
{
	CleanUp();
}

baseState * Level::onUpdate(float frameTime)
{
	ControlInput(frameTime);
	fps_ = (1.0f / frameTime);

	// update physics world
	float32 timeStep = (1.0f / fps_ * timeScale);
	int32 velocityIterations = 4;
	int32 positionIterations = 2;

	if (!gamePaused) // only update contents when the game isn't paused
	{
		garryMaterial.set_texture(ChangeFace()); // return a texture depending on the player's speed and aim angle
		if (!gameOver) // only updates contents while the player is alive
		{
			duration = (std::clock() - start) / (float)CLOCKS_PER_SEC; // set the duration float to the duration of the shot
			world->Step(timeStep, velocityIterations, positionIterations);
			area3[5].elevator(frameTime); // call the surface function that updates the surface object that floats up and down
			camera.changeDistance(player.playerBody->GetLinearVelocity().Length(), frameTime); // call the camera function that changes the distance depending on player speed

			if (ropeExists)
				grappleHookBody->SetTransform(grapplePos, grappleRot); // set the position of the grapple hook to where the rope and surface connects

			if (raycast.validGrapple) // if the raycast returns a valid surface to hit
				rayMaterial.set_colour(0x666FFF66); // set the raycast representation to green
			else
				rayMaterial.set_colour(0x66FFFFFF); // otherwise set to grey

			rope.ropePhysics(world, player.playerBody, grappleTarget, primitiveBuilder, distance, raycast.intersectionPoint, ropeExists, frameTime); // update the physics of the rope

			rayBody->SetTransform(raycast.p1, raycast.currentRayAngle); // set the transform of the raycast representation to the raycast transform
			rayHitBody->SetTransform(raycast.intersectionPoint, raycast.currentRayAngle); // set the transform of the raycast hit representation to the place where the raycast hits

			// update the first set of surfaces
			for (int i = 0; i < area1.size(); i++)
				area1[i].UpdateFromSimulation(area1[i].surfaceBody);

			// update the second set of surfaces
			for (int i = 0; i < area2.size(); i++)
				area2[i].UpdateFromSimulation(area2[i].surfaceBody);

			// update the third set of surfaces
			for (int i = 0; i < area3.size(); i++)
			{
				if (area3[i].surfaceBody == grappleTarget)
					area3[i].set_mesh(GetMeshFromSceneAssets(grappleBallGlowing)); // set the mesh of the grapple ball if the player is attached
				else
					area3[i].set_mesh(GetMeshFromSceneAssets(grappleBall)); // else set the mesh of the grapple ball if the player is not attached

				area3[i].UpdateFromSimulation(area3[i].surfaceBody);
			}

			// update the fourth set of surfaces
			for (int i = 0; i < area4.size(); i++)
				area4[i].UpdateFromSimulation(area4[i].surfaceBody);

			//update the rest of the objects
			defusalZone.UpdateFromSimulation(defusalZone.defusalZoneBody);
			player.UpdateFromSimulation(player.playerBody);
			ray.UpdateFromSimulation(rayBody);
			rayHit.UpdateFromSimulation(rayHitBody);
			grappleHook.UpdateFromSimulation(grappleHookBody);
			rope.UpdateFromSimulation(rope.ropeBody);

			// check if the player and the surface has collided and act upon it
			checkCollisions();
		}

		if (gameOver) // if the player dies
			deathFunction(frameTime);

		if (gameWon) // if the player reaches the end
			winFunction(frameTime);

		if (gameEnded)
		{
			onRender(); // call the onRender() function again so that the loading screen is displayed
			onExit();
			if (gameWon) // if the game is won
				return new Menu(platform, winTime); // return to the menu with the recorded time
			else
				return new Menu(platform, 0); // return to the menu with no time
		}

		camera.Update(player.playerBody->GetPosition(), frameTime, gameOver, gameWon); // update the camera
	}

	return this;
} 

void Level::onRender()
{
	if (spriteRenderer)
	{
		spriteRenderer->Begin();
			spriteRenderer->DrawSprite(backgroundSprite); // draw the background before rendering anything else
		spriteRenderer->End();
	}

	if (renderer3d)
	{
		// projection
		gef::Matrix44 projection_matrix;
		projection_matrix = platform.PerspectiveProjectionFov(camera.fov, aspectRatio, 0.1f, 100.0f);
		renderer3d->set_projection_matrix(projection_matrix);

		gef::Matrix44 view_matrix;
		renderer3d->set_view_matrix(camera.view_matrix);

		// draw 3d geometry
		renderer3d->Begin(false);
			if (!gameOver2)
			{
				renderer3d->set_override_material(&garryMaterial);
				renderer3d->DrawMesh(player);

				renderer3d->set_override_material(&defusalZoneMaterial);
				renderer3d->DrawMesh(defusalZone);

				renderer3d->set_override_material(&surfaceMat);

				// Renders the objects that are inside the camera's view
				for (int i = 0; i < objectArray.size(); i++)
				{
					if ((objectArray[i].getPosition().x() < (camera.getCameraLookAt().x()) + 65) || (objectArray[i].getPosition().x() > (camera.getCameraLookAt().x() + camera.getCameraEye().x()) - 65))
						if ((objectArray[i].getPosition().y() < (camera.getCameraLookAt().y()) + 65) || (objectArray[i].getPosition().y() > (camera.getCameraLookAt().y() + camera.getCameraEye().x()) - 65))
						{
							if (i >= 12) // all objects beyond this number in the array have their own assigned textures
								renderer3d->set_override_material(0);

							renderer3d->DrawMesh(objectArray[i]); // draw the current object
						}
				}

				for (int i = 0; i < area3.size(); i++)
				{
					renderer3d->DrawMesh(area3[i]); // draw all of the surfaces in the third set
				}
			
				if (gameOver)
					renderer3d->set_override_material(&rayMaterial);

				renderer3d->set_override_material(&rope.ropeMaterial);

				if (ropeExists)
				{
					renderer3d->DrawMesh(rope); // draw the mesh only if it exists

					if (rope.ropeShot && grappleTarget != area3[5].surfaceBody) // if the player is NOT attached to the surface that moves
						renderer3d->DrawMesh(grappleHook); // render the grapple hook - the grapple hook otherwise doesn't follow the surface as it moves
				}

				renderer3d->set_override_material(&rayMaterial);

				// only render the raycast representations if the raycast is active (the player is pointing the joystick at an angle)
				if (raycast.raycastActive)
				{
					renderer3d->DrawMesh(ray);
					renderer3d->DrawMesh(rayHit);
				}
				renderer3d->set_override_material(0);
			}

			// display a black slate when the player dies
			if (gameOver2 && !gameEnded) 
			{
				renderer3d->set_override_material(&endSlateMaterial);
				renderer3d->DrawMesh(endSlate);
			}

			renderer3d->set_override_material(0);
		renderer3d->End();
	}

	if (spriteRenderer)
	{
		// start drawing sprites, but don't clear the frame buffer
		spriteRenderer->Begin(false);
			
			// only render the controls if the game isn't won or lost
			if (!gameOver2 && !gameWonClockStarted)
			{
				spriteRenderer->DrawSprite(controlsSprite);
			}
			
			// if game isn't returning to the menu
			if (!gameEnded)
			{
				// display the time taken in the level
				if (!gameOver2 && !gameWonClockStarted && font)
					font->RenderText(spriteRenderer, gef::Vector4(10.0f, 10.0f, -0.9f), 1.5f, 0xffffffff, gef::TJ_LEFT, "Time %.2f", duration);
				else if (gameOver2)
					spriteRenderer->DrawSprite(garryDeadSprite); // display this if player loses
				if (gameWon)
					spriteRenderer->DrawSprite(garryDefusedSprite); // display this if player wins
			}
				
			// if the game is about to return to the menu
			if (gameEnded)
			{
				spriteRenderer->DrawSprite(loading); // display the loading screen
			}

			// if the game is paused
			if (gamePaused)
				spriteRenderer->DrawSprite(pauseText); // display the pause text

		spriteRenderer->End();
	}
}

void Level::CleanUp()
{
	// free up audio assets
	if (audioManager)
	{
		audioManager->UnloadAllSamples();
		audioManager->UnloadMusic();
	}
	
	CleanUpFont();

	delete garryModel;
	garryModel = NULL;

	delete world;
	world = NULL;

	delete primitiveBuilder;
	primitiveBuilder = NULL;

	delete renderer3d;
	renderer3d = NULL;

	delete spriteRenderer;
	spriteRenderer = NULL;

	delete audioManager;
	audioManager = NULL;
}

void Level::ControlInput(float frameTime)
{
	if (input && !gameOver)
	{
		input->Update();

		// get controller input data for all controllers
		gef::SonyControllerInputManager* controllerInput = input->controller_input();
		if (controllerInput)
		{
			//read controlller data for controller 0
			const gef::SonyController* controller = controllerInput->GetController(0);

			if (controller)
			{
				// if the start button is pressed
				if (controller->buttons_pressed() & gef_SONY_CTRL_START)
				{
					if (gamePaused == true)
					{
						gamePaused = false; // if the game is paused, unpause the game
						audioManager->PlayMusic(); // start playing the music again
					}
					else if (gamePaused == false)
					{
						gamePaused = true; // if the game isn't paused, pause the game
						audioManager->StopMusic(); // stop the music
					}
				}

				if (!gamePaused)
				{
					rope.AdjustRope(frameTime, controller->left_stick_x_axis(), controller->left_stick_y_axis()); // call the function to adjust the rope based on player input

					raycast.Cast(world, platform, player.playerBody, controller->right_stick_x_axis(), controller->right_stick_y_axis(), frameTime); // cast the raycast depending on user input

					if (controller->buttons_pressed() & gef_SONY_CTRL_R2 && raycast.validGrapple) // Grapple
					{
						if (audioManager)
							audioManager->PlaySample(grapple);

						if (!gameInitiated)
							gameInitiated = true;

						//Destroy previous rope
						if (ropeExists)
						{
							rope.set_mesh(0);
							world->DestroyJoint(rope.joint);
							world->DestroyBody(rope.ropeBody);
							world->DestroyBody(grappleHookBody);
							ropeExists = false;
						}

						// calculate fistance between grapple point and player
						float distanceX = pow(raycast.intersectionPoint.x - raycast.p1.x, 2);
						float distanceY = pow(raycast.intersectionPoint.y - raycast.p1.y, 2);
						distance = sqrt(distanceX + distanceY);

						// set the new grapple target to the most recent body found by the raycast
						grappleTarget = raycast.validBody;

						// reset variables so that a new rope is created
						ropeExists = true;
						rope.needNewRope = true;
						raycast.validGrapple = false;

						// place a sphere where the rope and the surface connects - this is the grappling hook
						grappleHookBody = world->CreateBody(&grappleHookBodyDef);
						grapplePos = raycast.intersectionPoint;
						grappleRot = raycast.currentRayAngle;
					}

					if (controller->buttons_released() & gef_SONY_CTRL_R2 && ropeExists) // release grappling hook
					{
						if (audioManager)
							audioManager->PlaySample(unGrapple);

						if (!gameInitiated)
							gameInitiated = true;

						rope.set_mesh(0);
						world->DestroyJoint(rope.joint);
						world->DestroyBody(rope.ropeBody);
						world->DestroyBody(grappleHookBody);
						grappleTarget = 0;
						ropeExists = false;
					}

					if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS && ropeExists) // jump out of swing
					{
						player.playerBody->ApplyLinearImpulse(b2Vec2(0, 35), player.playerBody->GetPosition(), true);
						world->DestroyJoint(rope.joint);
						world->DestroyBody(rope.ropeBody);
						world->DestroyBody(grappleHookBody);
						grappleTarget = 0;
						ropeExists = false;
					}

					if (controller->buttons_down() & gef_SONY_CTRL_L2) // slow motion
						timeScale = 0.4f;
					else if (controller->buttons_released() & gef_SONY_CTRL_L2)
						timeScale = 1.0f;

					if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE) // exit game
					{
						gameEnded = true;
					}

					// change control sprite texture depending on button combination
					if (controller->buttons_down())
					{
						if ((controlsSprite.texture() != controlsTextureRB) && (controller->buttons_down() & gef_SONY_CTRL_R2))
							controlsSprite.set_texture(controlsTextureRB);
						if ((controlsSprite.texture() != controlsTextureLB) && (controller->buttons_down() & gef_SONY_CTRL_L2))
							controlsSprite.set_texture(controlsTextureLB);
						if ((controlsSprite.texture() != controlsTextureCross) && (controller->buttons_down() & gef_SONY_CTRL_CROSS))
							controlsSprite.set_texture(controlsTextureCross);
						if ((controlsSprite.texture() != controlsTextureLBCross) && (controller->buttons_down() & gef_SONY_CTRL_L2 && (controller->buttons_down() & gef_SONY_CTRL_CROSS)))
							controlsSprite.set_texture(controlsTextureLBCross);
						if ((controlsSprite.texture() != controlsTextureRBCross) && (controller->buttons_down() & gef_SONY_CTRL_R2 && (controller->buttons_down() & gef_SONY_CTRL_CROSS)))
							controlsSprite.set_texture(controlsTextureRBCross);
						if ((controlsSprite.texture() != controlsTextureLBRB) && (controller->buttons_down() & gef_SONY_CTRL_R2) && (controller->buttons_down() & gef_SONY_CTRL_L2) && !(controller->buttons_down() & gef_SONY_CTRL_CROSS))
							controlsSprite.set_texture(controlsTextureLBRB);
						if ((controlsSprite.texture() != controlsTextureLBRBCross) && (controller->buttons_down() & gef_SONY_CTRL_L2) && (controller->buttons_down() & gef_SONY_CTRL_R2) && (controller->buttons_down() & gef_SONY_CTRL_CROSS))
							controlsSprite.set_texture(controlsTextureLBRBCross);
					}
					else if ((controlsSprite.texture() != controlsTexture) && (!controller->buttons_down()))
						controlsSprite.set_texture(controlsTexture);
				}
			}
		}
	}
}

void Level::winFunction(float frameTime)
{
	garryMaterial.set_texture(garryModelHappy);

	if (!gameWonClockStarted) // only do this loop once when the player wins
	{
		if (audioManager)
		{
			audioManager->StopMusic();
			audioManager->PlaySample(win);
		}

		winTime = duration; // set the time that's passed to the menu as the current duration
		winClock = std::clock(); // begin the win clock
		gameWonClockStarted = true;
	}
	winDuration = (std::clock() - winClock) / (float)CLOCKS_PER_SEC; // measure the duration of the win clock
	if (winDuration > 5.0f)
	{
		gameEnded = true; // end the game once the game has been won for 5 seconds
	}
}

void Level::deathFunction(float frameTime)
{
	// only execute contents if the game hasn't already been won
	if (!gameWon)
	{
		garryMaterial.set_texture(garryModelRIP);
		if (!gameOverClockStarted) // only execute contents once
		{
			deathClock = std::clock(); // initiate death clock
			gameOverClockStarted = true;

			if (audioManager)
			{
				audioManager->PlaySample(hitSurface);
				audioManager->StopMusic();
			}
		}

		deathDuration = (std::clock() - deathClock) / (float)CLOCKS_PER_SEC; // measure duration of death clock

		// if the clock has been active for 1.5 seconds
		if (deathDuration > 1.5 && deathDuration < 1.6)
		{
			if (audioManager)
			{
				audioManager->PlaySample(tickTock);
				audioManager->PlaySample(wahwah);
			}
		}

		// if the clock has been active for 3 seconds
		if (deathDuration > 3 && deathDuration < 3.05)
		{
			if (audioManager)
			{
				audioManager->StopPlayingSampleVoice(tickTock);
				audioManager->StopPlayingSampleVoice(wahwah);
				audioManager->PlaySample(death);
			}

			gameOver2 = true; // when set to true, this initiates events in other functions
			endSlateMaterial.set_colour(0xFF000000);
			endSlate.set_mesh(primitiveBuilder->CreateBoxMesh(gef::Vector4(500.75f, 500.0f, 0, 0)));

		}
		if (deathDuration > 6.5f)
		{
			gameEnded = true; // end the game once 6.5 seconds has passed
		}
	}
}

void Level::checkCollisions()
{
	if (gameInitiated && !gameWon)
	{
		// collision detection
		b2Contact* contact = world->GetContactList(); // get the head of the contact list
		int contact_count = world->GetContactCount(); // get contact count

		for (int contact_num = 0; contact_num < contact_count; ++contact_num)
		{
			if (contact->IsTouching() && (contact->GetFixtureA()->GetBody() == player.playerBody) || (contact->GetFixtureB()->GetBody() == player.playerBody))
			{
				// get the colliding bodies
				b2Body* playerBody = contact->GetFixtureA()->GetBody();
				b2Body* collidingSurface = contact->GetFixtureB()->GetBody();

				// Collision response
				if (collidingSurface == defusalZone.defusalZoneBody || playerBody == defusalZone.defusalZoneBody) // if the player has collided with the defusal zone
				{
					gameWon = true;
				}
				else
				{
					gameOver = true; // any other collisions result in death
				}
			}
			contact = contact->GetNext(); // Get next contact point
		}
	}
}

void Level::InitialiseModels()
{
	// load the assets in from the .scn
	const char* garryFilename = "garry.scn";
	const char* grappleBallFilename = "grappleBall.scn";
	const char* grappleBallGlowingFilename = "grappleBallGlowing.scn";
	const char* crateFilename = "crate.scn";
	const char* dangerSignFilename = "danger.scn";
	const char* elevatorSignFilename = "elevatorSign.scn";
	const char* defuserFilename = "defuser.scn";

	// load all the textures for the player
	garryModelTex = CreateTextureFromPNG("garry2.png", platform);
	garryModelDown = CreateTextureFromPNG("lookdown.png", platform);
	garryModelDownLeft = CreateTextureFromPNG("lookdownleft.png", platform);
	garryModelDownRight = CreateTextureFromPNG("lookdownright.png", platform);
	garryModelUp = CreateTextureFromPNG("lookup.png", platform);
	garryModelUpLeft = CreateTextureFromPNG("lookupleft.png", platform);
	garryModelUpRight = CreateTextureFromPNG("lookupright.png", platform);
	garryModelRight = CreateTextureFromPNG("lookright.png", platform);
	garryModelLeft = CreateTextureFromPNG("lookleft.png", platform);
	garryModelScared = CreateTextureFromPNG("s.png", platform);
	garryModelDownScared = CreateTextureFromPNG("sD.png", platform);
	garryModelDownLeftScared = CreateTextureFromPNG("sDL.png", platform);
	garryModelDownRightScared = CreateTextureFromPNG("sDR.png", platform);
	garryModelUpScared = CreateTextureFromPNG("sU.png", platform);
	garryModelUpLeftScared = CreateTextureFromPNG("sUL.png", platform);
	garryModelUpRightScared = CreateTextureFromPNG("sUR.png", platform);
	garryModelRightScared = CreateTextureFromPNG("sR.png", platform);
	garryModelLeftScared = CreateTextureFromPNG("sL.png", platform);
	garryModelRIP = CreateTextureFromPNG("rip.png", platform);
	garryModelHappy = CreateTextureFromPNG("garryHappy.png", platform);

	garryModel = LoadSceneAssets(platform, garryFilename);
	grappleBall = LoadSceneAssets(platform, grappleBallFilename);
	grappleBallGlowing = LoadSceneAssets(platform, grappleBallGlowingFilename);
	crate = LoadSceneAssets(platform, crateFilename);
	dangerSign = LoadSceneAssets(platform, dangerSignFilename);
	elevatorSign = LoadSceneAssets(platform, elevatorSignFilename);
	defuser = LoadSceneAssets(platform, defuserFilename);

	if (garryModel)
	{
		player.set_mesh(GetMeshFromSceneAssets(garryModel));
	}

	else
	{
		gef::DebugOut("Scene file %s failed to load\n", garryFilename);
	}

}

void Level::InitialiseSurfaces()
{
	// set the textures for the surfaces
	surfaceMat.set_texture(surfaceTexture);
	defusalZoneMaterial.set_texture(defuserTexture);

	// initialise all of the surfaces that appear in the level
	// surfaces have been split up into areas

	dangerSignSurface.InitialiseBackground(world, primitiveBuilder);
	backgroundMatrix.Scale(gef::Vector4(2, 2, 1, 0));
	backgroundMatrix.SetTranslation(gef::Vector4(26, -10, -5, 0));
	dangerSignSurface.set_transform(backgroundMatrix);

	dangerSignSurface2.InitialiseBackground(world, primitiveBuilder);
	backgroundMatrix.Scale(gef::Vector4(2, 2, 1, 0));
	backgroundMatrix.SetTranslation(gef::Vector4(3.25, -45, -5, 0));
	dangerSignSurface2.set_transform(backgroundMatrix);

	elevatorSignSurface.InitialiseBackground(world, primitiveBuilder);
	backgroundMatrix.Scale(gef::Vector4(2, 2, 1, 0));
	backgroundMatrix.SetTranslation(gef::Vector4(115.5f, -35, -5, 0));
	elevatorSignSurface.set_transform(backgroundMatrix);

	area1[0].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(22.0f, 10.0f, 6.0f), b2Vec2(10.0f, 12.0f));
	area1[1].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(16.0f, 7.5f, 6.0f), b2Vec2(4.0f, -15.0f));
	area1[2].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(10.0f, 50.0f, 6.0f), b2Vec2(-22.0f, -28.0f)); 
	area1[3].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(10.0f, 32.0f, 6.0f), b2Vec2(42.0f, -10.0f)); 

	area2[0].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(6.0f, 20.25f, 6.0f), b2Vec2(-6.0f, -57.75f)); 
	area2[1].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(13.0f, 10.0f, 6.0f), b2Vec2(19.0f, -47.5f)); 
	area2[2].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(63.0f, 5.0f, 6.0f), b2Vec2(62.5f, -73.0f));

	area3[0].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(45.0f, -55.0f));
	area3[1].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(60.0f, -50.0f));
	area3[2].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(75.0f, -45.0f));
	area3[3].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(90.0f, -40.0f));
	area3[4].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(105.0f, -35.0f));
	area3[5].initialiseSphereSurface(world, primitiveBuilder, platform, 2.0f, b2Vec2(115.5f, -0.0f));

	area4[0].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(45.0f, 37.0f, 6.0f), b2Vec2(170.5f, -40.0f));
	area4[1].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(65.0f, 5.0f, 6.0f), b2Vec2(170.0f, 18.0f));
	area4[2].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(5.0f, 32.0f, 6.0f), b2Vec2(210.5f, -44.5f));
	area4[3].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(5.0f, 50.0f, 6.0f), b2Vec2(240.5f, -26.5f));
	area4[4].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(10.0f, 5.0f, 6.0f), b2Vec2(225.5f, -71.5f));

	crates[0].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-10.0f, -5.5f));
	crates[1].InitialiseCrate(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-10.0f, -5.5f));
	crates[2].InitialiseCrate(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-10.0f, -1.5f));
	crates[3].InitialiseCrate(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-6.0f, -5.5f));
	crates[4].InitialiseCrate(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(12.0f, -5.5f));
	crates[5].initialiseSurface(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-10.0f, -35.5f));
	crates[6].InitialiseCrate(world, primitiveBuilder, platform, gef::Vector4(2.0f, 2.0f, 2.0f), b2Vec2(-10.0f, -35.5f));

	defusalZone.InitialiseDefusalZone(world, primitiveBuilder, b2Vec2(227.0f, -54.0f));
	defusalZone.set_mesh(GetMeshFromSceneAssets(defuser));

	// create a matrix that allows certain objects to be placed in the background through manipulation of the z axis
	gef::Matrix44 backLayer;
	backLayer.Scale(gef::Vector4(1, 1, 1, 0));

	backLayer.SetTranslation(crates[1].transform().GetTranslation() + gef::Vector4(0, 0, -4, 0));
	crates[1].set_transform(backLayer);

	backLayer.SetTranslation(crates[2].transform().GetTranslation() + gef::Vector4(0, 0, -4, 0));
	crates[2].set_transform(backLayer);

	backLayer.SetTranslation(crates[3].transform().GetTranslation() + gef::Vector4(0, 0, -4, 0));
	crates[3].set_transform(backLayer);

	backLayer.SetTranslation(crates[4].transform().GetTranslation() + gef::Vector4(0, 0, -4, 0));
	crates[4].set_transform(backLayer);

	backLayer.SetTranslation(crates[6].transform().GetTranslation() + gef::Vector4(0, 0, -4, 0));
	crates[6].set_transform(backLayer);

	gef::Matrix44 frontLayer;

	// set the meshes of the crates to the imported model
	for (int i = 0; i < crates.size(); i++)
	{
		crates[i].set_mesh(GetMeshFromSceneAssets(crate));
	}
	
	// set the meshes of the spheres to the imported model
	for (int i = 0; i < area3.size(); i++)
	{
		area3[i].set_mesh(GetMeshFromSceneAssets(grappleBall));
	}
}

void Level::InitialiseRaycastVisualisations()
{
	// Representation of raycast
	rayMaterial.set_colour(0x66FFFFFF);
	rayBody = world->CreateBody(&rayBodyDef);
	ray.set_mesh(primitiveBuilder->CreateBoxMesh(gef::Vector4(7.5f, 0.3f, 0.01f), gef::Vector4(player.playerBody->GetPosition().x - 8, player.playerBody->GetPosition().y - 0.5f, 0)));
	rayHitBody = world->CreateBody(&rayHitBodyDef);
	rayHit.set_mesh(primitiveBuilder->CreateSphereMesh(0.4f, 20, 20, gef::Vector4(0, 0, 0)));
}

void Level::InitialiseFont()
{
	font = new gef::Font(platform);
	font->Load("comic_sans");
}

void Level::CleanUpFont()
{
	delete font;
	font = NULL;
}

void Level::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer3d->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.2f, 0.2f, 0.2f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.6f, 0.6f, 0.6f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

gef::Texture* Level::ChangeFace()
{
	float angle = raycast.currentRayAngle;

	if ((player.playerBody->GetLinearVelocity().Length() > scaredSpeed) && !hasGasped) // if the player reaches a certain speed, play the sound once
	{
		if (audioManager)
			audioManager->PlaySample(gasp);
		hasGasped = true;
	}

	if (player.playerBody->GetLinearVelocity().Length() < scaredSpeed && hasGasped)
		hasGasped = false; // sound can only be played again once the scaredSpeed is reached

	// only make the texture appear like it's looking in a direction when the raycast is active - i.e. when the right joystick is being pushed in an angle
	if (raycast.raycastActive)
	{
		if (angle < -1.75)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return garryModelUpRightScared;
			else
				return(garryModelUpRight);
		}
		else if (angle > -1.75 && angle < -1)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelUpScared);
			else
				return(garryModelUp);
		}
		else if (angle > -1 && angle < -0.25)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelUpLeftScared);
			else
				return(garryModelUpLeft);
		}
		else if (angle > -0.25 && angle < 0.5)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelLeftScared);
			else
				return(garryModelLeft);
		}
		else if (angle > 0.5 && angle < 1.25)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelDownLeftScared);
			else
				return(garryModelDownLeft);
		}
		else if (angle > 1.25 && angle < 1.75)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelDownScared);
			else
				return(garryModelDown);
		}
		else if (angle > 1.75 && angle < 2.5)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelDownRightScared);
			else
				return(garryModelDownRight);
		}
		else if (angle > 2.5 && angle < 3.25)
		{
			if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed)
				return(garryModelRightScared);
			else
				return(garryModelRight);
		}
	}
	else
	{
		if (player.playerBody->GetLinearVelocity().Length() > scaredSpeed) // display the default textures if the raycast is not active
			return(garryModelScared);
		else
			return(garryModelTex);
	}

}

gef::Scene* Level::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* Level::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}
