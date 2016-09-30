#include"PhysicsSystem.h"


void PhysicsSystem::SimulationCallbackHandler::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		auto entity1 = static_cast<SceneEntity*>(pairs[i].triggerActor->userData)->GetPhysicsComponent();
		auto entity2 = static_cast<SceneEntity*>(pairs[i].otherActor->userData)->GetPhysicsComponent();

		if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			entity1->_numberOfOverlaps++;
			entity2->_numberOfOverlaps++;
		}
		else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			entity1->_numberOfOverlaps--;
			entity2->_numberOfOverlaps--;

			// Sanity check
			if (entity1->_numberOfOverlaps < 0 || entity2->_numberOfOverlaps < 0)
			{
				Logger::LogError("SimulationCallback::OnContact", "Negative number of overlap registered for entity");
			}
		}
		else
		{
			Logger::LogWarning("SimulationCallback::OnContact", "Unexpected trigger pair status");
		}
	}
}
