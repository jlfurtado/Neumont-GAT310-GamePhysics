#ifndef PARTICLE_H
#define PARTICLE_H

// Justin Furtado
// 7/10/2017
// Particle.h
// Contains Velocity, Acceleration, Dampening, and Mass for an object

#include "ExportHeader.h"
#include "Vec3.h"

namespace Engine
{
	class ENGINE_SHARED Particle
	{
	public:
		Particle();
		Particle(const Vec3& startPos, const Vec3& startVel, float dampening, float mass);
		~Particle();

		void Update(float dt);
		void AddForce(const Vec3& force);
		void SetMass(float newMass, bool conserveMomentum);
		void SetVelocity(const Vec3& vel);
		void SetPosition(const Vec3& position);
		float GetMass() const;
		Vec3 GetMomentum() const;
		Vec3 GetPosition() const;
		Vec3 GetVelocity() const;
		bool HasFiniteMass() const;
		void ClearForces();
		void Rotate(float rotationsPerSecond, const Vec3& axis);
		float GetRadiansRotated() const;
		Vec3 GetRotationAxis() const;
		bool IsRotating() const;
		float GetRotationRate() const;
		void ResetRotation();

	private:
		Vec3 m_position;
		Vec3 m_velocity;
		Vec3 m_acceleration;
		Vec3 m_netForce;
		Vec3 m_lastAcceleration;
		Vec3 m_rotationAxis;
		float m_dampening;
		float m_inverseMass;
		float m_radiansRotated;
		float m_radiansPerSecond;
	};

}

#endif // ifndef PARTICLE_H