#pragma once
#include "input\input_manager.h"
#include "graphics\sprite_renderer.h"
#include <audio/audio_manager.h>
#include "graphics\font.h"
#include "graphics\sprite.h"
#include "states.h"

namespace gef
{
	class Platform;
	class priteRenderer;
	class AudioManager;
	class InputManager;
	class Font;
}

class Menu : public baseState
{
public:
	void onEnter() override;
	void onExit() override;
	baseState* onUpdate(float frameTime) override;
	void onRender() override;

	Menu(gef::Platform& platform, float time);
	~Menu();

private:
	void cleanUp();
	void initialiseFont();

	gef::Platform& platform;
	gef::InputManager* input;
	gef::SpriteRenderer* spriteRenderer;
	gef::AudioManager* audioManager;

	gef::Sprite background;
	gef::Texture* backgroundTexture;
	gef::Texture* backgroundLoadingTexture;
	gef::Sprite pointer;
	gef::Texture* pointerTexture;
	gef::Font* font;

	int menuSelection;
	bool loading;
	float fastestTime = 0;
	float levelTime;

	int menuClick = 0;
	int menuSelect = 0;
	int menuAmbience = 0;
};

