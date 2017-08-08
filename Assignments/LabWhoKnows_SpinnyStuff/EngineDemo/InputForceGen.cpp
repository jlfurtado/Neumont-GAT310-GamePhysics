#include "InputForceGen.h"
#include "Keyboard.h"
#include "MyWindow.h"

// Justin Furtado
// 7/16/2017
// InputForceGen.h
// move using input and force

const float STRENGTH = 100.0f;
void InputForceGen::UpdateForce(Engine::Particle * pParticle, float /*dt*/)
{
	if (Engine::Keyboard::KeyIsDown(VK_UP)) { pParticle->AddForce(Engine::Vec3(0.0f, 1.0f, 0.0f) * STRENGTH); }
	if (Engine::Keyboard::KeyIsDown(VK_DOWN)) { pParticle->AddForce(Engine::Vec3(0.0f, -1.0f, 0.0f) * STRENGTH); }
	if (Engine::Keyboard::KeyIsDown(VK_LEFT)) { pParticle->AddForce(Engine::Vec3(1.0f, 0.0f, 0.0f) * STRENGTH); }
	if (Engine::Keyboard::KeyIsDown(VK_RIGHT)) { pParticle->AddForce(Engine::Vec3(-1.0f, 0.0f, 0.0f) * STRENGTH); }
}
