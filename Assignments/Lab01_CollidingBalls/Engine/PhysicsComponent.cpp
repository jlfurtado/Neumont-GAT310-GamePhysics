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

		// todo: log success
		return true;
	}

	bool PhysicsComponent::PhysicsUpdate()
	{
		return true;
	}

	void PhysicsComponent::CalcCollisionInfo(PhysicsComponent * pOther)
	{
		Vec3 toOther = pOther->GetPosition() - m_pSpatialComp->GetPosition();
		float f = toOther.Length() - (pOther->m_radius + m_radius);
		m_tempPosStore = (m_pSpatialComp->GetPosition() + (toOther.Normalize() * (f / 2.0f)));

		float m1 = m_mass, m2 = pOther->GetMass();
		Vec3 v1 = m_pSpatialComp->GetVelocity(), v2 = pOther->m_pSpatialComp->GetVelocity();
		m_tempVelStore = ((v1 * (m1 - m2) + (2 * m2*v2)) / (m1 + m2));
	}

	void PhysicsComponent::ResolveCollision()
	{
		m_pSpatialComp->SetPosition(m_tempPosStore);
		m_pSpatialComp->SetVelocity(m_tempVelStore);
	}

	void PhysicsComponent::SetRadius(float radius)
	{
		m_radius = radius;
	}

	float PhysicsComponent::GetRadius() const
	{
		return m_radius;
	}

	Vec3 PhysicsComponent::GetPosition() const
	{
		return m_pSpatialComp->GetPosition();
	}

	void PhysicsComponent::SetMass(float mass)
	{
		m_mass = mass;
	}

	float PhysicsComponent::GetMass() const
	{
		return m_mass;
	}

}
