#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

// Justin Furtado
// 6/27/2017
// PhysicsManager.h
// Manages physics!

#include "ExportHeader.h"
#include "DynamicArray.h"
#include "ParticleForceGeneratorRegistry.h"

namespace Engine
{
	class PhysicsComponent;
	class ENGINE_SHARED PhysicsManager
	{
	public:
		static bool Initialize();
		static bool Shutdown();
		static void Update();
		static void UpdateForceGens(float dt);
		static void RegisterComponent(PhysicsComponent *pCompToAdd);
		static void UnregisterComponent(PhysicsComponent *pCompToRemove);
		static void AddForcGen(Particle *pParticle, ParticleForceGenerator *pForceGen);
		static void RemoveforceGen(Particle *pParticle, ParticleForceGenerator *pForceGen);
		static void ClearForceGens();

	private:
		static ParticleForceGeneratorRegistry s_registry;
		static DynamicArray<PhysicsComponent *> s_components;
	};
}

#endif // ifndef PHYSICSMANAGER_H