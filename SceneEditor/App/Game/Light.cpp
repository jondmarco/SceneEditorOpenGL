#pragma once

#include "Game/Light.h"
#include "Data\Core\Vertex.h"


glm::vec3 Light::getPosition()
{
	return position;
}
glm::vec3 Light::getColor()
{
	return lightColor;
}
void Light::setPosition(glm::vec3 position)
{
	this->position = position;
}
void Light::setColor(glm::vec3 lightColor)
{
	this->lightColor = lightColor;
}

void Light::addModel(SceneEntity* lightModel)
{
	_lightModel = lightModel;

	_lightModel->Translate(position.x, position.y, position.z);
	_lightModel->Scale(0.2f, 0.2f, 0.2f);

	_lightModel->setBaseColor(lightColor.x, lightColor.y, lightColor.z);
	_lightModel->setMetallness(1.0);
	_lightModel->setRoughness(1.0);
	_lightModel->setIsLight(true);
}

SceneEntity* Light::getEntity()
{
	return _lightModel;
}
