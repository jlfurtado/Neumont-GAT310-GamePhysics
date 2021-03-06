#ifndef EngineDemo_H
#define EngineDemo_H

// Justin Furtado
// 6/21/2016
// EngineDemo.h
// The game

#include "ShaderProgram.h"
#include "TextObject.h"
#include "Keyboard.h"
#include "Perspective.h"
#include "Camera.h"
#include "ChaseCamera.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Entity.h"
#include "MyWindow.h"
#include "GraphicalObject.h"
#include "FrameBuffer.h"
#include "LinkedList.h"
#include "AStarNodeMap.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "ChaseCameraComponent.h"
#include "GraphicalObjectComponent.h"
#include "SpatialComponent.h"

class QMouseEvent;

class EngineDemo
{
public:
	// methods
	bool Initialize(Engine::MyWindow *window);
	bool Shutdown();

	// callbacks
	static bool InitializeCallback(void *game, Engine::MyWindow *window);
	static void UpdateCallback(void *game, float dt);
	static void ResizeCallback(void *game);
	static void DrawCallback(void *game);
	static void MouseScrollCallback(void *game, int degrees);
	static void MouseMoveCallback(void *game, int dx, int dy);
	static void PhysicsCallback(void *game);

	void DoPhysics();
	void Update(float dt);
	void Draw();
	
	void OnResizeWindow();
	void OnMouseScroll(int degrees);
	void OnMouseMove(int deltaX, int deltaY);

	bool GameIsPaused() const { return paused; }

	static void OnConfigReload(void *classInstance);
	static const float RENDER_DISTANCE;

private:
	// methods
	bool ReadConfigValues();
	bool InitializeGL();
	bool ProcessInput(float dt);
	void ShowFrameRate(float dt);
	bool UglyDemoCode();
	void InitIndicesForMeshNames(const char *const meshNames, int *indices, int numMeshes);
	void LoadWorldFileAndApplyPCUniforms();
	void UpdatePartitionText();
	void InitObj(int index);
	void AlignObj(int index);
	void RandomizeObj(int index);
	void BounceDefaultDemo(int index);
	static bool DestroyObjsCallback(Engine::GraphicalObject *pObj, void *pClassInstance);
	static void InitEditorObj(Engine::GraphicalObject *pObj, void *pClass);
	static void SetPCUniforms(Engine::GraphicalObject *pObj, void *pInstance);

	//data
	static const int NUM_SHADER_PROGRAMS = 5;
	static const float LIGHT_HEIGHT;
	static const Engine::CollisionLayer NODE_LAYER = Engine::CollisionLayer::LAYER_3;
	static const Engine::CollisionLayer CONNECTION_LAYER = Engine::CollisionLayer::LAYER_4;

	float speedMultiplier = 1.0f;
	Engine::CollisionLayer currentCollisionLayer = Engine::CollisionLayer::NUM_LAYERS;
	bool drawGrid = true;
	bool allLayersEnabled = true;

	Engine::GraphicalObject m_grid;
	Engine::GraphicalObject m_lights[1];
	Engine::GraphicalObject playerGraphicalObject;

	Engine::Entity player;
	Engine::ChaseCameraComponent playerCamera;
	Engine::GraphicalObjectComponent playerGob;
	Engine::SpatialComponent playerSpatial;
	MouseComponent mouseComponent;
	KeyboardComponent playerInput;
	int numCelLevels = 4;
	Engine::Vec3 backgroundColor{ 0.0f, 0.0f, 0.0f };

	bool paused = false;
	Engine::Perspective m_perspective;
	Engine::TextObject m_fpsTextObject;
	Engine::TextObject m_objectText;
	Engine::TextObject m_layerText;
	Engine::MyWindow *m_pWindow{ nullptr };
	Engine::ShaderProgram m_shaderPrograms[NUM_SHADER_PROGRAMS];
	Engine::ShaderProgram m_shaderProgramText;
	GLint matLoc;
	GLint debugColorLoc;
	GLint tintColorLoc;
	GLint tintIntensityLoc;
	GLint texLoc;
	GLint modelToWorldMatLoc;
	GLint worldToViewMatLoc;
	GLint perspectiveMatLoc;
	GLint lightLoc;
	GLint cameraPosLoc;
	GLint ambientColorLoc;
	GLint ambientIntensityLoc;
	GLint diffuseColorLoc;
	GLint diffuseIntensityLoc;
	GLint specularColorLoc;
	GLint specularIntensityLoc;
	GLint specularPowerLoc;
	GLint lightsMin;
	GLint spotMin;
	GLint phongShaderMethodIndex;
	GLint diffuseShaderMethodIndex;
	GLint directionalPositionLoc;
	GLint levelsLoc;
	GLint halfWidthLoc;
	GLint repeatScaleLoc;
	Engine::GraphicalObject m_gazebo;
	Engine::GraphicalObject m_flag;
	Engine::AStarNodeMap m_nodeMap;
	float repeatScale;
	GLint numIterationsLoc;
	int numIterations;
	GLint shaderOffsetLoc;
	float step = 0.1f;
	float shaderOffset = 0.0f;
	Engine::Vec2 fractalSeed;
	Engine::Vec3 fsx;
	Engine::Vec3 fsy;
	Engine::Vec4 spotlightAttenuations;
	float m_fpsInterval = 1.0f;
	bool won = false;
	float specularPower;
	Engine::GraphicalObject m_originMarker;
	int m_objCount{ 0 };
	Engine::LinkedList<Engine::GraphicalObject*> m_fromWorldEditorOBJs;
};

#endif // ifndef EngineDemo_h