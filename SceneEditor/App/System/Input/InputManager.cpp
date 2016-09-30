#include "Utils\Win32\OpenFileDialog.h"
#include "Utils\Win32\SaveFileDialog.h"

#include "InputManager.h"

#include "SceneEditor.h"
#include "../../Services/GameStorage.h"

#include "Game\Scene.h"
#include "Game\Camera.h"
#include "Services\Logger.h"

#include <sstream>

using std::stringstream;
void InputManager::ResetInputStates()
{
	_rightMouseButtonPressed = false;
	for (int i = 0; i < 1024; ++i)
	{
		_keys[i] = false;
	}
}

void InputManager::UpdateKeyState(int key, int action)
{
	if (action == GLFW_PRESS)
	{
		_keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		_keys[key] = false;

		if (key == GLFW_KEY_DELETE)
		{
			_canDelete = true;
		}
	}

	ProcessAttachToolInput(key, action);
	UpdateActiveModel(key, action);
}

void InputManager::ProcessInput(float deltaTime)
{
	ProcessSystemInput();
	ProcessCameraInput(deltaTime);
	ProcessToolInput(deltaTime);
}

void InputManager::UpdateActiveModel(int key, int action)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_PAGE_UP) _sceneEditor->SelectNextEntity();
		if (key == GLFW_KEY_PAGE_DOWN) _sceneEditor->SelectPreviousEntity();
	}
}

// Process application-level events
void InputManager::ProcessSystemInput()
{
	if (_keys[GLFW_KEY_ESCAPE])
	{
		_sceneEditor->BeginShutdown();
	}
	else if (_keys[GLFW_KEY_L])
	{
		_sceneEditor->LoadEntity();
	}
	else if (_keys[GLFW_KEY_M])
	{
		_gameStorage->save(&_sceneEditor->GetLoadedScene());
	}
	else if (_keys[GLFW_KEY_N])
	{
		_gameStorage->load();
	}
	else if (_keys[GLFW_KEY_DELETE] && _canDelete)
	{
		_sceneEditor->DeleteEntity();
		_canDelete = false;
	}
}

void InputManager::ProcessCameraInput(float deltaTime)
{
	if (_rightMouseButtonPressed)
	{
		Camera& camera = _sceneEditor->GetLoadedScene().GetCamera();

		// Position
		if (_keys[GLFW_KEY_W]) camera.Move(MoveDirection::FORWARD, deltaTime);
		if (_keys[GLFW_KEY_S]) camera.Move(MoveDirection::BACK, deltaTime);
		if (_keys[GLFW_KEY_A]) camera.Move(MoveDirection::LEFT, deltaTime);
		if (_keys[GLFW_KEY_D]) camera.Move(MoveDirection::RIGHT, deltaTime);

		// Orientation
		static const float epsilon = 0.00000001f;
		bool cursorOffsetChanged = (fabs(_cursorOffsetX) > epsilon) || (fabs(_cursorOffsetY) > epsilon);
		camera.Rotate(_cursorOffsetX, _cursorOffsetY);

		// Zoom
		camera.Zoom(_scrollOffsetY);

		// Reset all values
		_cursorOffsetX = 0.0f;
		_cursorOffsetY = 0.0f;
		_scrollOffsetY = 0.0f;
	}
}

void InputManager::ProcessToolInput(float deltaTime)
{
	// Set active tool
	if (_keys[GLFW_KEY_T]) _sceneEditor->SetActiveTool(EToolType::TRANSLATION);
	if (_keys[GLFW_KEY_R]) _sceneEditor->SetActiveTool(EToolType::ROTATION);
	if (_keys[GLFW_KEY_S] && !_rightMouseButtonPressed) _sceneEditor->SetActiveTool(EToolType::SCALE);

	// Process tool input
	auto activeTool = _sceneEditor->GetActiveTool();
	activeTool->ProcessToolInput(_sceneEditor, _keys, deltaTime);
}

string InputManager::SelectFile()
{
	try
	{
		OpenFileDialog dialog;
		dialog.Show();

		return dialog.Result;
	}
	catch (...)
	{
		Logger::LogError(LOG_CHANNEL, "InputManager:: Failed to select file");
		return "";
	}
}

string InputManager::SaveFile()
{
	try
	{
		SaveFileDialog dialog;
		dialog.Show();

		return dialog.Result;
	}
	catch (...)
	{
		Logger::LogError(LOG_CHANNEL, "InputManager:: Failed to select file");
		return "";
	}
}

void InputManager::UpdateCursorPosition(double xpos, double ypos)
{
	static bool firstMouseEnter = true;

	float mouseX = float(xpos);
	float mouseY = float(ypos);

	if (firstMouseEnter)
	{
		_lastMouseX = mouseX;
		_lastMouseY = mouseY;

		firstMouseEnter = false;
	}

	_cursorOffsetX = (mouseX - _lastMouseX) * _cursorSensitivity;
	_cursorOffsetY = (_lastMouseY - mouseY) * _cursorSensitivity;

	_lastMouseX = mouseX;
	_lastMouseY = mouseY;
}

void InputManager::UpdateScrollOffset(double xOffset, double yOffset)
{
	_scrollOffsetY = (float)yOffset;
}

void InputManager::UpdateMouseButtonState(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			_rightMouseButtonPressed = true;
			_sceneEditor->HideCursor();
		}
		else if (action == GLFW_RELEASE)
		{
			_rightMouseButtonPressed = false;
			_sceneEditor->ShowCursor();
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		_sceneEditor->TryPickEntity(_lastMouseX, _lastMouseY);
	}
}

void InputManager::OnFramebufferSizeChanged(int width, int height)
{
	if (height != 0)
	{
		GLfloat aspectRatio = ((float)width) / height;
		
		auto& camera = _sceneEditor->GetLoadedScene().GetCamera();
		camera.SetAspectRatio(aspectRatio);

		_sceneEditor->SetScreenSize(width, height);
	}
}

void InputManager::ProcessAttachToolInput(int key, int action) 
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_TAB && _attachToolState == 0) {
			_parentIndex = _sceneEditor->GetSelectedEntityIndex();
			Logger::LogInfo(LOG_CHANNEL, "Attach Tool - You are attaching entity " + std::to_string(_parentIndex));
			_attachToolState = 1;
		}
		else if (key == GLFW_KEY_TAB && _attachToolState == 1) {
			if (_sceneEditor->GetSelectedEntityIndex() != _parentIndex && !IsParent(_parentIndex, _sceneEditor->GetSelectedEntityIndex())) {
				auto parent = _sceneEditor->GetEntityAtIndex(_parentIndex);
				parent->linkChild(_sceneEditor->GetSelectedEntityIndex());
				Logger::LogInfo(LOG_CHANNEL, "Attach Tool - You have linked parent entity " + std::to_string(_parentIndex) + " to child entity " + std::to_string(_sceneEditor->GetSelectedEntityIndex()) + "\n");
			}
			else
			{
				Logger::LogInfo(LOG_CHANNEL, "Attach Tool - Cannot link to this entity " + std::to_string(_parentIndex) + "\n");
			}
			_attachToolState = 0;
		}
	}
}

bool InputManager::IsParent(int parent, int child) {
	vector<int> children = _sceneEditor->GetEntityAtIndex(child)->GetChildren();
	for each(int i in children) {
		if (i == parent)
			return true;
		if (IsParent(parent, i))
			return true;
	}
	return false;
}
