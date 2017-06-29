#include "PhysicsManager.h"
#include "PhysicsComponent.h"

// Justin Furtado
// 6/27/2017
// PhysicsManager.cpp
// Manages physics!

namespace Engine
{
	LinkedList<PhysicsComponent *> PhysicsManager::m_componentPtrs;

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
		m_componentPtrs.WalkList(PhysicsUpdateCallback, nullptr);
	}

	bool PhysicsManager::PhysicsUpdateCallback(PhysicsComponent * pComp, void * /*pData*/)
	{
		if (pComp->IsEnabled()) { pComp->PhysicsUpdate(); }
		return true;
	}

	void PhysicsManager::RegisterComponent(PhysicsComponent * pCompToAdd)
	{
		m_componentPtrs.AddToListFront(pCompToAdd);
	}

	void PhysicsManager::UnregisterComponent(PhysicsComponent * pCompToRemove)
	{
		m_componentPtrs.RemoveFirstFromList(pCompToRemove);
	}
}