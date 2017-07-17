#include "PhysicsComponent.h"
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

		m_particle = Particle(m_pSpatialComp->GetPosition(), m_pSpatialComp->GetVelocity(), 0.9995f, Engine::MathUtility::PI * 4.0f / 3.0f * m_radius * m_radius * m_radius);

		// todo: log success
		return true;
	}

	bool PhysicsComponent::Update(float dt)
	{
		m_particle.Update(dt);
		m_pSpatialComp->SetPosition(m_particle.GetPosition());
		m_pSpatialComp->SetVelocity(Vec3(0.0f));
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

		float m1 = GetMass(), m2 = pOther->GetMass();
		Vec3 v1 = m_particle.GetVelocity(), v2 = pOther->m_particle.GetVelocity();
		m_tempVelStore = ((v1 * (m1 - m2) + (2 * m2*v2)) / (m1 + m2));
	}

	void PhysicsComponent::ResolveCollision()
	{
		// TODO REFACTOR!?!?!
		m_particle.SetPosition(m_tempPosStore);
		m_particle.SetVelocity(m_tempVelStore);
	}

	void PhysicsComponent::SetPosition(const Vec3 & pos)
	{
		m_particle.SetPosition(pos);
	}

	void PhysicsComponent::SetVelocity(const Vec3 & vel)
	{
		m_particle.SetVelocity(vel);
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

	Vec3 PhysicsComponent::GetVelocity() const
	{
		return m_particle.GetVelocity();
	}

	void PhysicsComponent::SetMass(float mass, bool conserveMomentum)
	{
		m_particle.SetMass(mass, conserveMomentum);
	}

	float PhysicsComponent::GetMass() const
	{
		return m_particle.GetMass();
	}

	void PhysicsComponent::AddForce(const Vec3 & force)
	{
		m_particle.AddForce(force);
	}

	Vec3 PhysicsComponent::GetMomentum() const
	{
		return m_particle.GetMomentum();
	}

	Particle * PhysicsComponent::GetParticlePtr()
	{
		return &m_particle;
	}

}
