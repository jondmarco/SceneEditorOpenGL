#pragma once

#include "Camera.h"

#include <vector>

class SceneEntity;
class Light;

using std::vector;


class Scene
{
public:

	~Scene();

	void AddEntity(SceneEntity* entity, bool normalize = true);
	vector<SceneEntity*>& GetEntities();

	void AddLight(Light* light);
	vector<Light*>& GetLightSources();

	Camera& GetCamera();

private:

	Camera _camera;
	vector<SceneEntity*> _entities;
	vector<Light*> _lightSources;
};
