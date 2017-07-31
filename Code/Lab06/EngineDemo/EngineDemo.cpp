#include "EngineDemo.h"
#include "AStarPathFinder.h"
#include "MousePicker.h"
#include "MyWindow.h"
#include <iostream>
#include "WorldFileIO.h"

#include "InstanceBuffer.h"
#include "StringFuncs.h"
#include "MouseManager.h"
#include "GameLogger.h"
#include "MyGL.h"
#include "CollisionTester.h"
#include "MyFiles.h"
#include "ShaderProgram.h"
#include "BitmapLoader.h"
#include "SoundEngine.h"
#include "SoundObject.h"
#include "ParticleGravity.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4127)
#include <QtGui\qmouseevent>
#pragma warning(pop)

#include "ShapeGenerator.h"
#include "AStarPathFollowComponent.h"
#include "RenderEngine.h"
#include "ConfigReader.h"
#include "MathUtility.h"

#include "UniformData.h"
#include "FrameBuffer.h"
#include <winuser.h>
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

// Justin Furtado
// 6/21/2016
// EngineDemo.cpp
// The game

const float MAGIC_RADIUS_SCALE = 1.0f; // todo: collider visualization? update const if different model???
const float FOUR_THIRDS_PI = Engine::MathUtility::PI * 4.0f / 3.0f;
const float MASS_PER_VOLUME = 0.0001f;
const float EngineDemo::LIGHT_HEIGHT = 15.0f;
const float EngineDemo::RENDER_DISTANCE = 2000.0f;
Engine::Mat4 EngineDemo::identity;
Engine::Vec3 EngineDemo::zero;
const Engine::Vec3 BASE_ARROW_DIR = Engine::Vec3(1.0f, 0.0f, 0.0f);
const Engine::Vec3 ORIGIN = Engine::Vec3(0.0f, 0.0f, 250.0f);
const float MIN_DRAG = 0.0f;
const float MAX_DRAG = 1.0f;

namespace {
	const int TYPES = 3;
	const int MAX_OBJS = 6;
	Engine::Entity m_objs[MAX_OBJS];
	Engine::SpatialComponent m_objSpatials[MAX_OBJS];
	Engine::GraphicalObjectComponent m_objGobsComps[MAX_OBJS];
	Engine::GraphicalObject m_objGobs[MAX_OBJS];
	Engine::Mat4 m_instanceMatrices[MAX_OBJS];
	Engine::Mat4 m_velocityArrowMatrices[MAX_OBJS];
	Engine::Mat4 m_momentumArrowMatrices[MAX_OBJS];
	Engine::PhysicsComponent m_objPhysics[MAX_OBJS];
	Engine::GraphicalObject m_instanceGob[TYPES];
	Engine::InstanceBuffer m_instanceBuffer[TYPES];
	Engine::Vec3 posOther;

	Engine::ParticleGravity m_gravity;
	Engine::GraphicalObject m_plane;


	//Engine::GraphicalObject m_planetGob;
	//Engine::GraphicalObject m_moonGob;
	//Engine::Entity m_planet;
	//Engine::Entity m_moon;
	//Engine::SpatialComponent m_planetSpatial;
	//Engine::SpatialComponent m_moonSpatial;
	//Engine::GraphicalObjectComponent m_planetGobComp;
	//Engine::GraphicalObjectComponent m_moonGobComp;
	//Engine::PhysicsComponent m_planetPhysics;
	//Engine::PhysicsComponent m_moonPhysics;
	//Engine::ParticleGravity m_particleGravity;
}

bool EngineDemo::Initialize(Engine::MyWindow *window)
{
	m_pWindow = window;

	if (!InitializeGL())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize game! Failed to InitializeGL()!\n");
		return false;
	}

	if (!Engine::RenderEngine::Initialize(&m_shaderPrograms[0], NUM_SHADER_PROGRAMS))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Could not do anything because RenderEngine failed...\n");
		return false;
	}

	if (!Engine::ShapeGenerator::Initialize(m_shaderPrograms[0].GetProgramId(), m_shaderPrograms[1].GetProgramId(), m_shaderPrograms[2].GetProgramId()))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to initialize game because shape generator failed to initialize!\n");
		return false;
	}

	if (!UglyDemoCode()) { return false; }

	if (!ReadConfigValues())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to read config values!\n");
		return false;
	}

	if (!Engine::ConfigReader::pReader->RegisterCallbackForConfigChanges(EngineDemo::OnConfigReload, this))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to register callback for EngineDemo!\n");
		return false;
	}

	if (!Engine::Keyboard::AddToggle('P', &paused, true))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to add key toggle!\n");
		return false;
	}

	if (!Engine::PhysicsManager::Initialize()) 
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to init physics manager!\n");
		return false;
	}

	//if (!SoundEngine::Initialize())
	//{
	//	Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to initialize SoundEngine");
	//	return false;
	//}
	//backgroundMusic.Initialize("..\\Data\\Sounds\\ElectricalAmbiance.wav");

	toggleDrag = true;
	storeK1 = m_particleDrag.GetK1();
	storeK2 = m_particleDrag.GetK2();

	Engine::CollisionTester::CalculateGrid(Engine::CollisionLayer::NUM_LAYERS);

	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Initialized Successfully!!!\n");
	return true;
}

bool EngineDemo::Shutdown()
{
	// Display some info on shutdown
	Engine::RenderEngine::LogStats();

	if (!m_pWindow->Shutdown()) { return false; }

	for (int i = 0; i < NUM_SHADER_PROGRAMS; ++i)
	{
		if (!m_shaderPrograms[i].Shutdown()) { return false; }
	}

	// have to clear objs before shape gen, cuz shape gen deletes mesh before render engine can remove it
	m_fromWorldEditorOBJs.WalkList(DestroyObjsCallback, this);
	if (m_objCount != 0) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to DestroyObjs! Check for memory leak or counter inaccuracy [%d] objs left!\n", m_objCount); return false; }

	if (!Engine::PhysicsManager::Shutdown()) { return false; }
	if (!Engine::TextObject::Shutdown()) { return false; }
	if (!Engine::RenderEngine::Shutdown()) { return false; }
	if (!Engine::ShapeGenerator::Shutdown()) { return false; }
	
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Shutdown Successfully!!!\n");
	return true;
}

bool EngineDemo::InitializeCallback(void * game, Engine::MyWindow * window)
{
	if (!game) { return false; }
	return reinterpret_cast<EngineDemo *>(game)->Initialize(window);
}

void EngineDemo::UpdateCallback(void * game, float dt)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Update(dt);
}

void EngineDemo::ResizeCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnResizeWindow();
}

void EngineDemo::DrawCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Draw();
}

void EngineDemo::MouseScrollCallback(void * game, int degrees)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseScroll(degrees);
}

void EngineDemo::MouseMoveCallback(void * game, int dx, int dy)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseMove(dx, dy);
}

void EngineDemo::PhysicsCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->DoPhysics();
}

void EngineDemo::DoPhysics()
{
	Engine::PhysicsManager::Update();
}

void EngineDemo::Update(float dt)
{
	const float maxBorder = 0.1f;
	const float minBorder = -0.1f;

	Engine::Keyboard::Update(dt); // needs to come before to un-pause
	Engine::MousePicker::SetCameraInfo(Engine::Vec3(0.0f), Engine::Vec3(0.0f, 0.0f, 1.0f), Engine::Vec3(0.0f, 1.0f, 0.0f));

	if (!won) { ShowFrameRate(dt); }
	if (!ProcessInput(dt)) { return; }
	if (paused) { return; }

	bool select = false;
	if (Engine::MouseManager::IsLeftMouseClicked())
	{
		if (m_sphereIndex != -1) 
		{
			Engine::PhysicsManager::RemoveforceGen(m_objPhysics[m_sphereIndex].GetParticlePtr(), &m_inputForceGen); 
			m_sphereIndex = -1;
		}
		else
		{
			Engine::Vec3 origin = Engine::MousePicker::GetOrigin(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY());
			Engine::Vec3 direction = Engine::MousePicker::GetDirection(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY());
			for (int i = 0; i < MAX_OBJS - 1; ++i)
			{
				if (Engine::CollisionTester::RaySphereIntersect(origin, direction, m_objSpatials[i].GetPosition(), m_objPhysics[i].GetRadius()))
				{
					m_sphereIndex = i;
					Engine::PhysicsManager::AddForcGen(m_objPhysics[m_sphereIndex].GetParticlePtr(), &m_inputForceGen);
					select = true;
					break;
				}
			}

			if (!select && m_sphereIndex >= 0)
			{
				Engine::PhysicsManager::RemoveforceGen(m_objPhysics[m_sphereIndex].GetParticlePtr(), &m_inputForceGen);
				m_sphereIndex = -1;
			}
		}
	}

	m_selectedSphere.SetEnabled(m_sphereIndex >= 0);

	if (m_sphereIndex >= 0)
	{
		//Engine::RayCastingOutput rco = Engine::CollisionTester::FindFromMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY(), RENDER_DISTANCE, Engine::CollisionLayer::NUM_LAYERS);
		//if (rco.m_didIntersect) { m_objPhysics[m_sphereIndex].SetPosition(rco.m_intersectionPoint); }

		m_selectedSphere.SetTransMat(Engine::Mat4::Translation(m_objPhysics[m_sphereIndex].GetPosition()));
		m_selectedSphere.SetScaleMat(Engine::Mat4::Scale(m_objPhysics[m_sphereIndex].GetRadius()));
		m_selectedSphere.CalcFullTransform();

	}

	for (int i = 0; i < MAX_OBJS; ++i)
	{
		m_objs[i].Update(dt);
		m_objGobs[i].CalcFullTransform();
		m_instanceMatrices[i] = *m_objGobs[i].GetFullTransformPtr();
		Engine::Vec3 v = m_objPhysics[i].GetVelocity();
		Engine::Vec3 m = m_objPhysics[i].GetMomentum();
		float len = v.Length();
		float momentumLen = m.Length();
		Engine::Mat4 rot = Engine::Mat4::RotationToFace(BASE_ARROW_DIR, v);
		Engine::Mat4 trans = Engine::Mat4::Translation(rot * (BASE_ARROW_DIR * (len * 2.5f + 0.5f)) + m_objPhysics[i].GetPosition());
		Engine::Mat4 scale = Engine::Mat4::Scale(len, BASE_ARROW_DIR) * Engine::Mat4::Scale(2.5f);
		m_velocityArrowMatrices[i] = (trans * (rot * scale));
		trans = Engine::Mat4::Translation(rot * (BASE_ARROW_DIR * (momentumLen * 2.5f + 0.5f)) + m_objPhysics[i].GetPosition());
		scale = Engine::Mat4::Scale(momentumLen, BASE_ARROW_DIR) * Engine::Mat4::Scale(2.5f);
		m_momentumArrowMatrices[i] = (trans * (rot * scale));
	}

	m_instanceBuffer[0].UpdateData(&m_instanceMatrices[0], 0, 16 * sizeof(float)*MAX_OBJS, MAX_OBJS);
	m_instanceBuffer[1].UpdateData(&m_velocityArrowMatrices[0], 0, 16 * sizeof(float)*MAX_OBJS, MAX_OBJS);
	m_instanceBuffer[2].UpdateData(&m_momentumArrowMatrices[0], 0, 16 * sizeof(float)*MAX_OBJS, MAX_OBJS);
	
	
	if (debug)
	{
		char buffer[256]{ '\0' };
		char buffer2[256]{ '\0' };

		if (toggleDrag)
		{
			Engine::Vec3 m = m_objPhysics[m_sphereIndex].GetMomentum();
			sprintf_s(&buffer2[0], 256, "TODO DEBUG INFO HERE");
		}
	
		sprintf_s(&buffer[0], 256, "Debug Menu:\n%s", toggleDrag ? &buffer2[0] : "Drag off!");
		m_debugText.SetupText(0.2f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, &buffer[0]);
	}
	else
	{
		char buffer[256]{ '\0' };
		sprintf_s(&buffer[0], 256, "Controls:\n%s", "Menus->C/D\nZ-Toggle Mode\nB-K, V-R\nShift-Dir");
		m_debugText.SetupText(0.2f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, &buffer[0]);
	}

	posOther = m_objPhysics[0].GetPosition();
	Engine::PhysicsManager::UpdateForceGens(dt);
}

void EngineDemo::Draw()
{
	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < TYPES; ++i)
	{
		m_instanceGob[i].SetEnabled(false);
	}

	Engine::RenderEngine::Draw();

	for (int i = 0; i < TYPES; ++i)
	{
		m_instanceGob[i].SetEnabled(true);
	}

	if (drawGrid) { Engine::CollisionTester::DrawGrid(Engine::CollisionLayer::STATIC_GEOMETRY, Engine::Vec3(0.0f)); }

	Engine::RenderEngine::DrawInstanced(&m_instanceGob[0], &m_instanceBuffer[0]);

	if (debug)
	{
		glDisable(GL_DEPTH_TEST);
		Engine::RenderEngine::DrawInstanced(&m_instanceGob[1], &m_instanceBuffer[1]);
		Engine::RenderEngine::DrawInstanced(&m_instanceGob[2], &m_instanceBuffer[2]);
	}


	m_fpsTextObject.RenderText(&m_shaderPrograms[1], debugColorLoc);
	m_debugText.RenderText(&m_shaderPrograms[1], debugColorLoc);
}

void EngineDemo::OnResizeWindow()
{
	if (m_pWindow == nullptr) { return; }
	float aspect = static_cast<float>(m_pWindow->width()) / m_pWindow->height();
	m_perspective.SetAspectRatio(aspect);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());
}

const float MIN_RADIUS = 5.0f;
const float MAX_RADIUS = 15.0f;
const float DELTA_RADIUS = 0.001f;
void EngineDemo::OnMouseScroll(int degrees)
{
	if (m_sphereIndex > 0 && m_sphereIndex < MAX_OBJS - 1)
	{
		float radius = m_objPhysics[m_sphereIndex].GetRadius();
		float newRadius = Engine::MathUtility::Clamp(radius + DELTA_RADIUS * degrees, MIN_RADIUS, MAX_RADIUS);
		m_objGobs[m_sphereIndex].SetScaleMat(Engine::Mat4::Scale(radius));
		m_objPhysics[m_sphereIndex].SetRadius(newRadius);
		m_objPhysics[m_sphereIndex].SetMass(MASS_PER_VOLUME * FOUR_THIRDS_PI * radius*radius*radius, m_conserveMomentum);
	}
}

void EngineDemo::OnMouseMove(int /*deltaX*/, int /*deltaY*/)
{
}

void EngineDemo::OnConfigReload(void * classInstance)
{
	// error checking
	if (!classInstance) { return; }

	// get pointer to instance
	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(classInstance);

	if (Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", pGame->m_fpsInterval))
	{
		if (pGame->m_fpsInterval < 0.5f) { pGame->m_fpsInterval = 0.5f; }
	}

	float value;
	Engine::Vec2 inV2;
	int inInt;
	Engine::Vec3 color;
	Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, pGame->spotlightAttenuations.GetAddress());
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FractalSeed", 2, inV2.GetAddress())) { pGame->fractalSeed = inV2; }
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSX", 3, color.GetAddress())) { pGame->fsx = color; }
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSY", 3, color.GetAddress())) { pGame->fsy = color; }
	if (Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.ShaderTest.RepeatScale", value)) { pGame->repeatScale = value; }
	if (Engine::ConfigReader::pReader->GetIntForKey("EngineDemo.ShaderTest.NumIterations", inInt)) { pGame->numIterations = inInt; }
}

bool EngineDemo::InitializeGL()
{
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());

	glClearColor(backgroundColor.GetX(), backgroundColor.GetY(), backgroundColor.GetZ(), 1.0f);

	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);


	glClearStencil(0);

	if (m_shaderPrograms[0].Initialize())
	{
		m_shaderPrograms[0].AddVertexShader("..\\Data\\Shaders\\PCInstanced.Vert.shader");
		m_shaderPrograms[0].AddFragmentShader("..\\Data\\Shaders\\PCInstanced.Frag.shader");
		m_shaderPrograms[0].LinkProgram();
		m_shaderPrograms[0].UseProgram();
	}

	if (m_shaderPrograms[1].Initialize())
	{
		m_shaderPrograms[1].AddVertexShader("..\\Data\\Shaders\\Debug.vert.shader");
		m_shaderPrograms[1].AddFragmentShader("..\\Data\\Shaders\\Debug.frag.shader");
		m_shaderPrograms[1].LinkProgram();
		m_shaderPrograms[1].UseProgram();
	}

	if (m_shaderPrograms[2].Initialize())
	{					 
		m_shaderPrograms[2].AddVertexShader("..\\Data\\Shaders\\CelPhong.vert.shader");
		m_shaderPrograms[2].AddFragmentShader("..\\Data\\Shaders\\CelPhong.frag.shader");
		m_shaderPrograms[2].LinkProgram();
		m_shaderPrograms[2].UseProgram();
	}

	if (m_shaderPrograms[3].Initialize())
	{					 
		m_shaderPrograms[3].AddVertexShader("..\\Data\\Shaders\\DebugInstanced.Vert.shader");
		m_shaderPrograms[3].AddFragmentShader("..\\Data\\Shaders\\DebugInstanced.Frag.shader");
		m_shaderPrograms[3].LinkProgram();
		m_shaderPrograms[3].UseProgram();
	}

	if (m_shaderPrograms[4].Initialize())
	{
		m_shaderPrograms[4].AddVertexShader("..\\Data\\Shaders\\CelPhongInstanced.vert.shader");
		m_shaderPrograms[4].AddFragmentShader("..\\Data\\Shaders\\CelPhongInstanced.frag.shader");
		m_shaderPrograms[4].LinkProgram();
		m_shaderPrograms[4].UseProgram();
	}

	debugColorLoc = m_shaderPrograms[1].GetUniformLocation("tint");
	tintColorLoc = m_shaderPrograms[2].GetUniformLocation("tint");
	tintIntensityLoc = m_shaderPrograms[1].GetUniformLocation("tintIntensity");
	ambientColorLoc = m_shaderPrograms[2].GetUniformLocation("ambientLightColor");
	ambientIntensityLoc = m_shaderPrograms[2].GetUniformLocation("ambientLightIntensity");
	diffuseColorLoc = m_shaderPrograms[2].GetUniformLocation("diffuseLightColor");
	diffuseIntensityLoc = m_shaderPrograms[2].GetUniformLocation("diffuseLightIntensity");
	specularColorLoc = m_shaderPrograms[2].GetUniformLocation("specularLightColor");
	specularIntensityLoc = m_shaderPrograms[2].GetUniformLocation("specularLightIntensity");
	specularPowerLoc = m_shaderPrograms[2].GetUniformLocation("specularPower");
	modelToWorldMatLoc = m_shaderPrograms[2].GetUniformLocation("modelToWorld");
	worldToViewMatLoc = m_shaderPrograms[2].GetUniformLocation("worldToView");
	perspectiveMatLoc = m_shaderPrograms[2].GetUniformLocation("projection");
	lightLoc = m_shaderPrograms[2].GetUniformLocation("lightPos_WorldSpace");
	cameraPosLoc = m_shaderPrograms[2].GetUniformLocation("cameraPosition_WorldSpace");

	if (Engine::MyGL::TestForError(Engine::MessageType::cFatal_Error, "InitializeGL errors!"))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to InitializeGL()! TestForErrors found gl errors!\n");
		return false;
	}
	
	Engine::GameLogger::Log(Engine::MessageType::Process, "EngineDemo::InitializeGL() succeeded!\n");
	return true;
}

bool EngineDemo::ReadConfigValues()
{
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", m_fpsInterval)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FpsInterval!\n"); return false; }
	if (m_fpsInterval < 0.5f) { m_fpsInterval = 0.5f; }

	float cameraSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraSpeed", cameraSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraSpeed!\n"); return false; }
	if (cameraSpeed < 0.0f) { cameraSpeed = 1.0f; }

	float cameraRotationSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraRotationSpeed", cameraRotationSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraRotationSpeed!\n"); return false; }
	if (cameraRotationSpeed < 0.0f) { cameraRotationSpeed = 1.0f; }

	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, spotlightAttenuations.GetAddress()))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get floats for key AttenuationExponent!\n"); return false;
	}
	
	if(!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FractalSeed", 2, fractalSeed.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FractalSeed!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSX", 3, fsx.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FSX!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSY", 3, fsy.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FSY!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.ShaderTest.RepeatScale", repeatScale)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key RepeatScale!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetIntForKey("EngineDemo.ShaderTest.NumIterations", numIterations)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get int for key NumIterations!\n"); return false; }

	Engine::GameLogger::Log(Engine::MessageType::Process, "Successfully read in config values!\n");
	return true;
}

const float MULTIPLIER = 250.0f;
const float MIN_SPEED = 250.0f / MULTIPLIER;
const float MAX_SPEED = 250.0f * MULTIPLIER;
const float MIN_ROTATION_SPEED = 0.8f / MULTIPLIER;
const float MAX_ROTATION_SPEED = 0.8f * MULTIPLIER;
const float MIN_REST = 25.0f, MAX_REST = 250.0f;
const float MIN_CONST = 0.1f, MAX_CONST = 100.0f;
bool EngineDemo::ProcessInput(float /*dt*/)
{
	static int spotLightIndex = 0;
	static bool specToggle = false;

	//int multiKeyTest[]{ 'J', 'K', VK_OEM_PERIOD };
	//if (Engine::Keyboard::KeysArePressed(&multiKeyTest[0], 3)) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "3 keys pressed!\n"); }
	if (Engine::Keyboard::KeyWasPressed('C')) { debug = false; }
	if (Engine::Keyboard::KeyWasPressed('D')) { debug = true; }
	if (Engine::Keyboard::KeyWasPressed('M')) { m_conserveMomentum = !m_conserveMomentum; }

	if (Engine::Keyboard::KeyWasPressed('`')) { Engine::ConfigReader::pReader->ProcessConfigFile(); }
	if (Engine::Keyboard::KeyWasPressed('L')) { Engine::RenderEngine::LogStats(); }
	if (Engine::Keyboard::KeyWasPressed('G')) { drawGrid = !drawGrid; }
	if (Engine::Keyboard::KeyWasPressed('N'))
	{
		toggleDrag = !toggleDrag;
		if (!toggleDrag)
		{
			storeK1 = m_particleDrag.GetK1();
			storeK2 = m_particleDrag.GetK2();
			m_particleDrag.SetCoefficients(0.0f, 0.0f);
		}
		else
		{
			m_particleDrag.SetCoefficients(storeK1, storeK2);
		}
	}

	//bool shift = Engine::Keyboard::KeyIsDown(VK_SHIFT);
	//float deltaK = shift ? -0.1f : 0.1f;
	//float deltaL = shift ? -0.1f : 0.1f;

	//if (Engine::Keyboard::KeyIsDown('V'))
	//{
	//	m_particleSpring.SetRestLen(Engine::MathUtility::Clamp(m_particleSpring.GetRestLen() + deltaL, MIN_REST, MAX_REST));
	//}

	//if (Engine::Keyboard::KeyIsDown('B'))
	//{
	//	m_particleSpring.SetSpringConst(Engine::MathUtility::Clamp(m_particleSpring.GetSpringConst() + deltaK, MIN_CONST, MAX_CONST));
	//}

	if (Engine::Keyboard::KeyWasPressed('K'))
	{
		for (int i = 0; i < MAX_OBJS; ++i)
		{
			AlignObj(i);
		}
	}

	if (Engine::Keyboard::KeyWasPressed('X')) { Shutdown(); return false; }
	return true;
}

void EngineDemo::ShowFrameRate(float dt)
{
	static unsigned int numFrames = 0;
	static float    timeAccumulator = 0.0f;

	++numFrames;
	timeAccumulator += dt;	

	if (timeAccumulator >= m_fpsInterval)
	{
		//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		char fpsText[50];
		sprintf_s(fpsText, 50, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, fpsText);
		timeAccumulator = 0;
		numFrames = 0;
	}
}

const int numModels = 9;
const char *modelNames = "..\\Data\\Scenes\\Tree.PN.scene\0..\\Data\\Scenes\\Wedge.PN.scene\0..\\Data\\Scenes\\Soccer.PN.scene\0..\\Data\\Scenes\\Pipe.PN.scene\0..\\Data\\Scenes\\Coil.PN.scene\0..\\Data\\Scenes\\Cup.PN.scene\0..\\Data\\Scenes\\Star.PN.scene\0..\\Data\\Scenes\\Chair.PN.scene\0..\\Data\\Scenes\\Cone.PN.scene\0";
int indicesForModelNames[numModels] = { 0 };
bool EngineDemo::UglyDemoCode()
{
	InitIndicesForMeshNames(modelNames, &indicesForModelNames[0], numModels);

	if (!Engine::TextObject::Initialize(matLoc, tintColorLoc))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to initialize text renderer!\n");
		return false;
	}

	m_fpsTextObject.MakeBuffers();
	m_debugText.MakeBuffers();

	m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "FPS: 0\n");
	m_debugText.SetupText(0.2f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "D-Debug Menu, C-Controls\n");

	m_perspective.SetPerspective(m_pWindow->width() / static_cast<float>(m_pWindow->height()), Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());

	Engine::CollisionTester::InitializeGridDebugShapes(Engine::CollisionLayer::STATIC_GEOMETRY, Engine::Vec3(1.0f, 0.0f, 0.0f), identity.GetAddress(),
		m_perspective.GetPerspectivePtr()->GetAddress(), tintIntensityLoc, tintColorLoc, modelToWorldMatLoc, worldToViewMatLoc, perspectiveMatLoc, m_shaderPrograms[3].GetProgramId());

	Engine::CollisionTester::OnlyShowLayer(Engine::CollisionLayer::NUM_LAYERS);

	m_instanceBuffer[0].Initialize(&m_instanceMatrices[0], 16 * sizeof(float), MAX_OBJS, MAX_OBJS * 16, GL_STREAM_DRAW); // todo: ugly code fix 
	m_instanceBuffer[1].Initialize(&m_velocityArrowMatrices[0], 16 * sizeof(float), MAX_OBJS, MAX_OBJS * 16, GL_STREAM_DRAW); // todo: ugly code fix 
	m_instanceBuffer[2].Initialize(&m_momentumArrowMatrices[0], 16 * sizeof(float), MAX_OBJS, MAX_OBJS * 16, GL_STREAM_DRAW); // todo: ugly code fix 

	Engine::ShapeGenerator::MakeSphere(&m_selectedSphere, Engine::Vec3(1.0f));
	m_selectedSphere.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_selectedSphere.GetFullTransformPtr(), modelToWorldMatLoc));
	m_selectedSphere.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, &identity, worldToViewMatLoc));
	m_selectedSphere.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr(), perspectiveMatLoc));
	m_selectedSphere.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_selectedSphere.GetMatPtr()->m_materialColor, debugColorLoc));
	m_selectedSphere.SetScaleMat(Engine::Mat4::Scale(1.0f));
	m_selectedSphere.CalcFullTransform();
	Engine::RenderEngine::AddGraphicalObject(&m_selectedSphere);
	m_selectedSphere.SetEnabled(false);

	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\Sphere.PN.scene", &m_instanceGob[0], m_shaderPrograms[4].GetProgramId());
	m_instanceGob[0].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, identity.GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_instanceGob[0].GetMatPtr()->m_materialColor, cameraPosLoc, &zero, lightLoc, m_lights[0].GetLocPtr());
	
	// shader it up
	m_instanceGob[0].GetMatPtr()->m_specularIntensity = 32.0f;
	m_instanceGob[0].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_instanceGob[0].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.7f, 0.0f, 0.0f);
	m_instanceGob[0].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_instanceGob[0].SetScaleMat(Engine::Mat4::Scale(1.0f));

	m_instanceGob[0].AddUniformData(Engine::UniformData(GL_INT, &numCelLevels, 18));
	Engine::RenderEngine::AddGraphicalObject(&m_instanceGob[0]);
	m_instanceGob[0].CalcFullTransform();

	Engine::ShapeGenerator::MakeDebugArrow(&m_instanceGob[1], Engine::Vec3(1.0f, 1.0f, 0.0f), Engine::Vec3(0.0f, 1.0f, 0.0f));
	m_instanceGob[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_instanceGob[1].GetFullTransformPtr(), modelToWorldMatLoc));
	m_instanceGob[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, &identity, worldToViewMatLoc));
	m_instanceGob[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr(), perspectiveMatLoc));
	m_instanceGob[1].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_instanceGob[1].GetMatPtr()->m_materialColor, tintColorLoc));
	m_instanceGob[1].AddUniformData(Engine::UniformData(GL_FLOAT, &m_instanceGob[1].GetMatPtr()->m_specularIntensity, tintIntensityLoc));

	m_instanceGob[1].GetMatPtr()->m_specularIntensity = 0.7f;
	m_instanceGob[1].GetMatPtr()->m_materialColor = Engine::Vec3(0.0f, 1.0f, 0.0f);

	Engine::RenderEngine::AddGraphicalObject(&m_instanceGob[1]);
	m_instanceGob[1].CalcFullTransform();

	Engine::ShapeGenerator::MakeDebugArrow(&m_instanceGob[2], Engine::Vec3(1.0f, 1.0f, 0.0f), Engine::Vec3(0.0f, 1.0f, 0.0f));
	m_instanceGob[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_instanceGob[2].GetFullTransformPtr(), modelToWorldMatLoc));
	m_instanceGob[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, &identity, worldToViewMatLoc));
	m_instanceGob[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr(), perspectiveMatLoc));
	m_instanceGob[2].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_instanceGob[2].GetMatPtr()->m_materialColor, tintColorLoc));
	m_instanceGob[2].AddUniformData(Engine::UniformData(GL_FLOAT, &m_instanceGob[2].GetMatPtr()->m_specularIntensity, tintIntensityLoc));

	m_instanceGob[2].GetMatPtr()->m_specularIntensity = 0.7f;
	m_instanceGob[2].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 0.0f);

	Engine::RenderEngine::AddGraphicalObject(&m_instanceGob[2]);
	m_instanceGob[2].CalcFullTransform();

	for (int i = 0; i < MAX_OBJS; ++i)
	{
		InitObj(i);
	}

	//m_particleSpring.Init(&posOther, 1.0f, 100.0f);
	//Engine::PhysicsManager::AddForcGen(m_objPhysics[1].GetParticlePtr(), &m_particleSpring);

	m_lights[0].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(0.0f)));

	m_particleDrag.SetCoefficients(0.001f, 0.001f);


	float size = 1000.0f;
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_plane, Engine::Vec3(size, 0.0f, size), Engine::Vec3(-size, 0.0, -size), Engine::Vec3(1.0));
	m_plane.AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, identity.GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_plane.GetMatPtr()->m_materialColor, cameraPosLoc, &zero, lightLoc, m_lights[0].GetLocPtr());

	// shader it up
	m_plane.GetMatPtr()->m_specularIntensity = 32.0f;
	m_plane.GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_plane.GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.7f, 0.0f, 0.0f);
	m_plane.GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_plane.SetScaleMat(Engine::Mat4::Scale(1.0f));
	m_plane.SetTransMat(Engine::Mat4::Translation(ORIGIN));
	m_plane.SetRotMat(Engine::Mat4::RotationAroundAxis(Engine::Vec3::PLUS_X, Engine::MathUtility::ToRadians(270)));
	m_plane.AddUniformData(Engine::UniformData(GL_INT, &numCelLevels, 18));
	//Engine::RenderEngine::AddGraphicalObject(&m_plane);
	Engine::CollisionTester::AddGraphicalObjectToLayer(&m_plane, Engine::CollisionLayer::STATIC_GEOMETRY);
	m_plane.CalcFullTransform();
	m_gravity.SetGravity(Engine::Vec3(0.0f, -100.0f, 0.0f));

	return true;
}

void EngineDemo::InitIndicesForMeshNames(const char *const meshNames, int *indices, int numMeshes)
{
	indices[0] = 0;

	int meshIndex = 1;
	for (int i = 0; meshIndex < numMeshes; ++i)
	{
		if (*(meshNames + i) == '\0') { indices[meshIndex] = i + 1; meshIndex++; }
	}
}

void EngineDemo::LoadWorldFileAndApplyPCUniforms()
{
	char buffer[256]{ '\0' };
	if (Engine::ConfigReader::pReader->GetStringForKey("EngineDemo.World.InputFileName", buffer))
	{

		// read file
		Engine::WorldFileIO::ReadGobFile(&buffer[0], &m_fromWorldEditorOBJs, m_shaderPrograms[0].GetProgramId(), InitEditorObj, this);

		Engine::CollisionTester::CalculateGrid(Engine::CollisionLayer::NUM_LAYERS);
	}

	if (Engine::ConfigReader::pReader->GetStringForKey("EngineDemo.World.InputNodeFileName", buffer))
	{
		// read file

		m_nodeMap.ClearGobs(&m_fromWorldEditorOBJs, NODE_LAYER, CONNECTION_LAYER, EngineDemo::DestroyObjsCallback, this, &m_objCount);
		m_nodeMap.ClearMap();
		Engine::AStarNodeMap::FromFile(&buffer[0], &m_nodeMap);
		m_nodeMap.MakeArrowsForExistingConnections(&m_fromWorldEditorOBJs, CONNECTION_LAYER, EngineDemo::DestroyObjsCallback, this, &m_objCount, EngineDemo::SetPCUniforms, this);
		m_nodeMap.MakeObjsForExistingNodes(&m_fromWorldEditorOBJs, NODE_LAYER, EngineDemo::DestroyObjsCallback, this, &m_objCount, EngineDemo::SetPCUniforms, this);
		Engine::CollisionTester::CalculateGrid(Engine::CollisionLayer::NUM_LAYERS);
	}
}

void EngineDemo::UpdatePartitionText()
{
	/*static int lastX = 0;
	static int lastZ = 0;
	static int lastY = 0;
	static Engine::CollisionLayer lastCollisionLayer = currentCollisionLayer;
	static bool objectMoved = false;

	float x = playerGraphicalObject.GetPos().GetX();
	float y = playerGraphicalObject.GetPos().GetY();
	float z = playerGraphicalObject.GetPos().GetZ();
	int cX = Engine::CollisionTester::GetGridIndexFromPosX(x, Engine::CollisionLayer::STATIC_GEOMETRY);
	int cY = Engine::CollisionTester::GetGridIndexFromPosX(y, Engine::CollisionLayer::STATIC_GEOMETRY);
	int cZ = Engine::CollisionTester::GetGridIndexFromPosZ(z, Engine::CollisionLayer::STATIC_GEOMETRY);

	if (cX != lastX || cZ != lastZ || cY != lastY || lastCollisionLayer != currentCollisionLayer || objectMoved)
	{
		char buffer[75], buffer2[75];
		sprintf_s(buffer2, 75, "Layer [%s]:\n", Engine::CollisionTester::LayerString(currentCollisionLayer));
		if (Engine::CollisionTester::GetTriangleCountForSpace(x, y, z) < 0) { sprintf_s(buffer, 50, "Outside Spatial Grid!\n"); }
		else { sprintf_s(buffer, 75, "[%d] triangles in [%d] [%d] [%d]\n", Engine::CollisionTester::GetTriangleCountForSpace(x, y, z, currentCollisionLayer), cX, cY, cZ); }
		m_objectText.SetupText(0.20f, 0.75f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, buffer);
		m_layerText.SetupText(0.20f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, buffer2);
		lastX = cX;
		lastY = cY;
		lastZ = cZ;
		objectMoved = false;
	}

	lastCollisionLayer = currentCollisionLayer;*/
}


void EngineDemo::InitObj(int index)
{
	// make name for NPC
	char nameBuffer[7] = "OBJ";
	nameBuffer[3] = '0' + (char)(index / 100);
	nameBuffer[4] = '0' + (char)((index % 100) / 10);
	nameBuffer[4] = '0' + (char)(index % 10);
	nameBuffer[6] = '\0';

	// place it, some possibly redundant code here
	m_objGobsComps[index].SetGraphicalObject(&m_objGobs[index]);

	Engine::PhysicsManager::RegisterComponent(&m_objPhysics[index]);

	// add components to obj and initialize it
	m_objs[index].SetName(&nameBuffer[0]);
	m_objs[index].AddComponent(&m_objSpatials[index], "OBJ Spatial");
	m_objs[index].AddComponent(&m_objGobsComps[index], "OBJ Gob");
	m_objs[index].AddComponent(&m_objPhysics[index], "OBJ Physics");
	m_objs[index].Initialize();

	// give all ze particles drag!
	Engine::PhysicsManager::AddForcGen(m_objPhysics[index].GetParticlePtr(), &m_particleDrag);
	if (index < MAX_OBJS - 1 && index > 0)
	{
		Engine::PhysicsManager::AddForcGen(m_objPhysics[index].GetParticlePtr(), &m_gravity);
	}

	AlignObj(index);
}

const int OBJ_PER_DIR = (int)sqrtf((float)MAX_OBJS) + 1;
void EngineDemo::AlignObj(int index)
{
	float scale = index < MAX_OBJS - 1 ? 15.0f : 100.0f;

	Engine::Vec3 start = (Engine::Vec3(0.0f, 50.0f, 0.0f) * (float)(index - (MAX_OBJS / 2)));
	Engine::Vec3 pos = ORIGIN + (index == 0 ? Engine::Vec3(150.0f, 0.0f, 0.0f) : start);
	if (index == MAX_OBJS - 1)
	{
		pos = ORIGIN + Engine::Vec3(0.0f, -(scale / 2.0f + 150.0f), 0.0f);
		m_objPhysics[index].SetMass(0.0f, false);
	}
	else
	{
		m_objPhysics[index].SetMass(MASS_PER_VOLUME * FOUR_THIRDS_PI * scale*scale*scale, false);
	}

	m_objGobs[index].SetTransMat(Engine::Mat4::Translation(pos));
	m_objPhysics[index].SetPosition(pos);
	m_objPhysics[index].SetVelocity(Engine::Vec3(0.0f));
	m_objGobs[index].SetScaleMat(Engine::Mat4::Scale(scale));
	
	// radius 4/3 pi r cubed, but all have that coefficient so not really needed...
	m_objPhysics[index].SetRadius(MAGIC_RADIUS_SCALE * scale);
}

void EngineDemo::RandomizeObj(int index)
{
	float scale = Engine::MathUtility::Rand(MIN_RADIUS, MAX_RADIUS);
	Engine::Vec3 pos = ORIGIN + Engine::MathUtility::GetRandSphereEdgeVec(Engine::MathUtility::Rand(100.0f, 1000.0f));
	pos = Engine::Vec3(pos.GetX(), pos.GetY(), ORIGIN.GetZ());
	m_objGobs[index].SetTransMat(Engine::Mat4::Translation(pos));
	m_objPhysics[index].SetPosition(pos);
	m_objPhysics[index].SetVelocity(Engine::MathUtility::Rand(10.0f, 50.0f) * (ORIGIN - pos).Normalize());
	m_objGobs[index].SetScaleMat(Engine::Mat4::Scale(scale));

	// radius 4/3 pi r cubed, but all have that coefficient so not really needed...
	m_objPhysics[index].SetMass(MASS_PER_VOLUME * FOUR_THIRDS_PI * scale*scale*scale, false);
	m_objPhysics[index].SetRadius(MAGIC_RADIUS_SCALE * scale);
}

bool EngineDemo::DestroyObjsCallback(Engine::GraphicalObject * pObj, void * pClassInstance)
{
	EngineDemo* pDemo = reinterpret_cast<EngineDemo*>(pClassInstance);

	Engine::RenderEngine::RemoveGraphicalObject(pObj);
	Engine::CollisionTester::RemoveGraphicalObjectFromLayer(pObj, Engine::CollisionLayer::LAYER_2); 

	delete pObj;

	pDemo->m_objCount--;

	return true;
}

void EngineDemo::InitEditorObj(Engine::GraphicalObject * pObj, void * pClass)
{
	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(pClass);

	SetPCUniforms(pObj, pGame);
	pObj->GetMatPtr()->m_materialColor = Engine::MathUtility::Rand(Engine::Vec3(0.0f), Engine::Vec3(1.0f));
	pObj->GetMatPtr()->m_specularIntensity = 0.5f;

	// add it to the necessary things, it'll get deleted on shutdown or remove
	Engine::RenderEngine::AddGraphicalObject(pObj);
	Engine::CollisionTester::AddGraphicalObjectToLayer(pObj, Engine::CollisionLayer::LAYER_2);
	pGame->m_objCount++;
}

void EngineDemo::SetPCUniforms(Engine::GraphicalObject * pObj, void * pInstance)
{
	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(pInstance);

	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, pObj->GetFullTransformPtr(), pGame->modelToWorldMatLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, identity.GetAddress(), pGame->worldToViewMatLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, pGame->m_perspective.GetPerspectivePtr(), pGame->perspectiveMatLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &pObj->GetMatPtr()->m_materialColor, pGame->tintColorLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT, &pObj->GetMatPtr()->m_specularIntensity, pGame->tintIntensityLoc));
}
