#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input\sony_controller_input_manager.h>
#include <input\keyboard.h>
#include <states.h>
#include "Menu.h"
#include "Level.h"
#include "Intro.h"

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	primitiveBuilder(NULL),
	renderer3d(NULL),
	spriteRenderer(NULL),
	inputManager(NULL),
	audioManager(NULL)
{
}

void SceneApp::Init()
{
	spriteRenderer = gef::SpriteRenderer::Create(platform_);
	renderer3d = gef::Renderer3D::Create(platform_); // create the renderer for draw 3D geometry
	primitiveBuilder = new PrimitiveBuilder(platform_); // initialise primitive builder to make create some 3D geometry easier
	inputManager = gef::InputManager::Create(platform_);
	audioManager = gef::AudioManager::Create(); // initialise audio manager
	initialiseFont();

	currentState = new Intro(platform_);
	nextState = currentState;
	currentState->onEnter();
}

void SceneApp::CleanUp()
{
	delete primitiveBuilder;
	primitiveBuilder = NULL;

	delete renderer3d;
	renderer3d = NULL;

	delete spriteRenderer;
	spriteRenderer = NULL;
}

bool SceneApp::Update(float frameTime)
{
	if (nextState != currentState)
	{
		currentState->onExit();
		delete currentState;
		currentState = nextState;
		currentState->onEnter();
	}
	
	nextState = currentState->onUpdate(frameTime);

	return true;
}

void SceneApp::Render()
{
 	currentState->onRender();
}

void SceneApp::initialiseFont()
{
	font = new gef::Font(platform_);
	font->Load("comic_sans");
}

