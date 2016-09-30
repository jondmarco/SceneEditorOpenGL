#include "SceneEditor.h"

#include "Utils\Utils.h"
#include "Game\SceneEntity.h"
#include "Game\Shader.h"
#include "Game\Light.h"

#include "glm\glm.hpp"

using glm::vec4;
using glm::mat4;


SceneEditor::SceneEditor() : _renderSystem(_openGLApp), _inputManager(this), _gameStorage(&_physicsSystem)
{}

void SceneEditor::runApp(bool loadScene, Mode mode)
{
	Initialize();
	SetMode(mode);

	// Make sure all inputs are set to false initially
	_inputManager.ResetInputStates();

	if (loadScene)
	{
		Scene* s = _gameStorage.load();

		for (SceneEntity* elem : s->GetEntities())
		{
			_scene.AddEntity(elem, false);
		}

		for (Light* light : s->GetLightSources())
		{
			_scene.AddLight(light);

			if (_mode == EDIT)
			{
				auto lightModel = _modelLoader.LoadMesh(_FILE_LIGHT_MODEL);
				auto physicsComponent = _physicsSystem.CreatePhysicsComponent(lightModel);

				auto lightEntity = new SceneEntity(lightModel, physicsComponent);

				light->addModel(lightEntity);
				_scene.AddEntity(light->getEntity());
			}
		}
	}

	GameLoop();
	Terminate();
}

// Application-level information and manipulation

void SceneEditor::BeginShutdown()
{
	_openGLApp.InitiateShutdown();
}

void SceneEditor::ShowCursor()
{
	_openGLApp.ShowCursor();
}

void SceneEditor::HideCursor()
{
	_openGLApp.HideCursor();
}

void SceneEditor::SetScreenSize(int width, int height)
{
	_windowWidth = width;
	_windowHeight = height;
}

// Game loop
void SceneEditor::GameLoop()
{
	while (!_openGLApp.ShouldTerminate())
	{
		auto deltaTime = GetDeltaTime();

		ProcessEvents();
		Update(deltaTime);
		Render(deltaTime);

		ReportErrors();
	}
}

// Scene manipulation

Scene& SceneEditor::GetLoadedScene()
{
	return _scene;
}

void SceneEditor::LoadEntity()
{
	auto fileName = _inputManager.SelectFile();

	auto model = _modelLoader.LoadMesh(fileName);
	auto physicsComponent = _physicsSystem.CreatePhysicsComponent(model);

	auto entity = new SceneEntity(model, physicsComponent);

	_scene.AddEntity(entity);

	// If this is the first entity loaded, select it
	if (_scene.GetEntities().size() == 1)
	{
		entity->SetSelected(true);
	}
}

SceneEntity* SceneEditor::GetSelectedEntity()
{
	auto sceneObjects = _scene.GetEntities();

	if (sceneObjects.size() > 0)
	{
		return sceneObjects.at(_selectedEntityIndex);
	}

	return nullptr;
}

void SceneEditor::TryPickEntity(float cursorX, float cursorY)
{
	// Remove selected flag from previously selected entity
	auto selectedEntity = GetSelectedEntity();
	if (selectedEntity != nullptr)
	{
		selectedEntity->SetSelected(false);
	}

	auto& camera = _scene.GetCamera();

	// Find ray origin and target in Normalized Device Coordinates
	vec4 rayOriginNDC((cursorX/_windowWidth - 0.5f) * 2.0f, -(cursorY/_windowHeight - 0.5f) * 2.0f, -1.0, 1.0f);
	vec4 rayTargetNDC((cursorX/_windowWidth - 0.5f) * 2.0f, -(cursorY/_windowHeight - 0.5f) * 2.0f, 0.0, 1.0f);

	// Transfrom from NDC to World Space
	mat4 inverseTransform = glm::inverse(camera.GetProjectionMatrix() * camera.GetViewMatrix());

	vec4 rayOrigin = inverseTransform * rayOriginNDC;
	rayOrigin /= rayOrigin.w;
	vec4 rayTarget = inverseTransform  * rayTargetNDC;
	rayTarget /= rayTarget.w;

	// Compute ray direction
	vec3 rayDirection = glm::normalize(vec3(rayTarget - rayOrigin));

	// Run intersection query
	auto intersectedEntity = _physicsSystem.CastRay(vec3(rayOrigin), rayDirection);
	if (intersectedEntity != nullptr)
	{
		auto& sceneEntities = _scene.GetEntities();

		// Find index of the intersected entity
		auto index = std::distance(sceneEntities.begin(), std::find(sceneEntities.begin(), sceneEntities.end(), intersectedEntity));

		// Change selection
		_selectedEntityIndex = index;

		// Add selection flag to newly selected entity
		GetSelectedEntity()->SetSelected(true);
	}
}

void SceneEditor::SelectNextEntity()
{
	// Remove selected flag from previously selected entity
	auto selectedEntity = GetSelectedEntity();
	if (selectedEntity != nullptr)
	{
		selectedEntity->SetSelected(false);
	}

	// Change selection
	_selectedEntityIndex++;

	if (_selectedEntityIndex == _scene.GetEntities().size())
	{
		_selectedEntityIndex = 0;
	}

	// Add selection flag to newly selected entity
	GetSelectedEntity()->SetSelected(true);
}

void SceneEditor::SelectPreviousEntity()
{
	// Remove selected flag from previously selected entity
	auto selectedEntity = GetSelectedEntity();
	if (selectedEntity != nullptr)
	{
		selectedEntity->SetSelected(false);
	}
	
	// Change selection
	if (_selectedEntityIndex == 0)
	{
		_selectedEntityIndex = _scene.GetEntities().size() - 1;
	}
	else
	{
		_selectedEntityIndex--;
	}

	// Add selection flag to newly selected entity
	GetSelectedEntity()->SetSelected(true);
}

SceneEntity* SceneEditor::GetEntityAtIndex(int index)
{
	auto sceneObjects = _scene.GetEntities();

	if (sceneObjects.size() > 0)
	{
		return sceneObjects.at(index);
	}

	return nullptr;
}

void SceneEditor::SelectEntityAtIndex(int index) 
{
	_selectedEntityIndex = index;
}

int SceneEditor::GetSelectedEntityIndex()
{
	return _selectedEntityIndex;
}

// Tools

ITransformTool* SceneEditor::GetActiveTool()
{
	return _activeTool;
}

void SceneEditor::SetActiveTool(EToolType tool)
{
	switch (tool)
	{
	case EToolType::TRANSLATION:
		_activeTool = &_translationTool;
		break;
	case EToolType::ROTATION:
		_activeTool = &_rotationTool;
		break;
	case EToolType::SCALE:
		_activeTool = &_scaleTool;
		break;
	}
}


// Private helper methods

void SceneEditor::Initialize()
{
	_openGLApp.Initialize(_defaultWindowWidth, _defaultWindowHeight, _windowTitle, _isWindowResizable);
	_openGLApp.SetInputManager(&_inputManager);
}

void SceneEditor::Terminate()
{
	_openGLApp.Terminate();
}

float SceneEditor::GetDeltaTime()
{
	static auto lastFrameTime = glfwGetTime();
	auto currentFrameTime = glfwGetTime();

	auto deltaTime = (float)(currentFrameTime - lastFrameTime);

	lastFrameTime = currentFrameTime;

	return deltaTime;
}

void SceneEditor::ProcessEvents()
{
	_openGLApp.PollEvents();
}

void SceneEditor::Update(float deltaTime)
{
	_inputManager.ProcessInput(deltaTime);

	// Update physics
	_physicsSystem.Simulate(deltaTime);
}

void SceneEditor::Render(float deltaTime)
{
	// TODO:: move this shader to RenderSystem as default shader
	static Shader shader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/PhongFragmentShader.glsl");

	_renderSystem.ClearScreen();

	_renderSystem.RenderScene(_scene, shader, _mode == EDIT);
	
	RenderUI(deltaTime);

	_renderSystem.SwapBuffers();

}

void SceneEditor::ReportErrors()
{
	_openGLApp.CheckOpenGLErrors();
}

void SceneEditor::RenderUI(float deltaTime)
{
	RenderFPSCounter(deltaTime);
	RenderActiveToolInfo();
}

void SceneEditor::RenderFPSCounter(float deltaTime)
{
	auto textColor = glm::vec3(0.5, 0.8f, 0.2f);

	// Initialize static variables
	static double lastPrintTime = glfwGetTime();

	static string frameTime = "0";
	static string FPS = "0";

	// Update current time and frame count
	auto currentTime = glfwGetTime();

	// Update FPS count if last print was more than 1 sec ago
	if (currentTime - lastPrintTime >= 1.0)
	{
		frameTime = Utils::to_string_with_precision<float>(deltaTime * 1000, 4);
		FPS = Utils::to_string_with_precision<float>(1.0f / deltaTime, 4);

		lastPrintTime = currentTime;
	}

	_renderSystem.RenderText("Frame time: " + frameTime + " ms", _windowWidth - 256.0f, _windowHeight - 32.0f, 0.5f, textColor, _windowWidth, _windowHeight);
	_renderSystem.RenderText("FPS: " + FPS, _windowWidth - 256.0f, _windowHeight - 64.0f, 0.5f, textColor, _windowWidth, _windowHeight);
}

void SceneEditor::RenderActiveToolInfo()
{
	auto textColor = glm::vec3(0.5, 0.8f, 0.2f);

	auto activeToolName = _activeTool->GetToolName();
	auto activeAxis = _activeTool->GetActiveAxis();

	_renderSystem.RenderText("Active tool: " + activeToolName, 16.0f, _windowHeight - 32.0f, 0.5f, textColor, _windowWidth, _windowHeight);
	_renderSystem.RenderText("Active axis: " + activeAxis, 16.0f, _windowHeight - 64.0f, 0.5f, textColor, _windowWidth, _windowHeight);
}

void SceneEditor::SaveScene()
{
	_gameStorage.save(&_scene);
}

Mode SceneEditor::GetMode()
{
	return _mode;
}

void SceneEditor::SetMode(Mode mode)
{
	this->_mode = mode;
}

void SceneEditor::AddLight(vec3 lightp, vec3 lightc)
{
	Light* light = new Light(lightp, lightc);

	if (_mode == EDIT)
	{
		auto lightModel = _modelLoader.LoadMesh(_FILE_LIGHT_MODEL);
		auto physicsComponent = _physicsSystem.CreatePhysicsComponent(lightModel);

		auto lightEntity = new SceneEntity(lightModel, physicsComponent);

		light->addModel(lightEntity);
		_scene.AddEntity(light->getEntity());
	}

	_scene.AddLight(light);
}

void SceneEditor::DeleteEntity()
{
	//if the object is colliding with another object we cant delete it
	if (!_scene.GetEntities().at(_selectedEntityIndex)->GetPhysicsComponent()->IsValid())
		return;

	vector<SceneEntity*>::iterator it;

	int index = 0;
	for (it = _scene.GetEntities().begin(); it != _scene.GetEntities().end();)
	{
		if (index == _selectedEntityIndex)
		{
			_physicsSystem.DeletePhysicsComponent(_scene.GetEntities().at(index)->GetPhysicsComponent());
			delete *it;
			it = _scene.GetEntities().erase(it);
			break;
		}
		else
		{
			++index;
			++it;
		}		
	}

	//UPDATE CHILDREN INDICES

	for (int i = 0; i < _scene.GetEntities().size(); ++i)
	{

		for (int j = 0; j < _scene.GetEntities().at(i)->GetChildren().size(); ++j)
		{
			if (_scene.GetEntities().at(i)->GetChildren().at(j) > _selectedEntityIndex)
			{
				_scene.GetEntities().at(i)->GetChildren()[j] = _scene.GetEntities().at(i)->GetChildren().at(j) - 1;
			}
			else if (_scene.GetEntities().at(i)->GetChildren().at(j) == _selectedEntityIndex)
			{
				_scene.GetEntities().at(i)->GetChildren().erase(_scene.GetEntities().at(i)->GetChildren().begin() + j);
			}
		}
	}

	_selectedEntityIndex = 0;
}

