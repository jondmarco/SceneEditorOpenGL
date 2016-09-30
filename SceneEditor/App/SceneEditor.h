#pragma once

#include "Base\OpenGLApp.h"

#include "Game\Scene.h"

#include "Services\ModelLoader.h"

#include "System\Input\InputManager.h"
#include "System\Render\RenderSystem.h"
#include "System\Physics\PhysicsSystem.h"

#include "Tools\EToolType.h"
#include "Tools\ITransformTool.h"
#include "Tools\TranslationTool.h"
#include "Tools\RotationTool.h"
#include "Tools\ScaleTool.h"

#include "Services\GameStorage.h"

#include <string>

using std::string;

enum Mode { EDIT, PLAY };


// Note: This class has grown a lot and has too many responsibilities
// TODO: Extract responsibilities into separate classes, use delegation. 
// Example: Create GameLoop class with all functionality related to game loop
class SceneEditor
{
public:

	SceneEditor();

	void runApp(bool loadScene, Mode mode);

	// Application-level information and manipulation
	void BeginShutdown();
	void ShowCursor();
	void HideCursor();

	void SetScreenSize(int width, int height);

	// Start game loop
	void GameLoop();

	// Scene manipulation
	Scene& GetLoadedScene();

	void LoadEntity();
	void AddLight(vec3 lightp, vec3 lightc);

	/*Refactor* the below functions to a game manager class? 
	From which we could also keep track of the game mode*/
	SceneEntity* GetSelectedEntity();
	void TryPickEntity(float cursorX, float cursorY);
	void SelectNextEntity();
	void SelectPreviousEntity();
	SceneEntity* GetEntityAtIndex(int index);
	void SelectEntityAtIndex(int index);
	int GetSelectedEntityIndex();

	// Tools
	ITransformTool* GetActiveTool();
	void SetActiveTool(EToolType tool);

	void SaveScene();

	Mode GetMode();
	void SetMode(Mode mode);

	void DeleteEntity();

	void Terminate();

private:

	void Initialize();
	
	// Game loop helpers
	float GetDeltaTime();
	void ProcessEvents();
	void Update(float deltaTime);
	void Render(float deltaTime);
	void ReportErrors();

	// Render helpers
	void RenderUI(float deltaTime);
	void RenderFPSCounter(float deltaTime);
	void RenderActiveToolInfo();

	// Components and services
	OpenGLApp _openGLApp;

	RenderSystem _renderSystem;
	PhysicsSystem _physicsSystem;
	InputManager _inputManager;
	ModelLoader _modelLoader;
	GameStorage _gameStorage;

	Mode _mode;

	// Currently loaded scene
	Scene _scene;
	unsigned int _selectedEntityIndex = 0;

	// Manipulation Tools
	TranslationTool _translationTool;
	RotationTool _rotationTool;
	ScaleTool _scaleTool;

	ITransformTool* _activeTool = &_translationTool;

	// Application window parameters
	const int _defaultWindowWidth = 1280;
	const int _defaultWindowHeight = 1280;
	const string _windowTitle = "SceneEditor";
	const bool _isWindowResizable = true;

	int _windowHeight = _defaultWindowHeight;
	int _windowWidth = _defaultWindowWidth;
};
