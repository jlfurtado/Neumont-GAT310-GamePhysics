#ifndef PARTICLEGRAVITY_H
#define PARTICLEGRAVITY_H

// Justin Furtado
// 7/16/2017
// ParticleGravity.h
// Implements force generator 

#include "ExportHeader.h"
#include "ParticleForceGenerator.h"

namespace Engine
{
	class ENGINE_SHARED ParticleGravity : public ParticleForceGenerator
	{
	public:
		void SetGravity(const Vec3& newGravity);
		Vec3 GetGravity() const;
		void UpdateForce(Particle *pParticle, float dt) override;

	private:
		Vec3 m_gravity;
	};

}

#endif // ifnder PARTICLEGRAVITY_H