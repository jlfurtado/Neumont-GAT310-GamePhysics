#include "PhysicsComponent.h"
#include "SpatialComponent.h"
#include "GraphicalObjectComponent.h"
#include "MathUtility.h"

// Justin Furtado
// 6/27/2017
// PhysicsComponent.cpp
// A comonent for entities to have physics


namespace Engine
{
	PhysicsComponent::PhysicsComponent()
	{
	}

	PhysicsComponent::~PhysicsComponent()
	{
	}

	bool PhysicsComponent::Initialize()
	{
		m_pSpatialComp = GetSiblingComponent<SpatialComponent>();
		if (!m_pSpatialComp) { GameLogger::Log(MessageType::cError, "Cannot do physics! Entity has PhysicsComponent but no SpatialComponent!\n"); return false; }

		m_pGobComp = GetSiblingComponent<GraphicalObjectComponent>();
		if (!m_pGobComp) { GameLogger::Log(MessageType::cError, "Cannot do physics! Entity has PhysicsComponent but no GobComponent!\n"); return false; }

		m_pSpatialComp->SetVelocity(MathUtility::GetRandSphereEdgeVec(10.0f));

		// todo: log success
		return true;
	}

	bool PhysicsComponent::PhysicsUpdate()
	{
		return true;
	}

}
