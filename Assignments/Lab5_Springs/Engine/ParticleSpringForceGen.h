#ifndef PARTICLESPRINGFORCEGEN_H
#define PARTICLESPRINGFORCEGEN_H

// Justin Furtado
// 7/23/2017
// ParticleSpringForceGen.h
// Does particle spring stuff

#include "ExportHeader.h"
#include "ParticleForceGenerator.h"

namespace Engine
{
	class ENGINE_SHARED ParticleSpringForceGen : public ParticleForceGenerator
	{
	public:
		void Init(Vec3 *pAnch, float springConst, float restLen);
		float GetSpringConst() const;
		float GetRestLen() const;
		void SetSpringConst(float springConst);
		void SetRestLen(float restLen);
		void UpdateForce(Particle *pParticle, float dt) override;

	private:
		Vec3 *m_pAnchor;
		float m_sprintConstant;
		float m_restingLength;
	};
}


#endif // ifndef PARTICLESPRINGFORCEGEN_H