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

class Intro : public baseState
{
public:
	void onEnter() override;
	void onExit() override;
	baseState* onUpdate(float frameTime) override;
	void onRender() override;

	Intro(gef::Platform& platform);
	~Intro();

private:
	void cleanUp();
	void initialiseFont();

	gef::Platform& platform;
	gef::InputManager* input;
	gef::SpriteRenderer* spriteRenderer;
	gef::AudioManager* audioManager;

	gef::Sprite screen;
	gef::Texture* page1;
	gef::Texture* page2;
	gef::Texture* page3;
	gef::Texture* loading;
	gef::Font* font;
	

	int click = 0;
	int music = 0;
	int page = 0;

};
