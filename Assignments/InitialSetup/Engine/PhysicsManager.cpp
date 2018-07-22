#include "PhysicsManager.h"
#include "PhysicsComponent.h"

// Justin Furtado
// 6/27/2017
// PhysicsManager.cpp
// Manages physics!

namespace Engine
{
	DynamicArray<PhysicsComponent *> PhysicsManager::s_components;

	bool PhysicsManager::Initialize()
	{
		GameLogger::Log(MessageType::cInfo, "PhysicsManager Initialized Successfully!\n");
		return true;
	}

	bool PhysicsManager::Shutdown()
	{
		GameLogger::Log(MessageType::cInfo, "PhysicsManager Shutdown Successfully!\n");
		return true;
	}

	void PhysicsManager::Update()
	{
		unsigned iMax = s_components.GetCount() - 1;
		unsigned jMax = iMax + 1;
		for (unsigned i = 0; i < iMax; ++i)
		{
			for (unsigned j = i + 1; j < jMax; ++j)
			{
				PhysicsComponent *pComp1 = s_components[i];
				PhysicsComponent *pComp2 = s_components[j];
				float r1 = pComp1->GetRadius(), r2 = pComp2->GetRadius();
				float r = r1 + r2;
				if ((pComp1->GetPosition() - pComp2->GetPosition()).LengthSquared() < (r*r))
				{
					pComp1->CalcCollisionInfo(pComp2);
					pComp2->CalcCollisionInfo(pComp1);
					pComp1->ResolveCollision();
					pComp2->ResolveCollision();
				}
			}
		}
	}

	void PhysicsManager::RegisterComponent(PhysicsComponent * pCompToAdd)
	{
		s_components.Add(pCompToAdd);
	}

	void PhysicsManager::UnregisterComponent(PhysicsComponent * pCompToRemove)
	{
		s_components.RemoveFirst(pCompToRemove);
	}
}; 