#include "ParticleDrag.h"


// Justin Furtado
// 7/17/2017
// ParticleDrag.cpp
// Controls drag for particles

namespace Engine
{
	void ParticleDrag::SetCoefficients(float newk1, float newk2)
	{
		k1 = newk1;
		k2 = newk2;
	}

	void ParticleDrag::SetK1(float newk1)
	{
		k1 = newk1;
	}

	void ParticleDrag::SetK2(float newk2)
	{
		k2 = newk2;
	}

	float ParticleDrag::GetK1() const
	{
		return k1;
	}

	float ParticleDrag::GetK2() const
	{
		return k2;
	}

	void ParticleDrag::UpdateForce(Particle * pParticle, float /*dt*/)
	{
		Vec3 vel = pParticle->GetVelocity();
		
		float len = vel.Length();
		float dragCoeff = k1 * len + k2 * len * len;

		pParticle->AddForce(vel.Normalize() * -dragCoeff);
	}
}
