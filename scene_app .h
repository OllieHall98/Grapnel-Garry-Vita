#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class AudioManager;
	class Renderer3D;
}

class baseState;

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	bool Update(float frameTime);
	void CleanUp();
	void Render();

	
private:
	void initialiseFont();
	gef::InputManager* inputManager;
	gef::SpriteRenderer* spriteRenderer;
	gef::Font* font;
	gef::Renderer3D* renderer3d;
	gef::AudioManager* audioManager;
	PrimitiveBuilder* primitiveBuilder;
	baseState* currentState;
	baseState* nextState;

	float fps;
	//gef::Keyboard keyboard;
	
};

#endif // _SCENE_APP_H
