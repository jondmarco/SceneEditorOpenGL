#include "Scene.h"

#include "SceneEntity.h"


Scene::~Scene()
{
	for(auto& entity : _entities)
	{
		delete(entity);
		entity = nullptr;
	}
}

void Scene::AddEntity(SceneEntity* entity, bool normalize)
{
	if(normalize)
		entity->Normalize();

	_entities.push_back(entity);
}

Camera& Scene::GetCamera()
{
	return _camera;
}

vector<SceneEntity*>& Scene::GetEntities()
{
	return _entities;
}

void Scene::AddLight(Light* light)
{
	_lightSources.push_back(light);
}

vector<Light*>& Scene::GetLightSources()
{
	return _lightSources;
}

