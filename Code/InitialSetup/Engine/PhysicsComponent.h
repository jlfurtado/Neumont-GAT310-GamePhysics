#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

// Justin Furtado
// 6/27/2017
// PhysicsComponent.h
// A comonent for entities to have physics

#include "ExportHeader.h"
#include "Component.h"

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

	private:
		GraphicalObjectComponent *m_pGobComp{ nullptr };
		SpatialComponent *m_pSpatialComp{ nullptr };
	};
}


#endif // ifndef PHYSICSCOMPONENT_H