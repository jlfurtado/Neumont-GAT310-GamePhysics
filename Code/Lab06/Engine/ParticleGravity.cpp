#include "ParticleGravity.h"


// Justin Furtado
// 7/16/2017
// ParticleGravity.cpp
// Implements force generator 

namespace Engine
{
	void ParticleGravity::SetGravity(const Vec3 & newGravity)
	{
		m_gravity = newGravity;
	}
	
	Vec3 ParticleGravity::GetGravity() const
	{
		return m_gravity;
	}

	void ParticleGravity::UpdateForce(Particle * pParticle, float /*dt*/)
	{
		if (pParticle->HasFiniteMass())
		{
			pParticle->AddForce(m_gravity * pParticle->GetMass()); 
		}
	}
}
