#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"


#include <string>

class SceneEditor;
class GameStorage;

using std::string;


class InputManager
{
public:

	InputManager() {};
	InputManager(SceneEditor* sceneEditor) : _sceneEditor(sceneEditor), _attachToolState(0) {}

	void UpdateKeyState(int key, int action);
	void UpdateCursorPosition(double xpos, double ypos);
	void UpdateScrollOffset(double xOffset, double yOffset);
	void UpdateMouseButtonState(int button, int action);
	void OnFramebufferSizeChanged(int width, int height);

	string SelectFile();
	string SaveFile();

	void ProcessInput(float deltaTime);
	void ResetInputStates();

private:

	void UpdateActiveModel(int key, int action);

	void ProcessSystemInput();
	void ProcessCameraInput(float deltaTime);
	void ProcessToolInput(float deltaTime);

	void ProcessAttachToolInput(int key, int action);
	bool IsParent(int parent, int child);

	SceneEditor* _sceneEditor;

	float _lastMouseX;
	float _lastMouseY;

	float _cursorOffsetX;
	float _cursorOffsetY;

	float _scrollOffsetY;

	bool _keys[1024];
	bool _rightMouseButtonPressed;

	const float _cursorSensitivity = 0.1f;

	const string LOG_CHANNEL = "InputManager";

	GameStorage* _gameStorage;

	int _attachToolState;
	int _parentIndex;

	bool _canDelete = false;
};
