#pragma once

#include "Data\Model.h"
#include "Game\SceneEntity.h"
#include "Game\PhysicsComponent.h"
#include "Services\Logger.h"

#include "PhysX\PxPhysicsAPI.h"

using namespace physx;
using glm::vec3;
using glm::vec4;


class PhysicsSystem
{
public:
	
	PhysicsSystem();
	~PhysicsSystem();

	void Simulate(float deltaTime);

	PhysicsComponent* CreatePhysicsComponent(Model* model);

	SceneEntity* CastRay(vec3 origin, vec3 direction);

	void DeletePhysicsComponent(PhysicsComponent* physComp);

private:

	class SimulationCallbackHandler : public PxSimulationEventCallback
	{
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs) override;

		// Not implemented
		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {};
		virtual void onWake(PxActor** actors, PxU32 count) override {};
		virtual void onSleep(PxActor** actors, PxU32 count) override {};
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {};
	};

	void InitializePhysics();
	void CreatePhysicsScene();

	void ConnectToVisualDebugger();

	PxFoundation* _foundation;
	PxPhysics* _physics;

	PxScene* _scene;
	PxCpuDispatcher* _dispatcher;

	PxMaterial* _defaultMaterial;

	SimulationCallbackHandler _simulationCallback;

	PxVisualDebuggerConnection* _debuggerConnection;
	const string LOG_CHANNEL = "PhysicsSystem";
};
