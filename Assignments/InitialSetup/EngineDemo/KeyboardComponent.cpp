#include "KeyboardComponent.h"
#include "SpatialComponent.h"
#include "ChaseCameraComponent.h"
#include "GraphicalObjectComponent.h"
#include "GraphicalObject.h"

// Justin Furtado
// 8/16/2016
// KeyboardComponent.h
// Detects key presses and calls the proper component methods

KeyboardComponent::KeyboardComponent()
{
}


KeyboardComponent::~KeyboardComponent()
{
}

bool KeyboardComponent::Initialize()
{
	Engine::GameLogger::Log(Engine::MessageType::Process, "KeyboardComponent [%s] initialized successfully!\n", GetName());
	return true;
}

bool KeyboardComponent::Update(float dt)
{
	return HandleKeyboardInput(dt);
}

bool KeyboardComponent::HandleKeyboardInput(float dt)
{
	// Requires access to transform info to modify stuff
	Engine::SpatialComponent *pSpatialComponent = GetSiblingComponent<Engine::SpatialComponent>();
	if (!pSpatialComponent) { return true; }

	// Requires access to camera to rotate it
	Engine::ChaseCameraComponent *pCameraComponent = GetSiblingComponent<Engine::ChaseCameraComponent>();
	if (!pCameraComponent) { return true; }

	// Requires access to graphical object to rotate it
	Engine::GraphicalObjectComponent *pGraphicalObjectComponent = GetSiblingComponent<Engine::GraphicalObjectComponent>();
	if (!pGraphicalObjectComponent) { return true; }

	// get vectors
	Engine::Vec3 f = pSpatialComponent->GetForward();
	Engine::Vec3 u = pSpatialComponent->GetUp();
	Engine::Vec3 r = pSpatialComponent->GetRight();

	// radians to rotate
	float radians = pCameraComponent->GetRotateSpeed() * dt;

	Engine::Mat4 rot;

	// handle dargon input
	if (Engine::Keyboard::KeyIsDown('W')) { rot = Engine::Mat4::RotationAroundAxis(r, -radians) * rot; }
	if (Engine::Keyboard::KeyIsDown('S')) { rot = Engine::Mat4::RotationAroundAxis(r, radians) * rot; }
	if (Engine::Keyboard::KeyIsDown('A')) { rot = Engine::Mat4::RotationAroundAxis(u, radians) * rot; }
	if (Engine::Keyboard::KeyIsDown('D')) { rot = Engine::Mat4::RotationAroundAxis(u, -radians) * rot; }
	if (Engine::Keyboard::KeyIsDown('Q')) { rot = Engine::Mat4::RotationAroundAxis(f, -radians) * rot; }
	if (Engine::Keyboard::KeyIsDown('E')) { rot = Engine::Mat4::RotationAroundAxis(f, radians) * rot; }

	//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "---(%.3f, %.3f, %.3f)\n", f.GetX(), f.GetY(), f.GetZ());

	// update vectors
	pSpatialComponent->SetAxes(rot * f, rot * u);

	// calculate matrix and sqqqqet it on gob so its drawn
	Engine::Mat4 calcdRot = pSpatialComponent->CalcRotationMatrix();
	pGraphicalObjectComponent->GetGraphicalObject()->SetRotMat(calcdRot);

	// update camera
	pCameraComponent->Move(pSpatialComponent->GetPosition(), Engine::Vec3(0.0f));

	Engine::Vec3 vel = (Engine::Keyboard::KeyIsDown(' ') ? pCameraComponent->GetSpeed()*(calcdRot*(Engine::Vec3(0.0f, 0.0f, -1.0f))) : Engine::Vec3(0.0f, 0.0f, 0.0f));
	Engine::Vec3 move = vel * dt;

	//const float MIN_MOVE = 0.01f;
	//int count = 0;
	//float m1 = 20.0f, m2 = 20.0f;
	//bool hit = false;
	//Engine::RayCastingOutput moveRCO = Engine::CollisionTester::FindWall(pSpatialComponent->GetPosition() - (m1 * move.Normalize()), move.Normalize(), m1 + m2 + 1.0f, Engine::CollisionLayer::NUM_LAYERS);
	//while (moveRCO.m_didIntersect && moveRCO.m_distance < (m1 + m2) && move.LengthSquared() > MIN_MOVE)
	//{
	//	Engine::Vec3 from = pSpatialComponent->GetPosition() - (m1 * move.Normalize());
	//	Engine::Vec3 dir = move.Normalize();
	//	float len = moveRCO.m_distance;
	//	Engine::Vec3 hitPos = from + dir * len;

	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Count: %d\n", ++count);
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "From: (%.3f, %.3f, %.3f)\n", from.GetX(), from.GetY(), from.GetZ());
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Dir: (%.3f, %.3f, %.3f)\n", dir.GetX(), dir.GetY(), dir.GetZ());
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Hit: (%.3f, %.3f, %.3f)\n", hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Len: %.3f\n", moveRCO.m_distance);
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Move Before: (%.3f, %.3f, %.3f)\n", move.GetX(), move.GetY(), move.GetZ());
	//	move = move - move.ProjectOnto(moveRCO.m_triangleNormal);
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "Move After: (%.3f, %.3f, %.3f)\n", move.GetX(), move.GetY(), move.GetZ());

	//	hit = true;
	//	moveRCO = Engine::CollisionTester::FindWall(pSpatialComponent->GetPosition() - (m1 * move.Normalize()), move.Normalize(), m1 + m2 + 1.0f, Engine::CollisionLayer::NUM_LAYERS);
	//}

	//if (!hit)
	//{
	//	pSpatialComponent->SetVelocity(vel);
	//}
	//else if (move.LengthSquared() > MIN_MOVE) // snap
	//{
	//	pSpatialComponent->SetPosition(pSpatialComponent->GetPosition() + move);
	//	pSpatialComponent->SetVelocity(Engine::Vec3(0.0f));
	//}

	pSpatialComponent->SetVelocity(vel);
	return true;
}
