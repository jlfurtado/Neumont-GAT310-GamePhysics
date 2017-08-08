#ifndef PARTICLEDRAG_H
#define PARTICLEDRAG_H

// Justin Furtado
// 7/17/2017
// ParticleDrag.h
// Controls drag for particles

#include "ExportHeader.h"
#include "ParticleForceGenerator.h"

namespace Engine
{
	class ENGINE_SHARED ParticleDrag : public ParticleForceGenerator
	{
	public:
		void SetCoefficients(float k1, float k2);
		void SetK1(float k1);
		void SetK2(float k2);
		float GetK1() const;
		float GetK2() const;
		void UpdateForce(Particle *pParticle, float dt) override;

	private:
		float k1{ 1.0f }, k2{ 1.0f };
	};

}

#endif // ifndef PARTICLEDRAG_H