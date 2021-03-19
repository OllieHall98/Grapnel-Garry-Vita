#include "Controls.h"
#include <input\sony_controller_input_manager.h>
#include <system/platform.h>
#include "loadTexture.h"
#include "Menu.h"


void Controls::onEnter()
{
	spriteRenderer = gef::SpriteRenderer::Create(platform);
	input = gef::InputManager::Create(platform);
	audioManager = gef::AudioManager::Create(); // initialise audio manager

	// initialise background sprite
	backgroundTexture = CreateTextureFromPNG("controlsScreen.png", platform);
	loading = CreateTextureFromPNG("menuLoading.png", platform);
	background.set_texture(backgroundTexture);
	background.set_width(platform.width());
	background.set_height(platform.height());
	background.set_position(platform.width() / 2, platform.height() / 2, 0);

	if (audioManager)
	{
		menuSelect = audioManager->LoadSample("sound/menuSelect.wav", platform);
	}
}

void Controls::onExit()
{
	cleanUp();
}

baseState* Controls::onUpdate(float frameTime)
{
	if (input)
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
				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
				{
					background.set_texture(loading);
					if (audioManager)
						audioManager->PlaySample(menuSelect);
					return new Menu(platform, 0);
				}
			}
		}
	}
	return this;
}

void Controls::onRender()
{
	render();
}

Controls::Controls(gef::Platform& platform) :
	platform(platform),
	spriteRenderer(NULL),
	audioManager(NULL),
	input(NULL)
{
}

Controls::~Controls()
{
}


void Controls::cleanUp()
{
	delete spriteRenderer;
	spriteRenderer = NULL;

	delete input;
	input = NULL;
}


void Controls::render()
{
	if (spriteRenderer)
	{
		// start drawing sprites, but don't clear the frame buffer
		spriteRenderer->Begin(false);
		spriteRenderer->DrawSprite(background);
		spriteRenderer->End();
	}
}