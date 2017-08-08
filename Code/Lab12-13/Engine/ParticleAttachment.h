#ifndef PARTICLEATTACHMENT_H
#define PARTICLEATTACHMENT_H

// Justin Furtado
// ParticleAttachment.h
// 8/7/2017
// Used for a physics lab

#include "ExportHeader.h"
#include "Vec3.h"

namespace Engine
{
	class Particle;
	class ENGINE_SHARED ParticleAttachment
	{
	public:
		void Attach(Particle *pParticle, Particle *pAttachTo);
		void Detach();
		void Update(float dt);
		Vec3 GetRotationalVelocity() const;
		bool IsAttached() const;

	private:
		Particle *m_pParticle{ nullptr };
		Particle *m_pAttachTo{ nullptr };
		Vec3 m_baseAttachment{ 0.0f, 0.0, 0.0f };
	};

}

#endif // ifndef PARTICLEATTACHMENT_H