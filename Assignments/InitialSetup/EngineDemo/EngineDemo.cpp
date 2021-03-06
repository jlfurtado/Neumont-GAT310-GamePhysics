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

const float EngineDemo::LIGHT_HEIGHT = 15.0f;
const float EngineDemo::RENDER_DISTANCE = 2000.0f;

namespace {
	const int MAX_OBJS = 100; // 1000 for release
	Engine::Entity m_objs[MAX_OBJS];
	Engine::SpatialComponent m_objSpatials[MAX_OBJS];
	Engine::GraphicalObjectComponent m_objGobsComps[MAX_OBJS];
	Engine::GraphicalObject m_objGobs[MAX_OBJS];
	Engine::Mat4 m_instanceMatrices[MAX_OBJS];
	Engine::PhysicsComponent m_objPhysics[MAX_OBJS];

	Engine::GraphicalObject m_instanceGob;
	Engine::InstanceBuffer m_instanceBuffer;
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


	Engine::CollisionTester::CalculateGrid();

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
	
	player.Shutdown();

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

	if (!won) { ShowFrameRate(dt); }
	if (!ProcessInput(dt)) { return; }

	playerGraphicalObject.CalcFullTransform();
	player.Update(dt);

	UpdatePartitionText();

	if (paused) { return; }

	//if (!backgroundMusic.GetIsPlaying())
	//{
	//	backgroundMusic.Play();
	//}

	m_lights[0].SetTransMat(Engine::Mat4::Translation(playerGraphicalObject.GetPos() + Engine::Vec3(0.0f, 15.0f, 0.0f)));

	for (int i = 0; i < MAX_OBJS; ++i)
	{
		m_objs[i].Update(dt);
		m_objGobs[i].CalcFullTransform();
		m_instanceMatrices[i] = *m_objGobs[i].GetFullTransformPtr();
	}

	m_instanceBuffer.UpdateData(&m_instanceMatrices[0], 0, 16 * sizeof(float)*MAX_OBJS, MAX_OBJS);

}

void EngineDemo::Draw()
{
	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_instanceGob.SetEnabled(false);
	Engine::RenderEngine::Draw();
	m_instanceGob.SetEnabled(true);

	Engine::RenderEngine::DrawInstanced(&m_instanceGob, &m_instanceBuffer);
	if (drawGrid) { Engine::CollisionTester::DrawGrid(Engine::CollisionLayer::STATIC_GEOMETRY, playerGraphicalObject.GetPos()); }

	m_fpsTextObject.RenderText(&m_shaderPrograms[1], debugColorLoc);
	m_objectText.RenderText(&m_shaderPrograms[1], debugColorLoc);
	m_layerText.RenderText(&m_shaderPrograms[1], debugColorLoc);
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

void EngineDemo::OnMouseScroll(int degrees)
{
	mouseComponent.MouseScroll(degrees);
}

void EngineDemo::OnMouseMove(int deltaX, int deltaY)
{
	if (Engine::MouseManager::IsRightMouseDown()) { mouseComponent.MouseMove(deltaX, deltaY); }
	mouseComponent.SetMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY());
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
		m_shaderPrograms[0].AddVertexShader("..\\Data\\Shaders\\PC.Vert.shader");
		m_shaderPrograms[0].AddFragmentShader("..\\Data\\Shaders\\PC.Frag.shader");
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
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetSpeed(cameraSpeed);

	float cameraRotationSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraRotationSpeed", cameraRotationSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraRotationSpeed!\n"); return false; }
	if (cameraRotationSpeed < 0.0f) { cameraRotationSpeed = 1.0f; }
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetRotateSpeed(cameraRotationSpeed);

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
bool EngineDemo::ProcessInput(float /*dt*/)
{
	static int spotLightIndex = 0;
	static bool specToggle = false;

	//int multiKeyTest[]{ 'J', 'K', VK_OEM_PERIOD };
	//if (Engine::Keyboard::KeysArePressed(&multiKeyTest[0], 3)) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "3 keys pressed!\n"); }
	if (Engine::Keyboard::KeyWasReleased('C')) { Engine::CollisionTester::ConsoleLogOutput(); }
	if (Engine::Keyboard::KeyWasPressed('`')) { Engine::ConfigReader::pReader->ProcessConfigFile(); }
	if (Engine::Keyboard::KeyWasPressed('I')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "(%.3f, %.3f, %.3f)\n", playerGraphicalObject.GetPos().GetX(), playerGraphicalObject.GetPos().GetY(), playerGraphicalObject.GetPos().GetZ()); }
	if (Engine::Keyboard::KeyWasPressed('L')) { Engine::RenderEngine::LogStats(); }
	if (Engine::Keyboard::KeyWasPressed('G')) { drawGrid = !drawGrid; }
	if (Engine::Keyboard::KeyWasPressed('N'))
	{
		playerCamera.SetSpeed(Engine::MathUtility::Clamp(playerCamera.GetSpeed() * 0.75f, MIN_SPEED, MAX_SPEED));
		playerCamera.SetRotateSpeed(Engine::MathUtility::Clamp(playerCamera.GetRotateSpeed() * 0.75f, MIN_ROTATION_SPEED, MAX_ROTATION_SPEED));

	}
	if (Engine::Keyboard::KeyWasPressed('M')) 
	{ 
		playerCamera.SetSpeed(Engine::MathUtility::Clamp(playerCamera.GetSpeed() * 1.25f, MIN_SPEED, MAX_SPEED));
		playerCamera.SetRotateSpeed(Engine::MathUtility::Clamp(playerCamera.GetRotateSpeed() * 1.25f, MIN_ROTATION_SPEED, MAX_ROTATION_SPEED));
	}
	if (Engine::Keyboard::KeyWasPressed('J'))
	{
		for (int i = 0; i < MAX_OBJS; ++i)
		{
			RandomizeObj(i);
		}
	}
	if (Engine::Keyboard::KeyWasPressed('K'))
	{
		for (int i = 0; i < MAX_OBJS; ++i)
		{
			AlignObj(i);
		}
	}
	if (Engine::Keyboard::KeyIsDown(VK_SHIFT))
	{
		if (Engine::Keyboard::KeyWasPressed('0')) { currentCollisionLayer = Engine::CollisionLayer::STATIC_GEOMETRY; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('1')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_1; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('2')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_2; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('3')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_3; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('4')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_4; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('5')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_5; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('6')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_6; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('7')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_7; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('8')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_8; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }
		if (Engine::Keyboard::KeyWasPressed('9')) { currentCollisionLayer = Engine::CollisionLayer::LAYER_9; Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer); }

		if (Engine::Keyboard::KeyWasPressed('U')) { currentCollisionLayer = Engine::CollisionLayer::NUM_LAYERS; Engine::CollisionTester::OnlyShowLayer(Engine::CollisionLayer::NUM_LAYERS); }
	}
	
	if (Engine::Keyboard::KeyWasPressed('X')) { Shutdown(); return false; }
	if (Engine::Keyboard::KeyWasPressed('O')) { speedMultiplier -= 0.1f; }
	if (Engine::Keyboard::KeyWasPressed('Z')) { speedMultiplier += 0.1f; }
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
	playerCamera = Engine::ChaseCameraComponent(Engine::Vec3( 0, 30, 50), Engine::Vec3(0, 5, 0), Engine::Vec3(0), true, Engine::CollisionLayer::LAYER_2);

	player.SetName("Player");
	player.AddComponent(&playerSpatial, "PlayerSpatial");
	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\BetterDargon.PN.scene", &playerGraphicalObject, m_shaderPrograms[2].GetProgramId());
	playerGraphicalObject.AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&playerGraphicalObject.GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_INT, &numCelLevels, 18));

	playerGraphicalObject.GetMatPtr()->m_specularIntensity = 32.0f;
	playerGraphicalObject.GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.0f, 0.1f, 0.0f);
	playerGraphicalObject.GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.0f, 0.7f, 0.0f);
	playerGraphicalObject.GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.0f, 0.1f, 0.0f);
	playerGraphicalObject.SetScaleMat(Engine::Mat4::Scale(1.0f));
	playerGraphicalObject.SetTransMat(Engine::Mat4::Translation(Engine::Vec3(375.0f, 5.0f, 5.0f)));
	Engine::RenderEngine::AddGraphicalObject(&playerGraphicalObject);
	playerGob.SetGraphicalObject(&playerGraphicalObject);
	player.AddComponent(&playerGob, "PlayerGob");
	player.AddComponent(&playerCamera, "PlayerCamera");
	player.AddComponent(&playerInput, "PlayerInput");
	player.AddComponent(&mouseComponent, "MouseComponent");
	player.Initialize();

	player.GetComponentByType<Engine::SpatialComponent>()->SetPosition(Engine::Vec3(375.0f, 5.0f, 5.0f));
	if (!Engine::TextObject::Initialize(matLoc, tintColorLoc))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to initialize text renderer!\n");
		return false;
	}

	m_fpsTextObject.MakeBuffers();
	m_objectText.MakeBuffers();
	m_layerText.MakeBuffers();

	m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "FPS: 0\n");
	m_objectText.SetupText(0.0f, 0.75f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "TRIANGLES: 0\n");
	m_layerText.SetupText(0.20f, 0.9f, 0.1f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "LAYER NOT DISPLAYED YET");
	m_perspective.SetPerspective(m_pWindow->width() / static_cast<float>(m_pWindow->height()), Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());

	Engine::CollisionTester::InitializeGridDebugShapes(Engine::CollisionLayer::STATIC_GEOMETRY, Engine::Vec3(1.0f, 0.0f, 0.0f), playerCamera.GetWorldToViewMatrixPtr()->GetAddress(),
		m_perspective.GetPerspectivePtr()->GetAddress(), tintIntensityLoc, tintColorLoc, modelToWorldMatLoc, worldToViewMatLoc, perspectiveMatLoc, m_shaderPrograms[3].GetProgramId());

	Engine::CollisionTester::OnlyShowLayer(currentCollisionLayer);

	Engine::ShapeGenerator::MakeGrid(&m_grid, 85, 85, Engine::Vec3(0.5f));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_grid.GetFullTransformPtr(), modelToWorldMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr(), perspectiveMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_grid.GetMatPtr()->m_materialColor, tintColorLoc));
	m_grid.SetScaleMat(Engine::Mat4::Scale(25.0f));
	m_grid.CalcFullTransform();
	Engine::RenderEngine::AddGraphicalObject(&m_grid);

	Engine::ShapeGenerator::MakeSphere(&m_originMarker, Engine::Vec3(1.0f));
	m_originMarker.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_originMarker.GetFullTransformPtr(), modelToWorldMatLoc));
	m_originMarker.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_originMarker.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr(), perspectiveMatLoc));
	m_originMarker.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_originMarker.GetMatPtr()->m_materialColor, tintColorLoc));
	m_originMarker.SetScaleMat(Engine::Mat4::Scale(25.0f));
	m_originMarker.CalcFullTransform();
	Engine::RenderEngine::AddGraphicalObject(&m_originMarker);
	
	LoadWorldFileAndApplyPCUniforms();

	m_instanceBuffer.Initialize(&m_instanceMatrices[0], 16 * sizeof(float), MAX_OBJS, MAX_OBJS * 16, GL_STREAM_DRAW); // todo: ugly code fix 

	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\Sphere.PN.scene", &m_instanceGob, m_shaderPrograms[4].GetProgramId());
	m_instanceGob.AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_instanceGob.GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
	
	// shader it up
	m_instanceGob.GetMatPtr()->m_specularIntensity = 32.0f;
	m_instanceGob.GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_instanceGob.GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.7f, 0.0f, 0.0f);
	m_instanceGob.GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f, 0.0f, 0.0f);
	m_instanceGob.SetScaleMat(Engine::Mat4::Scale(1.0f));

	m_instanceGob.AddUniformData(Engine::UniformData(GL_INT, &numCelLevels, 18));
	Engine::RenderEngine::AddGraphicalObject(&m_instanceGob);
	m_instanceGob.CalcFullTransform();
	for (int i = 0; i < MAX_OBJS; ++i)
	{
		InitObj(i);
	}

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
	static int lastX = 0;
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

	lastCollisionLayer = currentCollisionLayer;
}

const Engine::Vec3 ORIGIN = Engine::Vec3(0.0f, 250.0f, 0.0f);
const float MAGIC_RADIUS_SCALE = 1.0f; // todo: collider visualization? update const if different model???
const float MASS_PER_VOLUME = 1.0f;
void EngineDemo::InitObj(int index)
{
	// make name for NPC
	char nameBuffer[7] = "OBJ";
	nameBuffer[3] = '0' + (char)(index / 100);
	nameBuffer[4] = '0' + (char)((index % 100) / 10);
	nameBuffer[4] = '0' + (char)(index % 10);
	nameBuffer[6] = '\0';

	// place it, some possibly redundant code here
	RandomizeObj(index);
	m_objGobsComps[index].SetGraphicalObject(&m_objGobs[index]);

	Engine::PhysicsManager::RegisterComponent(&m_objPhysics[index]);

	// add components to obj and initialize it
	m_objs[index].SetName(&nameBuffer[0]);
	m_objs[index].AddComponent(&m_objSpatials[index], "OBJ Spatial");
	m_objs[index].AddComponent(&m_objGobsComps[index], "OBJ Gob");
	m_objs[index].AddComponent(&m_objPhysics[index], "OBJ Physics");
	m_objs[index].Initialize();
}

const int OBJ_PER_DIR = (int)sqrtf(MAX_OBJS/2) + 1;
void EngineDemo::AlignObj(int index)
{
	if (index >= 2)
	{
		float scale = Engine::MathUtility::Rand(5.0f, 15.0f);

		Engine::Vec3 pos = ORIGIN + Engine::MathUtility::GetCubification(index, OBJ_PER_DIR, 2, OBJ_PER_DIR, 200.0f);
		m_objGobs[index].SetTransMat(Engine::Mat4::Translation(pos));
		m_objSpatials[index].SetPosition(pos);
		int rand = Engine::MathUtility::Rand(0, 2);
		float v = Engine::MathUtility::Rand(10.0f, 50.0f);
		m_objSpatials[index].SetVelocity(Engine::Vec3(0.0f, rand == 1 ? v : -v, 0.0f));
		m_objGobs[index].SetScaleMat(Engine::Mat4::Scale(scale));

		// radius 4/3 pi r cubed, but all have that coefficient so not really needed...
		m_objPhysics[index].SetMass(MASS_PER_VOLUME * scale*scale*scale);
		m_objPhysics[index].SetRadius(MAGIC_RADIUS_SCALE * scale);
	}
	else { BounceDefaultDemo(index); }
}

void EngineDemo::RandomizeObj(int index)
{
	if (index >= 2)
	{
		float scale = Engine::MathUtility::Rand(5.0f, 15.0f);
		Engine::Vec3 pos = ORIGIN + Engine::MathUtility::GetRandSphereEdgeVec(Engine::MathUtility::Rand(100.0f, 1000.0f));
		m_objGobs[index].SetTransMat(Engine::Mat4::Translation(pos));
		m_objSpatials[index].SetPosition(pos);
		m_objSpatials[index].SetVelocity(Engine::MathUtility::Rand(10.0f, 50.0f) * (ORIGIN - pos).Normalize());
		m_objGobs[index].SetScaleMat(Engine::Mat4::Scale(scale));

		// radius 4/3 pi r cubed, but all have that coefficient so not really needed...
		m_objPhysics[index].SetMass(MASS_PER_VOLUME * scale*scale*scale);
		m_objPhysics[index].SetRadius(MAGIC_RADIUS_SCALE * scale);
	}
	else { BounceDefaultDemo(index); }
}

void EngineDemo::BounceDefaultDemo(int index)
{
	float scale = 5.0f;
	Engine::Vec3 pos = Engine::Vec3(index == 0 ? 300.0f : 400.0f, 10.0f, -150.0f);
	m_objGobs[index].SetTransMat(Engine::Mat4::Translation(pos));
	m_objSpatials[index].SetPosition(pos);
	m_objSpatials[index].SetVelocity(Engine::Vec3(index == 0 ? 10.0f : -10.0f, 0.0f, 0.0f));
	m_objGobs[index].SetScaleMat(Engine::Mat4::Scale(scale));

	// mass 4/3 pi r cubed, but all have that coefficient so not really needed...
	m_objPhysics[index].SetMass(MASS_PER_VOLUME * scale*scale*scale);
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
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, pGame->playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), pGame->worldToViewMatLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, pGame->m_perspective.GetPerspectivePtr(), pGame->perspectiveMatLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &pObj->GetMatPtr()->m_materialColor, pGame->tintColorLoc));
	pObj->AddUniformData(Engine::UniformData(GL_FLOAT, &pObj->GetMatPtr()->m_specularIntensity, pGame->tintIntensityLoc));
}
