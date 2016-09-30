#pragma once

#include "glm\glm.hpp"
#include "PhysX\PxPhysicsAPI.h"

class SceneEntity;

using glm::vec3;
using glm::quat;
using physx::PxRigidActor;


class PhysicsComponent
{
public:

	friend class PhysicsSystem;

	PhysicsComponent(PxRigidActor* actor) : _actor(actor) {}

	void SetEntityReference(SceneEntity* entity);

	void TranslateActor(float dx, float dy, float dz);
	void RotateActor(float angleInRadians, vec3& rotationAxis);
	void RotateActor(quat quat);
	void ScaleActor(float a, float b, float c);

	bool IsValid();

private:

	PxRigidActor* _actor;

	int _numberOfOverlaps = 0;
};
