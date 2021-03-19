#include "Menu.h"
#include <input\sony_controller_input_manager.h>
#include <system/platform.h>
#include "loadTexture.h"
#include "Level.h"
#include "Controls.h"


void Menu::onEnter()
{
	loading = false;
	spriteRenderer = gef::SpriteRenderer::Create(platform);
	input = gef::InputManager::Create(platform);
	audioManager = gef::AudioManager::Create(); // initialise audio manager
	menuSelection = 1;

	// initialise background sprite
	backgroundTexture = CreateTextureFromPNG("menu.png", platform);
	backgroundLoadingTexture = CreateTextureFromPNG("menuLoading.png", platform);
	background.set_texture(backgroundTexture);
	background.set_width(platform.width());
	background.set_height(platform.height());
	background.set_position(platform.width() / 2, platform.height() / 2, 0);

	//initialise pointer sprite
	pointerTexture = CreateTextureFromPNG("pointer.png", platform);
	pointer.set_texture(pointerTexture);
	pointer.set_width(90);
	pointer.set_height(51);

	if (audioManager)
	{
		menuClick = audioManager->LoadSample("sound/menuClick.wav", platform);
		menuSelect = audioManager->LoadSample("sound/menuSelect.wav", platform);
		menuAmbience = audioManager->LoadMusic("sound/menuAmbience.wav", platform);
		audioManager->PlayMusic();
	}

	initialiseFont();

	if (levelTime > fastestTime)
		fastestTime = levelTime;

}

void Menu::onExit()
{
	cleanUp();
}

void Menu::initialiseFont()
{
	font = new gef::Font(platform);
	font->Load("comic_sans");
}

baseState * Menu::onUpdate(float frameTime)
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
				if (controller->buttons_pressed() & gef_SONY_CTRL_DOWN)
				{
					if (audioManager)
						audioManager->PlaySample(menuClick);
					menuSelection++;
				}

				if (controller->buttons_pressed() & gef_SONY_CTRL_UP)
				{
					if (audioManager)
						audioManager->PlaySample(menuClick);
					menuSelection--;
				}

				if (menuSelection > 3)
					menuSelection = 1;
				if (menuSelection < 1)
					menuSelection = 3;

				if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
				{
					if(audioManager)
						audioManager->PlaySample(menuSelect);

					switch (menuSelection)
					{
					case(1):
					{
						loading = true;
						background.set_texture(backgroundLoadingTexture);
						return new Level(platform);  
					}
					case(2):
					{
						loading = true;
						background.set_texture(backgroundLoadingTexture);
						return new Controls(platform);
					}
					case(3):
					{
						exit(0);
					}
					}
				}
			}
		}
	}


	if (menuSelection == 1)
		pointer.set_position(200, 200, -0.1f);
	if (menuSelection == 2)
		pointer.set_position(285, 260, -0.1f);
	if (menuSelection == 3)
		pointer.set_position(170, 320, -0.1f);

	return this;
}

void Menu::onRender()
{
	if (spriteRenderer)
	{
		// start drawing sprites, but don't clear the frame buffer
		spriteRenderer->Begin(false);
		spriteRenderer->DrawSprite(background);
		if (loading == false)
		{
			spriteRenderer->DrawSprite(pointer);

			if (font)
			{
				if (fastestTime == 0)
					font->RenderText(spriteRenderer, gef::Vector4(330.0f, 120.0f, -0.9f), 1.5f, 0xff000000, gef::TJ_LEFT, "-");
				else if (fastestTime > 0)
					font->RenderText(spriteRenderer, gef::Vector4(330.0f, 120.0f, -0.9f), 1.5f, 0xff000000, gef::TJ_LEFT, "%.2f", fastestTime);
			}
		}

		spriteRenderer->End();
	}

	
}

Menu::Menu(gef::Platform& platform, float time) :
	platform(platform),
	spriteRenderer(NULL),
	audioManager(NULL),
	input(NULL),
	font(NULL),
	levelTime(time)
{
}

Menu::~Menu()
{
}


void Menu::cleanUp()
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