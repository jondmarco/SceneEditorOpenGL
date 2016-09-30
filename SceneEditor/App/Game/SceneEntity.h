#pragma once

#include "PhysicsComponent.h"

#include "glm\glm.hpp"

#include <vector>

class Model;

using std::vector;
using glm::mat4;
using glm::vec3;

class SceneEntity
{
public:

	friend class GameStorage;
	friend class Grid;

	SceneEntity(Model* model, PhysicsComponent* physicsComponent);
	~SceneEntity();

	void Normalize();

	SceneEntity& Translate(float dx, float dy, float dz);
	SceneEntity& Rotate(float angleInDegrees, float u, float v, float w);
	SceneEntity& Scale(float a, float b, float c);

	Model* GetModel();
	PhysicsComponent* GetPhysicsComponent();

	mat4& GetModelMatrix();
	void SetTransormationMatrix(vec3& position, float rotationAngle, vec3& rotationAxis);
	void SetTransformationMatrix(mat4 transformationMatrix);

	bool IsSelected();
	void SetSelected(bool isSelected);

	bool IsValid();

	glm::vec3& GetBaseColor();
	void setBaseColor(float r, float g, float b);

	float& getMetallness();
	float& getRoughness();

	void setMetallness(float metallness);
	void setRoughness(float roughness);

	bool isLight();
	void setIsLight(bool _isLight);

	void linkChild(int index);
	vector<int>& GetChildren();

private:

	// Model Matrix
	mat4 _modelMatrix;

	mat4 _transformationMatrix;
	mat4 _normalizationMatrix;

	// Render Component
	Model* _model;

	glm::vec3 baseColor;
	float metallness;
	float roughness;

	// Physics Component
	PhysicsComponent* _physicsComponent;

	// State
	bool _isSelected = false;
	bool _isLight = false;

	// Children
	vector<int> _children;
};
