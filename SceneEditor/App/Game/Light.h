#pragma once

#include "glm\glm.hpp"
#include "Game\SceneEntity.h"
#include "Services\ModelLoader.h"

#define _FILE_LIGHT_MODEL "Assets\\Models\\Primitives\\sphere.obj"


class Light
{
public:
	friend class RenderSystem;

	Light(vec3 position, vec3 lightColor) : position(position), lightColor(lightColor) {};

	glm::vec3 getPosition();
	glm::vec3 getColor();

	void setPosition(glm::vec3 position);
	void setColor(glm::vec3 lightColor);

	void addModel(SceneEntity* lightModel);
	SceneEntity* getEntity();

private:

	glm::vec3 position;
	glm::vec3 lightColor;

	SceneEntity* _lightModel;
};
