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
		if (!IsAttached()) { return; }
		m_pParticle->SetVelocity(GetRotationalVelocity()); 
		m_pAttachTo = nullptr;
		m_pParticle = nullptr;
	}

	void ParticleAttachment::Update(float /*dt*/)
	{
		if (!IsAttached()) { return; }
		m_pParticle->SetPosition(m_pAttachTo->GetPosition() + (Engine::Mat4::RotationAroundAxis(m_pAttachTo->GetRotationAxis(), m_pAttachTo->GetRadiansRotated()) * m_baseAttachment));
	}

	Vec3 ParticleAttachment::GetRotationalVelocity() const
	{
		if (!IsAttached()) { return Vec3::ZERO; }
		return ((m_pAttachTo->GetPosition() - m_pParticle->GetPosition()).Cross(m_pAttachTo->GetRotationAxis())) * (m_pAttachTo->GetRotationRate());
	}

	bool ParticleAttachment::IsAttached() const
	{
		return m_pAttachTo != nullptr && m_pParticle != nullptr;
	}
}
