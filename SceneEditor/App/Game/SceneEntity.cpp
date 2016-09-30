#include "SceneEntity.h"

#include "Data\Model.h"

#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_decompose.hpp"

#include <algorithm>

using glm::vec4;
using glm::quat;


SceneEntity::SceneEntity(Model* model, PhysicsComponent* physicsComponent) : _model(model), _physicsComponent(physicsComponent)
{
	_physicsComponent->SetEntityReference(this);
}

SceneEntity::~SceneEntity()
{
	delete(_model);
	_model = nullptr;

	delete(_physicsComponent);
	_physicsComponent = nullptr;
}

void SceneEntity::Normalize()
{
	auto minPoint = _model->GetMinPoint();
	auto maxPoint = _model->GetMaxPoint();

	auto center = (maxPoint + minPoint) / 2.0f;

	auto extentX = maxPoint.x - minPoint.x;
	auto extentY = maxPoint.y - minPoint.y;
	auto extentZ = maxPoint.z - minPoint.z;

	auto maxExtent = std::max(extentX, std::max(extentY, extentZ));

	if (maxExtent > 1)
	{
		auto scaleFactor = 1.0f / maxExtent;
		_normalizationMatrix = glm::scale(_normalizationMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	}

	_normalizationMatrix = glm::translate(_normalizationMatrix, glm::vec3(center.x, -center.y, -center.z));
}

SceneEntity& SceneEntity::Translate(float dx, float dy, float dz)
{
	_transformationMatrix = glm::translate(_transformationMatrix, glm::vec3(dx, dy, dz));
	_physicsComponent->TranslateActor(dx, dy, dz);

	return *this;
}

SceneEntity& SceneEntity::Rotate(float angleInDegrees, float u, float v, float w)
{
	auto angleInRadians = glm::radians(angleInDegrees);
	_transformationMatrix = glm::rotate(_transformationMatrix, angleInRadians, glm::vec3(u, v, w));

	_physicsComponent->RotateActor(angleInRadians, glm::vec3(u, v, w));

	return *this;
}

SceneEntity& SceneEntity::Scale(float a, float b, float c)
{
	_transformationMatrix = glm::scale(_transformationMatrix, glm::vec3(a, b, c));
	_physicsComponent->ScaleActor(a, b, c);

	return *this;
}

Model* SceneEntity::GetModel()
{
	return _model;
}

PhysicsComponent* SceneEntity::GetPhysicsComponent()
{
	return _physicsComponent;
}

glm::mat4& SceneEntity::GetModelMatrix()
{
	_modelMatrix = _transformationMatrix * _normalizationMatrix;

	return _modelMatrix;
}

void SceneEntity::SetTransormationMatrix(vec3& position, float rotationAngle, vec3& rotationAxis)
{
	// Decompose current transformation matrix into components
	vec3 scale, translation, skew;
	vec4 perspective;
	quat rotation;

	glm::decompose(_transformationMatrix, scale, rotation, translation, skew, perspective);

	// Reset transformation matrix
	_transformationMatrix = mat4(1.0);

	// Apply new translation
	_transformationMatrix = glm::translate(_transformationMatrix, position);
	// Apply new rotation
	_transformationMatrix = glm::rotate(_transformationMatrix, rotationAngle, rotationAxis);
	// Keep scaling from the previous transformation matrix
	_transformationMatrix = glm::scale(_transformationMatrix, scale);
}

void SceneEntity::SetTransformationMatrix(mat4 transformationMatrix)
{
	// Decompose current transformation matrix into components
	vec3 scale, translation, skew;
	vec4 perspective;
	quat rotation;

	glm::decompose(transformationMatrix, scale, rotation, translation, skew, perspective);

	// Set transformation matrix
	_transformationMatrix = transformationMatrix;

	// Update physics components
	_physicsComponent->TranslateActor(translation.x, translation.y, translation.z);
	_physicsComponent->RotateActor(rotation);
	_physicsComponent->ScaleActor(scale.x, scale.y, scale.z);

}

bool SceneEntity::IsSelected()
{
	return _isSelected;
}

void SceneEntity::SetSelected(bool isSelected)
{
	_isSelected = isSelected;
}

bool SceneEntity::IsValid()
{
	return _physicsComponent->IsValid();
}

glm::vec3& SceneEntity::GetBaseColor()
{
	return baseColor;
}

void SceneEntity::setBaseColor(float r, float g, float b)
{
	this->baseColor = vec3(r, g, b);
}

float& SceneEntity::getMetallness()
{
	return metallness;
}

float& SceneEntity::getRoughness()
{
	return roughness;
}

void SceneEntity::setMetallness(float metallness)
{
	this->metallness = metallness;
}

void SceneEntity::setRoughness(float roughness)
{
	this->roughness = roughness;
}

bool SceneEntity::isLight()
{
	return _isLight;
}

void SceneEntity::setIsLight(bool _isLight)
{
	this->_isLight = _isLight;
}

void SceneEntity::linkChild(int index)
{
	for (int i = 0; i < _children.size(); i++) 
	{
		if (_children[i] == index) 
		{
			_children.erase(_children.begin() + i);
			return;
		}
	}

	_children.push_back(index);
}

vector<int>& SceneEntity::GetChildren()
{
	return _children;
}
