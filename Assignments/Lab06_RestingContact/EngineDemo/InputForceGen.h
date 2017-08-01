#ifndef INPUTFORCEGEN_H
#define INPUTFORCEGEN_H

// Justin Furtado
// 7/16/2017
// InputForceGen.h
// move using input and force

#include "ParticleForceGenerator.h"

class InputForceGen : public Engine::ParticleForceGenerator
{
public:
	void UpdateForce(Engine::Particle *pParticle, float dt) override;
};

#endif // ifndef INPUTFORCEGEN_H