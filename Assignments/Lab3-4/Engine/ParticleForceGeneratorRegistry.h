#ifndef PARTICLEFORCEGENERATORREGISTRY_H
#define PARTICLEFORCEGENERATORREGISTRY_H

// Justin Furtado
// 7/16/2017
// ParticleForceGeneratorRegistry.h
// Management for connecting particles and forces

#include "ExportHeader.h"
#include "DynamicArray.h"

namespace Engine
{
	class Particle; // pre-declare for pointer only needed
	class ParticleForceGenerator;
	class ENGINE_SHARED ParticleForceGeneratorRegistry
	{
		struct ParticleForceRegistration
		{
			ParticleForceRegistration() : pParticle(nullptr), pForceGenerator(nullptr) {}
			ParticleForceRegistration(Particle *pParticle, ParticleForceGenerator *pForceGen) : pParticle(pParticle), pForceGenerator(pForceGen) {}

			Particle *pParticle;
			ParticleForceGenerator *pForceGenerator;

			bool ParticleForceRegistration::operator==(const ParticleForceRegistration &other) const {
				return this->pParticle == other.pParticle && this->pForceGenerator == other.pForceGenerator;
			}

		};

	public:
		void Add(Particle *pParticle, ParticleForceGenerator *pForceGen);
		void Remove(Particle *pParticle, ParticleForceGenerator *pForceGen);
		void Clear();
		void UpdateForces(float dt);

	private:
		DynamicArray<ParticleForceRegistration> m_registrations;
	};
}


#endif // ifndef PARTICLEFORCEGENERATORREGISTRY_H