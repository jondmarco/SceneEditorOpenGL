#include "PhysicsSystem.h"
#include "Services\Logger.h"

#include "Game\SceneEntity.h"

#include "glm\gtc\matrix_transform.hpp"

#include <algorithm>


PhysicsSystem::PhysicsSystem()
{
	InitializePhysics();
	CreatePhysicsScene();
	ConnectToVisualDebugger();
}

PhysicsSystem::~PhysicsSystem()
{
	if (_debuggerConnection != nullptr)
	{
		_debuggerConnection->release();
	}

	_physics->release();
	_foundation->release();
}

void PhysicsSystem::Simulate(float deltaTime)
{
	// Run synchronous simulation
	_scene->simulate(deltaTime);
	_scene->fetchResults(true);

	// Update scene entities
	PxU32 nbActiveTransforms;
	const PxActiveTransform* activeTransforms = _scene->getActiveTransforms(nbActiveTransforms);

	for (PxU32 i = 0; i < nbActiveTransforms; ++i)
	{
		SceneEntity* sceneEntity = static_cast<SceneEntity*>(activeTransforms[i].userData);

		auto transform = activeTransforms[i].actor2World;

		// Get new position
		auto pxPosition = transform.p;
		vec3 position = vec3(pxPosition.x, pxPosition.y, pxPosition.z);

		// Get new rotation
		PxReal angle;
		PxVec3 pxAxis; 
		transform.q.toRadiansAndUnitAxis(angle, pxAxis);
		vec3 axis = vec3(pxAxis.x, pxAxis.y, pxAxis.z);

		// Update entity transformation matrix
		sceneEntity->SetTransormationMatrix(position, angle, axis);
	}
}

SceneEntity* PhysicsSystem::CastRay(vec3 glmOrigin, glm::vec3 glmDirection)
{
	PxVec3 origin(glmOrigin.x, glmOrigin.y, glmOrigin.z);					// [in] Ray origin
	PxVec3 unitDir(glmDirection.x, glmDirection.y, glmDirection.z);			// [in] Normalized ray direction
	PxReal maxDistance = PX_MAX_REAL;										// [in] Raycast max distance
	PxRaycastBuffer hit;													// [out] Raycast results

	bool status = _scene->raycast(origin, unitDir, maxDistance, hit);
	if (status)
	{
		return static_cast<SceneEntity*>(hit.block.actor->userData);
	}
	else
	{
		return nullptr;
	}	
}

PhysicsComponent* PhysicsSystem::CreatePhysicsComponent(Model* model)
{
	// Calculated AABB extents
	auto minPoint = model->GetMinPoint();
	auto maxPoint = model->GetMaxPoint();

	auto extentX = maxPoint.x - minPoint.x;
	auto extentY = maxPoint.y - minPoint.y;
	auto extentZ = maxPoint.z - minPoint.z;

	auto maxExtent = std::max(extentX, std::max(extentY, extentZ));

	if (maxExtent > 1)
	{
		auto scaleFactor = 1.0f / maxExtent;

		extentX *= scaleFactor;
		extentY *= scaleFactor;
		extentZ *= scaleFactor;
	}

	// Create an actor based on the actor type
	PxRigidDynamic* actor = _physics->createRigidDynamic(PxTransform(0,0,0));
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	
	// Create a collision box 
	PxShape* boxCollider = actor->createShape(PxBoxGeometry(extentX/2, extentY/2, extentZ/2), *_defaultMaterial);
	boxCollider->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	boxCollider->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	// Add actor to the physics scene graph
	_scene->addActor(*actor);

	return new PhysicsComponent(actor);
}

void PhysicsSystem::InitializePhysics()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!_foundation)
	{
		Logger::LogError(LOG_CHANNEL, "PxCreateFoundation failed!");
	}

	bool recordMemoryAllocations = true;
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, PxTolerancesScale(), recordMemoryAllocations);
	if (!_physics)
	{
		Logger::LogError(LOG_CHANNEL, "PxCreatePhysics failed!");
	}
}

void PhysicsSystem::CreatePhysicsScene()
{
	PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		_dispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!_dispatcher)
		{
			Logger::LogError(LOG_CHANNEL, "PxDefaultCpuDispatcherCreate failed!");
		}
			
		sceneDesc.cpuDispatcher = _dispatcher;
	}

	if (!sceneDesc.filterShader)
	{
		
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	}

	sceneDesc.flags.set(PxSceneFlag::eENABLE_ACTIVETRANSFORMS);

	_scene = _physics->createScene(sceneDesc);
	_scene->setSimulationEventCallback(&_simulationCallback);

	if (!_scene)
	{
		Logger::LogError(LOG_CHANNEL, "createScene failed!");
	}

	// Create default physics material
	_defaultMaterial = _physics->createMaterial(0.5f, 0.5f, 0.1f);    // Static friction, dynamic friction, restitution
	if (!_defaultMaterial)
	{
		Logger::LogError(LOG_CHANNEL, "createMaterial failed!");
	}		
}

void PhysicsSystem::ConnectToVisualDebugger()
{
	if (_physics->getPvdConnectionManager() != NULL)
	{
		// Setup connection parameters
		const char*     pvd_host_ip = "127.0.0.1";
		int             port = 5425;
		unsigned int    timeout = 100;

		PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
		connectionFlags = PxVisualDebuggerConnectionFlag::eDEBUG;
		_physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);

		// Try to connect
		_debuggerConnection = PxVisualDebuggerExt::createConnection(_physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);

		if (_debuggerConnection != nullptr)
		{
			Logger::LogDebug(LOG_CHANNEL, "Connected to visual debugger\n");
		}
		else
		{
			Logger::LogDebug(LOG_CHANNEL, "Visual debugger not connected\n");
		}
	}
}

void PhysicsSystem::DeletePhysicsComponent(PhysicsComponent* physComp)
{
	auto actor = physComp->_actor;
	_scene->removeActor(*actor);
}
