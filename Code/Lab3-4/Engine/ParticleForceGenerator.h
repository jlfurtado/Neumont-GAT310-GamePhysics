#ifndef PARTICLEFORCEGENERATOR_H
#define PARTICLEFORCEGENERATOR_H

// Justin Furtado
// 7/16/2017
// ParticleForceGenerator.h
// Abstract base class for generating particle forces

#include "ExportHeader.h"
#include "Particle.h"

namespace Engine 
{
	class ENGINE_SHARED ParticleForceGenerator
	{
	public:
		virtual void UpdateForce(Particle *pParticle, float dt) = 0;
	};
}


#endif // ifndef PARTICLEFORCEGENERATOR_H