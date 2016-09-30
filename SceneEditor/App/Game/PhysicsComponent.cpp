#include "PhysicsComponent.h"

#include "glm\gtc\quaternion.hpp"

using namespace physx;


void PhysicsComponent::SetEntityReference(SceneEntity* entity)
{
	_actor->userData = entity;
}

void PhysicsComponent::TranslateActor(float dx, float dy, float dz)
{
	auto transform = _actor->getGlobalPose();
	transform.p += PxVec3(dx, dy, dz);
	_actor->setGlobalPose(transform);
}

void PhysicsComponent::RotateActor(float angleInRadians, vec3& rotationAxis)
{
	auto transform = _actor->getGlobalPose();

	auto quat = glm::angleAxis(angleInRadians, rotationAxis);
	transform.q *= PxQuat(quat.x, quat.y, quat.z, quat.w);

	_actor->setGlobalPose(transform);
}

void PhysicsComponent::RotateActor(quat quat)
{
	auto transform = _actor->getGlobalPose();
	transform.q *= PxQuat(quat.x, quat.y, quat.z, quat.w);
	_actor->setGlobalPose(transform);
}

void PhysicsComponent::ScaleActor(float a, float b, float c)
{
	// Get first shape attached to the actor
	auto buffer = new PxShape*[1];
	_actor->getShapes(buffer, 1);
	auto shape = buffer[0];

	// Get box geometry from the shape
	PxBoxGeometry boxCollider;
	auto collisionBox = shape->getBoxGeometry(boxCollider);

	// Scale the box
	boxCollider.halfExtents = boxCollider.halfExtents.multiply(PxVec3(a, b, c));

	// Apply scaled box geometry
	shape->setGeometry(boxCollider);

	// HACK:: Translate actor by a very small distance to trigger overlap recalculation
	// TODO:: Find a better way to do this
	auto transform = _actor->getGlobalPose();
	//transform.p = PxVec3(transform.p.x / a, transform.p.y / b, transform.p.z / c);
	_actor->setGlobalPose(transform);

	delete[] buffer;
}

bool PhysicsComponent::IsValid()
{
	return (_numberOfOverlaps == 0);
}
