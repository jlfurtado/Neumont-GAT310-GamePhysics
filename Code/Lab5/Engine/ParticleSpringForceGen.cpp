#include "ParticleSpringForceGen.h"

// Justin Furtado
// 7/23/2017
// ParticleSpringForceGen.h
// Does particle spring stuff

namespace Engine
{
	void ParticleSpringForceGen::Init(Vec3 * pAnch, float springConst, float restLen)
	{
		m_pAnchor = pAnch;
		m_sprintConstant = springConst;
		m_restingLength = restLen;
	}

	float ParticleSpringForceGen::GetSpringConst() const
	{
		return m_sprintConstant;
	}

	float ParticleSpringForceGen::GetRestLen() const
	{
		return m_restingLength;
	}

	void ParticleSpringForceGen::SetSpringConst(float springConst)
	{
		m_sprintConstant = springConst;
	}

	void ParticleSpringForceGen::SetRestLen(float restLen)
	{
		m_restingLength = restLen;
	}

	void ParticleSpringForceGen::UpdateForce(Particle * pParticle, float /*dt*/)
	{
		Vec3 toParticle = pParticle->GetPosition() - *m_pAnchor;
		pParticle->AddForce(((m_restingLength - toParticle.Length()) * m_sprintConstant) * toParticle.Normalize());
	}


}