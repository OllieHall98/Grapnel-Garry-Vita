#pragma once
#include "graphics/scene.h"
#include "Player.h"
#include "Rope.h"
#include "Surface.h"
#include "Raycast.h"
#include "Camera.h"
#include <cstdio>
#include <ctime>
#include <audio/audio_manager.h>
#include "states.h"


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class AudioManager;
	class InputManager;
	class Renderer3D;
}

class Level : public baseState
{
public:
	Level(gef::Platform& platform);
	~Level();

	void onEnter() override;
	void onExit() override;
	baseState* onUpdate(float frameTime) override;
	void onRender() override;
	
private:
	void CleanUp();
	void ControlInput(float frameTime); // takes input and acts on them
	void winFunction(float frameTime); // game win state
	void deathFunction(float frameTime); // game over state
	void checkCollisions(); // checks if the player is colliding with a surface
	void InitialiseModels();
	void InitialiseSurfaces();
	void InitialiseRaycastVisualisations();
	void InitialiseFont();
	void CleanUpFont();
	void SetupLights();
	gef::Texture* ChangeFace(); // changes the texture of the player model depending on speed and aim direction
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);

	b2World* world; // create the physics world
	PrimitiveBuilder* primitiveBuilder;
	gef::SpriteRenderer* spriteRenderer;
	gef::Renderer3D* renderer3d;
	gef::Platform& platform;
	gef::InputManager* input;
	gef::AudioManager* audioManager;
	gef::Font* font;

	// clocks
	std::clock_t start;
	std::clock_t deathClock;
	std::clock_t winClock;
	float duration = 0; // duration of the entire game session
	float deathDuration = 0; // a clock to determine when to cue certain events when the player loses
	float winDuration = 0; //  a clock to determine when to cue certain events when the player wins

	// the raycast representations
	GameObject ray;
	b2Body* rayBody;
	b2BodyDef rayBodyDef;
	gef::Material rayMaterial;

	// UI Sprites and Textures
	gef::Texture* controlsTexture;
	gef::Texture* controlsTextureCross;
	gef::Texture* controlsTextureRB;
	gef::Texture* controlsTextureLB;
	gef::Texture* controlsTextureRBCross;
	gef::Texture* controlsTextureLBCross;
	gef::Texture* controlsTextureLBRBCross;
	gef::Texture* controlsTextureLBRB;
	gef::Texture* surfaceTexture;
	gef::Texture* garryDeadText;
	gef::Texture* garryDefusedText;
	gef::Sprite garryDeadSprite;
	gef::Sprite garryDefusedSprite;
	gef::Sprite controlsSprite;
	gef::Sprite pauseText;
	gef::Texture* pauseTexture;
	gef::Texture* backgroundTexture;
	gef::Sprite backgroundSprite;
	gef::Sprite loading;
	gef::Texture* loadingTexture;
	gef::Texture* defuserTexture;

	// sound
	int tickTock = 0;
	int death = 0;
	int music = 0;
	int grapple = 0;
	int wahwah = 0;
	int win = 0;
	int hitSurface = 0;
	int unGrapple = 0;
	int gasp = 0;

	// floats
	float fps_;
	float rAngle; // angle of the right joystick
	float lAngle; // angle of the left joystick
	float currentRayAngle = 0; // the angle of the joystick raycast
	float distance = 3; // the default length of the rope
	float timeScale; // the variable that is multiplied by the time to create slow motion
	const float scaredSpeed = 20; // the speed that the player has to reach to change the texture to looking scared
	float aspectRatio;
	float winTime; // the time that is passed to the menu if the player wins

	// bools
	bool gamePaused; // if this bool is true, the game stops updating
	bool ropeExists; // if the rope exists this bool is true
	bool gameOver; // is set to true when the player hits a wall
	bool gameOver2; // is set to true after a few seconds
	bool gameEnded; // is set to true when the game is ready to quit to the menu
	bool gameWon; // is set to true when the player reaches the end
	bool gameOverClockStarted; // is set to true when the game over clock is started
	bool gameWonClockStarted; // is set to true when the game won clock is started
	bool gameInitiated; // is set to true when the player launches the first rope
	bool slowTime; // if the left bumper is held down, slow down is set to true
	bool hasGasped; // is set to true when the 'gasp' sound is played, this makes sure it only happens once
		
	// models
	gef::Scene* backgroundModel;
	gef::Matrix44 backgroundMatrix;
	gef::Scene* garryModel;
	gef::Scene* grappleBall;
	gef::Scene* grappleBallGlowing;
	gef::Texture* garryModelTex;
	gef::Texture* garryModelDown;
	gef::Texture* garryModelDownLeft;
	gef::Texture* garryModelDownRight;
	gef::Texture* garryModelUp;
	gef::Texture* garryModelUpRight;
	gef::Texture* garryModelUpLeft;
	gef::Texture* garryModelLeft;
	gef::Texture* garryModelRight;
	gef::Texture* garryModelScared;
	gef::Texture* garryModelDownScared;
	gef::Texture* garryModelDownLeftScared;
	gef::Texture* garryModelDownRightScared;
	gef::Texture* garryModelUpScared;
	gef::Texture* garryModelUpRightScared;
	gef::Texture* garryModelUpLeftScared;
	gef::Texture* garryModelLeftScared;
	gef::Texture* garryModelRightScared;
	gef::Texture* garryModelHappy;
	gef::Texture* garryModelRIP;

	gef::Scene* crate;
	gef::Scene* dangerSign;
	gef::Scene* elevatorSign;
	gef::Scene* defuser;

	// grapple hook
	b2Body* grappleTarget;
	GameObject grappleHook;
	b2Body* grappleHookBody;
	b2BodyDef grappleHookBodyDef;
	b2Vec2 grapplePos;
	float grappleRot;

	// raycast
	GameObject rayHit;
	b2Body* rayHitBody;
	b2BodyDef rayHitBodyDef;

	// materials
	gef::Material surfaceMat;
	gef::Material endSlateMaterial;
	gef::Material defusalZoneMaterial;
	gef::Material garryMaterial;

	// surfaces
	Surface defusalZone;
	std::array<Surface, 4> area1;
	std::array<Surface, 3> area2;
	std::array<Surface, 6> area3;
	std::array<Surface, 5> area4;
	std::array<Surface, 7> crates;
	Surface background;
	Surface dangerSignSurface;
	Surface dangerSignSurface2;
	Surface elevatorSignSurface;
	Surface endSlate;

	// objects and class instances
	std::vector<GameObject> objectArray;
	Rope rope;
	Player player;
	Raycast raycast;
	Camera camera;
};

