#pragma once
#include <string>
#include "../Game/Scene.h"
#include "../System/Input/InputManager.h"
#include "ModelLoader.h"

#include "System\Physics\PhysicsSystem.h"

class GameStorage
{
public:

	GameStorage(PhysicsSystem* physicsSystem) : _physicsSystem(physicsSystem) {};
	Scene* load();
	void save(Scene* scene);

private:

	ModelLoader _modelLoader;
	InputManager _inputManager;
	PhysicsSystem* _physicsSystem;
};