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
}

class Controls : public baseState
{
public:
	void onEnter() override;
	void onExit() override;
	baseState* onUpdate(float frameTime) override;
	void onRender() override;

	Controls(gef::Platform& platform);
	~Controls();
	

private:
	void cleanUp();
	void render();
	gef::Platform& platform;
	gef::InputManager* input;
	gef::SpriteRenderer* spriteRenderer;
	gef::AudioManager* audioManager;

	gef::Sprite background;
	gef::Texture* backgroundTexture;
	gef::Texture* loading;

	int menuSelect = 0;
};