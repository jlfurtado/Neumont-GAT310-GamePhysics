#include "Particle.h"

// Justin Furtado
// 7/10/2017
// Particle.cpp
// Contains Velocity, Acceleration, Dampening, and Mass for an object

namespace Engine
{
	Particle::Particle()
		: m_position(Vec3(0.0f)), m_velocity(Vec3(0.0f)), m_acceleration(Vec3(0.0f)), m_dampening(0.0f), m_inverseMass(0.0f)
	{
	}

	Particle::Particle(const Vec3 & startPos, const Vec3 & startVel, float dampening, float mass)
		: m_position(startPos), m_velocity(startVel), m_acceleration(Vec3(0.0f)), m_dampening(dampening), m_inverseMass(mass <= 0.0f ? 0.0f : 1.0f / mass)
	{
	}


	Particle::~Particle()
	{
	}

	void Particle::Update(float dt)
	{
		if (dt > 0.0f && m_inverseMass > 0.0f)
		{
			m_position = m_position + m_velocity * dt;
			m_velocity = (m_velocity + (m_acceleration * dt)) * powf(m_dampening, dt);
			m_acceleration = Vec3(0.0f);
		}
	}

	void Particle::AddForce(const Vec3 & force)
	{
		m_acceleration = m_acceleration + (m_inverseMass * force);
	}

	void Particle::SetMass(float newMass, bool conserveMomentum)
	{
		if (newMass >= 0.0f)
		{
			// momentum is mass * velocity
			Vec3 momentum = (1.0f / m_inverseMass) * m_velocity;

			// set new mass
			m_inverseMass = 1.0f / newMass;

			// conserve momentum, update the velocity
			// m1v1 = m2v2 -> v2 = (m1v1)/m2 -> v2 = (momentum)/m2
			if (conserveMomentum) { m_velocity = momentum * m_inverseMass; }
		}
	}

	void Particle::SetVelocity(const Vec3 & vel)
	{
		m_velocity = vel;
	}

	void Particle::SetPosition(const Vec3 & position)
	{
		m_position = position;
	}

	float Particle::GetMass() const
	{
		return (1.0f / m_inverseMass);
	}

	Vec3 Particle::GetMomentum() const
	{
		return m_velocity * (1.0f / m_inverseMass);
	}

	Vec3 Particle::GetPosition() const
	{
		return m_position;
	}

	Vec3 Particle::GetVelocity() const
	{
		return m_velocity;
	}
}

