#pragma once

#include "GL\glew.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

using glm::vec3;
using glm::mat4;


enum class MoveDirection : unsigned int
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT
};


class Camera
{
public:

	friend class Grid;
	friend class RenderSystem;

	Camera();

	mat4& GetViewMatrix();
	mat4& GetProjectionMatrix();

	void Move(MoveDirection direction, float deltaTime);
	void Rotate(float deltaYaw, float deltaPitch);
	void Zoom(float magnitude);
	void SetAspectRatio(GLfloat aspectRatio);

private:

	// Transformation matrices
	mat4 _viewMatrix;
	mat4 _projectionMatrix;


	// Camera vectors
	vec3 _position = vec3(-5.0f, 3.5f, 5.0f);
	vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);
	vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Camera orientation
	GLfloat _pitch = 0.0f;
	GLfloat _yaw = -90.0f;

	// Projection values
	GLfloat _fieldOfView = 45.0f;
	GLfloat _aspectRatio = 1;
	const GLfloat _near = 0.1f;
	const GLfloat _far = 100.0f;

	// Input sensitivities
	const GLfloat _moveSpeed = 2.5f;
};
