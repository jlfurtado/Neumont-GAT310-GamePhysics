#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

// Justin Furtado
// 6/27/2017
// PhysicsComponent.h
// A comonent for entities to have physics

#include "ExportHeader.h"
#include "Component.h"
#include "Vec3.h"

namespace Engine
{
	class GraphicalObjectComponent;
	class SpatialComponent;
	class ENGINE_SHARED PhysicsComponent : public Component
	{
	public:
		PhysicsComponent();
		~PhysicsComponent();

		bool Initialize() override;
		bool PhysicsUpdate() override;
		void CalcCollisionInfo(PhysicsComponent *pOther);
		void ResolveCollision();
		void SetRadius(float radius);
		float GetRadius() const;
		Vec3 GetPosition() const;
		void SetMass(float mass);
		float GetMass() const;

	protected:
		GraphicalObjectComponent *m_pGobComp{ nullptr };
		SpatialComponent *m_pSpatialComp{ nullptr };
		Vec3 m_tempPosStore;
		Vec3 m_tempVelStore;
		float m_radius{ 1.0f };
		float m_mass{ 1.0f };
	};
}


#endif // ifndef PHYSICSCOMPONENT_H