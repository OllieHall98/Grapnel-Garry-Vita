#include "Intro.h"
#include <input\sony_controller_input_manager.h>
#include <system/platform.h>
#include "loadTexture.h"
#include "Level.h"
#include "Controls.h"
#include "Menu.h"


void Intro::onEnter()
{
	spriteRenderer = gef::SpriteRenderer::Create(platform);
	input = gef::InputManager::Create(platform);
	audioManager = gef::AudioManager::Create(); // initialise audio manager

	// initialise background sprite
	page1 = CreateTextureFromPNG("page1.png", platform);
	page2 = CreateTextureFromPNG("page2.png", platform);
	page3 = CreateTextureFromPNG("page3.png", platform);
	loading = CreateTextureFromPNG("menuLoading.png", platform);
	screen.set_texture(page1);
	screen.set_width(platform.width());
	screen.set_height(platform.height());
	screen.set_position(platform.width() / 2, platform.height() / 2, 0);

	page = 0;

	if (audioManager)
	{
		click = audioManager->LoadSample("sound/menuSelect.wav", platform);
		music = audioManager->LoadMusic("sound/introMusic.wav", platform);
		audioManager->PlayMusic();
	}

	initialiseFont();
}

void Intro::onExit()
{
	cleanUp();
}

void Intro::initialiseFont()
{
	font = new gef::Font(platform);
	font->Load("comic_sans");
}

baseState * Intro::onUpdate(float frameTime)
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
				if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
				{
					if (audioManager)
						audioManager->PlaySample(click);
					page++;					
				}

				if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
				{
					if (page > 0)
					{
						if (audioManager)
							audioManager->PlaySample(click);
						page--;
					}
				}

				}
			}
		}

	if (page == 0)
		screen.set_texture(page1);

	if (page == 1)
		screen.set_texture(page2);

	if (page == 2)
		screen.set_texture(page3);

	if (page == 3)
	{
		screen.set_texture(loading);
		return new Menu(platform, 0);
	}

	return this;
}

void Intro::onRender()
{
	if (spriteRenderer)
	{
		// start drawing sprites, but don't clear the frame buffer
		spriteRenderer->Begin(false);
			spriteRenderer->DrawSprite(screen);
		spriteRenderer->End();
	}


}

Intro::Intro(gef::Platform& platform) :
	platform(platform),
	spriteRenderer(NULL),
	audioManager(NULL),
	input(NULL),
	font(NULL)
{
}

Intro::~Intro()
{
}


void Intro::cleanUp()
{
	delete spriteRenderer;
	spriteRenderer = NULL;

	delete audioManager;
	audioManager = NULL;

	delete input;
	input = NULL;

	delete font;
	font = NULL;
}