#include "Camera.h"

#include <algorithm>

using std::max;


Camera::Camera()
{
	// Set initial camera position
	Rotate(45, -30);
}

mat4& Camera::GetViewMatrix()
{
	_viewMatrix = glm::lookAt(_position, _position + _front, _up);
	return _viewMatrix;
}

mat4& Camera::GetProjectionMatrix()
{
	_projectionMatrix = glm::perspective(glm::radians(_fieldOfView), _aspectRatio, _near, _far);
	return _projectionMatrix;
}

void Camera::Move(MoveDirection direction, float deltaTime)
{
	auto moveDistance = _moveSpeed * deltaTime;

	if (direction == MoveDirection::FORWARD)
	{
		_position += moveDistance * _front;
	}
	else if (direction == MoveDirection::BACK)
	{
		_position -= moveDistance * _front;
	}
	else if (direction == MoveDirection::LEFT)
	{
		auto right = glm::normalize(glm::cross(_front, _up));
		_position -= moveDistance * right;
	}
	else if (direction == MoveDirection::RIGHT)
	{
		auto right = glm::normalize(glm::cross(_front, _up));
		_position += moveDistance * right;
	}
}

void Camera::Rotate(float deltaYaw, float deltaPitch)
{
	// Update yaw and pitch values
	_yaw += deltaYaw;
	_pitch += deltaPitch;

	// Clamp pitch between -89 and 89
	_pitch = std::max(-89.0f, std::min(_pitch, 89.0f));

	// Update front vector
	_front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front.y = sin(glm::radians(_pitch));
	_front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

	_front = glm::normalize(_front);
}

void Camera::Zoom(float magnitude)
{
	if (_fieldOfView >= 1.0f && _fieldOfView <= 45.0f)
	{
		_fieldOfView -= magnitude;
	}

	if (_fieldOfView <= 1.0f)
	{
		_fieldOfView = 1.0f;
	}
	else if (_fieldOfView >= 45.0f)
	{
		_fieldOfView = 45.0f;

	}
}

void Camera::SetAspectRatio(GLfloat aspectRatio)
{
	_aspectRatio = aspectRatio;
}
