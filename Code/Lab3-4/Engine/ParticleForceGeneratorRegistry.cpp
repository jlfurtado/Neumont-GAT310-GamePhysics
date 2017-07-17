#include "ParticleForceGeneratorRegistry.h"
#include "Particle.h"
#include "ParticleForceGenerator.h"

// Justin Furtado
// 7/16/2017
// ParticleForceGeneratorRegistry.cpp
// Management for connecting particles and forces

namespace Engine
{
	void ParticleForceGeneratorRegistry::Add(Particle * pParticle, ParticleForceGenerator * pForceGen)
	{
		m_registrations.Add(ParticleForceRegistration(pParticle, pForceGen));
	}

	void ParticleForceGeneratorRegistry::Remove(Particle * pParticle, ParticleForceGenerator * pForceGen)
	{
		m_registrations.RemoveFirst(ParticleForceRegistration(pParticle, pForceGen));
	}

	void ParticleForceGeneratorRegistry::Clear()
	{
		m_registrations.Clear(ParticleForceRegistration()); // TODO: potential optimization? Don't reset to nullptr for safety but avoid copy? shouldn't matter unless clearing often?
	}

	// update all the things
	void ParticleForceGeneratorRegistry::UpdateForces(float dt)
	{
		for (unsigned i = 0; i < m_registrations.GetCount(); ++i)
		{
			if (m_registrations[i].pForceGenerator->IsEnabled())
			{
				m_registrations[i].pForceGenerator->UpdateForce(m_registrations[i].pParticle, dt);
			}
		}
	}
}
