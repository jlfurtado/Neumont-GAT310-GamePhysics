#include "ParticleAttachment.h"
#include "Particle.h"
#include "MathUtility.h"

// Justin Furtado
// ParticleAttachment.cpp
// 8/7/2017
// Used for a physics lab

namespace Engine
{
	void ParticleAttachment::Attach(Particle * pParticle, Particle * pAttachTo)
	{
		m_pAttachTo = pAttachTo;
		m_pParticle = pParticle;

		// TODO OTHER ON ATTACH STUFF HERE!?!?!
		m_baseAttachment = m_pParticle->GetPosition() - m_pAttachTo->GetPosition();
	}

	void ParticleAttachment::Detach()
	{
		m_pParticle->SetVelocity(GetRotationalVelocity());
	}

	void ParticleAttachment::Update(float /*dt*/)
	{
		m_pParticle->SetPosition(m_pAttachTo->GetPosition() + (Engine::Mat4::RotationAroundAxis(m_pAttachTo->GetRotationAxis(), m_pAttachTo->GetRadiansRotated()) * m_baseAttachment));
	}

	Vec3 ParticleAttachment::GetRotationalVelocity() const
	{
		return ((m_pParticle->GetPosition() - m_pAttachTo->GetPosition()).Cross(m_pAttachTo->GetRotationAxis())) * (m_pAttachTo->GetRotationRate());
	}
}
