#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

// Justin Furtado
// 6/27/2017
// PhysicsManager.h
// Manages physics!

#include "ExportHeader.h"
#include "DynamicArray.h"

namespace Engine
{
	class PhysicsComponent;
	class ENGINE_SHARED PhysicsManager
	{
	public:
		static bool Initialize();
		static bool Shutdown();
		static void Update();
		static void RegisterComponent(PhysicsComponent *pCompToAdd);
		static void UnregisterComponent(PhysicsComponent *pCompToRemove);

	private:
		static DynamicArray<PhysicsComponent *> s_components;
	};
}

#endif // ifndef PHYSICSMANAGER_H