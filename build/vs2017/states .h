#pragma once
#include <system/platform.h>

namespace gef
{
	class Font;
	class InputManager;
}

class baseState
{
public:
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual baseState* onUpdate(float frameTime) = 0;
	virtual void onRender() = 0;
};
