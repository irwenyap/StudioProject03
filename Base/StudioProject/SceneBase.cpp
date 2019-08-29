#include "SceneBase.h"
#include "GL/glew.h"

#include "../Source/Application.h"
#include "../Source/shader.hpp"
#include "../Source/MeshBuilder.h"
#include "../Source/Utility.h"
#include "../Source/LoadTGA.h"
#include <sstream>
#include "SceneManager.h"
#include "Physics.h"

SceneBase::SceneBase()
{
}


SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	CSoundEngine::Getinstance()->Init();
	CSoundEngine::Getinstance()->setSoundVolume(0.3f);
	CSoundEngine::Getinstance()->AddSound("ESHATTER", "Sound//Enemy.ogg");
	CSoundEngine::Getinstance()->AddSound("Gunshot", "Sound//Gunshot.ogg");
	CSoundEngine::Getinstance()->AddSound("pickup", "Sound//pickup.ogg");
	CSoundEngine::Getinstance()->AddSound("Text", "Sound//Text.ogg");
	CSoundEngine::Getinstance()->AddSound("super", "Sound//Super.mp3");
	CSoundEngine::Getinstance()->AddSound("cool", "Sound//Cool1.mp3");
	Physics::dead = false;
	// Black background
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	m_programID = LoadShaders("Shader//Shadow.vert", "Shader//Shadow.frag");

	// Shadow
	m_gPassShaderID = LoadShaders("Shader//GPass.vert", "Shader//GPass.frag");
	m_parameters[U_LIGHT_DEPTH_MVP_GPASS] = glGetUniformLocation(m_gPassShaderID, "lightDepthMVP");
	m_parameters[U_LIGHT_DEPTH_MVP] = glGetUniformLocation(m_programID, "lightDepthMVP");
	m_parameters[U_SHADOW_MAP] = glGetUniformLocation(m_programID, "shadowMap");

	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled[0]");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture[0]");
	m_parameters[U_COLOR_TEXTURE_ENABLED1] = glGetUniformLocation(m_programID, "colorTextureEnabled[1]");
	m_parameters[U_COLOR_TEXTURE1] = glGetUniformLocation(m_programID, "colorTexture[1]");
	m_parameters[U_COLOR_TEXTURE_ENABLED2] = glGetUniformLocation(m_programID, "colorTextureEnabled[2]");
	m_parameters[U_COLOR_TEXTURE2] = glGetUniformLocation(m_programID, "colorTexture[2]");
	m_parameters[U_SKYDOME] = glGetUniformLocation(m_programID, "skybox");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// FOG
	m_parameters[U_FOG_COLOR] = glGetUniformLocation(m_programID, "fogParam.color");
	m_parameters[U_FOG_START] = glGetUniformLocation(m_programID, "fogParam.start");
	m_parameters[U_FOG_END] = glGetUniformLocation(m_programID, "fogParam.end");
	m_parameters[U_FOG_DENSITY] = glGetUniformLocation(m_programID, "fogParam.density");
	m_parameters[U_FOG_TYPE] = glGetUniformLocation(m_programID, "fogParam.type");
	m_parameters[U_FOG_ENABLED] = glGetUniformLocation(m_programID, "fogParam.enabled");

	// Shadow Alpha
	m_parameters[U_SHADOW_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_gPassShaderID, "colorTextureEnabled[0]");
	m_parameters[U_SHADOW_COLOR_TEXTURE] = glGetUniformLocation(m_gPassShaderID, "colorTexture[0]");
	m_parameters[U_SHADOW_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_gPassShaderID, "colorTextureEnabled[1]");
	m_parameters[U_SHADOW_COLOR_TEXTURE] = glGetUniformLocation(m_gPassShaderID, "colorTexture[1]");
	m_parameters[U_SHADOW_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_gPassShaderID, "colorTextureEnabled[2]");
	m_parameters[U_SHADOW_COLOR_TEXTURE] = glGetUniformLocation(m_gPassShaderID, "colorTexture[2]");

	// Use our shader
	glUseProgram(m_programID);

	lights[1].type = Light::LIGHT_DIRECTIONAL;
	lights[1].position.Set(465, 255, 375);
	lights[1].color.Set(0.24f, 0.19f, 0.06f);
	lights[1].power = 10.f;
	lights[1].kC = 1.f;
	lights[1].kL = 0.01f;
	lights[1].kQ = 0.001f;
	lights[1].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[1].cosInner = cos(Math::DegreeToRadian(30));
	lights[1].exponent = 3.f;
	lights[1].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], lights[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], lights[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], lights[1].exponent);

	m_fchRotate = 0;

	cameraID = 1;

	m_fMTElapsedTime = 0.f;

	m_fFOV = 45.f;

	// Mesh Init
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}

	meshList[GEO_OBB_GREEN] = MeshBuilder::GenerateOBB("obb", Color(0, 1, 0), 1.f);
	meshList[GEO_OBB_RED] = MeshBuilder::GenerateOBB("obb", Color(1, 0, 0), 1.f);
	meshList[GEO_OBB_BLUE] = MeshBuilder::GenerateOBB("obb", Color(0, 0, 1), 1.f);
	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 1), 1.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 0), 1.f);
	// UI
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference");
	meshList[GEO_CROSSHAIR] = MeshBuilder::GenerateQuad("crosshair", Color(1, 1, 1), 1.f);
	meshList[GEO_CROSSHAIR]->textureArray[0] = LoadTGA("Image//crosshair.tga");
	meshList[GEO_FIST] = MeshBuilder::GenerateQuad("crosshair", Color(1, 1, 1), 1.f);
	meshList[GEO_FIST]->textureArray[0] = LoadTGA("Image//fist.tga");
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureArray[0] = LoadTGA("Image//calibri.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);


	meshList[GEO_FLOOR] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 1.f);
	meshList[GEO_WALL] = MeshBuilder::GenerateCube("cube", Color(0.45, 0.45, 0.45), 1.f);
	meshList[GEO_BULLET] = MeshBuilder::GenerateOBJ("bullet", "OBJ//bullet.obj");
	//meshList[GEO_BULLET]->textureArray[0] = LoadTGA("Image//gun.tga");
	meshList[GEO_TRACER] = MeshBuilder::GenerateCylinder("tracer", Color(1, 0, 0), 10, 10, 1, 1);
	meshList[GEO_PA_GUNSHATTER] = MeshBuilder::GenerateQuad("gunshatter", Color(), 1.f);
	meshList[GEO_PA_GUNSHATTER]->textureArray[0] = LoadTGA("Image//PistolTri.tga");
	meshList[GEO_PA_GUNSMOKE] = MeshBuilder::GenerateQuad("gunsmoke", Color(), 1.f);
	meshList[GEO_PA_GUNSMOKE]->textureArray[0] = LoadTGA("Image//smoke.tga");
	meshList[GEO_D] = MeshBuilder::GenerateQuad("gunsmoke", Color(), 1.f);
	meshList[GEO_D]->textureArray[0] = LoadTGA("Image//png.tga");
	meshList[GEO_PA_ENEMYSHATTER1] = MeshBuilder::GenerateQuad("gunshatter", Color(), 1.f);
	meshList[GEO_PA_ENEMYSHATTER1]->textureArray[0] = LoadTGA("Image//tri1.tga");
	meshList[GEO_PA_ENEMYSHATTER2] = MeshBuilder::GenerateQuad("gunshatter", Color(), 1.f);
	meshList[GEO_PA_ENEMYSHATTER2]->textureArray[0] = LoadTGA("Image//tri2.tga");

	meshList[GEO_TIME] = MeshBuilder::GenerateQuad("time", Color(1, 1, 1), 1.f);
	meshList[GEO_TIME]->textureArray[0] = LoadTGA("Image//time.tga");
	meshList[GEO_KILL] = MeshBuilder::GenerateQuad("kill", Color(1, 1, 1), 1.f);
	meshList[GEO_KILL]->textureArray[0] = LoadTGA("Image//kill.tga");
	meshList[GEO_THEM] = MeshBuilder::GenerateQuad("them", Color(1, 1, 1), 1.f);
	meshList[GEO_THEM]->textureArray[0] = LoadTGA("Image//them.tga");
	meshList[GEO_ALL] = MeshBuilder::GenerateQuad("all", Color(1, 1, 1), 1.f);
	meshList[GEO_ALL]->textureArray[0] = LoadTGA("Image//all.tga");
	meshList[GEO_GRAB] = MeshBuilder::GenerateQuad("grab", Color(1, 1, 1), 1.f);
	meshList[GEO_GRAB]->textureArray[0] = LoadTGA("Image//grab.tga");
	meshList[GEO_THE] = MeshBuilder::GenerateQuad("the", Color(1, 1, 1), 1.f);
	meshList[GEO_THE]->textureArray[0] = LoadTGA("Image//the.tga");
	meshList[GEO_GUN] = MeshBuilder::GenerateQuad("gun", Color(1, 1, 1), 1.f);
	meshList[GEO_GUN]->textureArray[0] = LoadTGA("Image//gun.tga");
	meshList[GEO_SUPER] = MeshBuilder::GenerateQuad("super", Color(1, 1, 1), 1.f);
	meshList[GEO_SUPER]->textureArray[0] = LoadTGA("Image//super.tga");
	meshList[GEO_COOL] = MeshBuilder::GenerateQuad("cool", Color(1, 1, 1), 1.f);
	meshList[GEO_COOL]->textureArray[0] = LoadTGA("Image//cool.tga");
	meshList[GEO_1] = MeshBuilder::GenerateQuad("1", Color(1, 1, 1), 1.f);
	meshList[GEO_1]->textureArray[0] = LoadTGA("Image//1.tga");
	meshList[GEO_2] = MeshBuilder::GenerateQuad("2", Color(1, 1, 1), 1.f);
	meshList[GEO_2]->textureArray[0] = LoadTGA("Image//2.tga");
	meshList[GEO_3] = MeshBuilder::GenerateQuad("3", Color(1, 1, 1), 1.f);
	meshList[GEO_3]->textureArray[0] = LoadTGA("Image//3.tga");
	meshList[GEO_4] = MeshBuilder::GenerateQuad("4", Color(1, 1, 1), 1.f);
	meshList[GEO_4]->textureArray[0] = LoadTGA("Image//4.tga");
	meshList[GEO_5] = MeshBuilder::GenerateQuad("5", Color(1, 1, 1), 1.f);
	meshList[GEO_5]->textureArray[0] = LoadTGA("Image//5.tga");
	meshList[GEO_6] = MeshBuilder::GenerateQuad("6", Color(1, 1, 1), 1.f);
	meshList[GEO_6]->textureArray[0] = LoadTGA("Image//6.tga");
	meshList[GEO_7] = MeshBuilder::GenerateQuad("7", Color(1, 1, 1), 1.f);
	meshList[GEO_7]->textureArray[0] = LoadTGA("Image//7.tga");
	meshList[GEO_8] = MeshBuilder::GenerateQuad("8", Color(1, 1, 1), 1.f);
	meshList[GEO_8]->textureArray[0] = LoadTGA("Image//8.tga");
	meshList[GEO_9] = MeshBuilder::GenerateQuad("9", Color(1, 1, 1), 1.f);
	meshList[GEO_9]->textureArray[0] = LoadTGA("Image//9.tga");
	meshList[GEO_10] = MeshBuilder::GenerateQuad("10", Color(1, 1, 1), 1.f);
	meshList[GEO_10]->textureArray[0] = LoadTGA("Image//10.tga");
	meshList[GEO_11] = MeshBuilder::GenerateQuad("11", Color(1, 1, 1), 1.f);
	meshList[GEO_11]->textureArray[0] = LoadTGA("Image//11.tga");
	meshList[GEO_12] = MeshBuilder::GenerateQuad("12", Color(1, 1, 1), 1.f);
	meshList[GEO_12]->textureArray[0] = LoadTGA("Image//12.tga");

	// Scene Objects
	if (SceneManager::GetSceneID() == 1 || SceneManager::GetSceneID() == 2 || SceneManager::GetSceneID() == 3)
	{
		meshList[GEO_LIGHTBALL] = MeshBuilder::GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
		meshList[GEO_SKYDOME] = MeshBuilder::GenerateSkyPlane("skydome", Color(0.f, 0.f, 1.f), 100, 600, 2000, 2, 2);
		meshList[GEO_SKYDOME]->textureArray[0] = LoadTGA("Image//sky.tga");
		meshList[GEO_FLOOR] = MeshBuilder::GenerateCube("cube", Color(0.87, 0.87, 0.87), 1.f);
		meshList[GEO_WALL] = MeshBuilder::GenerateCube("cube", Color(0.9, 0.9, 0.9), 1.f);
		meshList[GEO_PILLAR] = MeshBuilder::GenerateCube("cube", Color(0.92, 0.92, 0.92), 1.f);

		meshList[GEO_PISTOL] = MeshBuilder::GenerateOBJ("pistol", "OBJ//pistol.obj");
		meshList[GEO_PISTOL]->textureArray[0] = LoadTGA("Image//pistol.tga");

		// Player
		meshList[GEO_HANDS] = MeshBuilder::GenerateOBJ("hands", "OBJ//hands.obj");

		// Enemy
		meshList[GEO_ENEMY_STAND] = MeshBuilder::GenerateOBJ("enemy", "OBJ//enemy_stand.obj");
		meshList[GEO_ENEMY_STAND]->textureArray[0] = LoadTGA("Image//enemy.tga");
		meshList[GEO_ENEMY_WALK01] = MeshBuilder::GenerateOBJ("enemy", "OBJ//enemy_walk01.obj");
		meshList[GEO_ENEMY_WALK01]->textureArray[0] = LoadTGA("Image//enemy.tga");
		meshList[GEO_ENEMY_WALK02] = MeshBuilder::GenerateOBJ("enemy", "OBJ//enemy_walk02.obj");
		meshList[GEO_ENEMY_WALK02]->textureArray[0] = LoadTGA("Image//enemy.tga");
		meshList[GEO_ENEMY_SHOOT02] = MeshBuilder::GenerateOBJ("enemy", "OBJ//enemy_shoot02.obj");
		meshList[GEO_ENEMY_SHOOT02]->textureArray[0] = LoadTGA("Image//enemy.tga");
	}
	else if (SceneManager::GetSceneID() == 0)
	{
		meshList[GEO_PLAY] = MeshBuilder::GenerateOBJ("menu_play", "OBJ//menu_play.obj");
		meshList[GEO_LOGO] = MeshBuilder::GenerateQuad("menu logo", Color(1, 1, 1), 1.f);
		meshList[GEO_LOGO]->textureArray[0] = LoadTGA("Image//logo.tga");
	}
}

void SceneBase::Update(double dt)
{
	m_fMTElapsedTime += dt * 0.05;

	if (Application::GetKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if (Application::GetKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if (Application::GetKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (Application::GetKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (Application::GetKeyDown('9'))
		bLightEnabled = true;
	if (Application::GetKeyDown('0'))
		bLightEnabled = false;
	if (Application::GetKeyDown(VK_OEM_PLUS))
	{
		if (cameraID != 1)
			cameraID++;
	}
	if (Application::GetKeyDown(VK_OEM_MINUS))
	{
		if (cameraID != 0)
			cameraID--;
	}
}

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight, bool enableMT, bool enableReflect)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	if (m_renderPass == RENDER_PASS_PRE) // Pass 1  - geometry pass
	{
		Mtx44 lightDepthMVP = m_lightDepthProj * m_lightDepthView * modelStack.Top();
		//Mtx44 lightDepthMVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_MVP_GPASS], 1, GL_FALSE, &lightDepthMVP.a[0]);

		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			if (mesh->textureArray[i] > 0)
			{
				glUniform1i(m_parameters[U_SHADOW_COLOR_TEXTURE_ENABLED + i], 1);
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
				glUniform1i(m_parameters[U_SHADOW_COLOR_TEXTURE + i], i);
			}
			else
				glUniform1i(m_parameters[U_SHADOW_COLOR_TEXTURE_ENABLED + i], 0);
		}
		mesh->Render();
		return;
	}

	if (enableMT)
	{
		glUniform1i(m_parameters[U_MTENABLED], 1);

		glUniform1f(m_parameters[U_ELAPSEDTIME], m_fMTElapsedTime);
	}
	else
	{
		glUniform1i(m_parameters[U_MTENABLED], 0);
	}

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);
		Mtx44 lightDepthMVP = m_lightDepthProj * m_lightDepthView * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_MVP], 1, GL_FALSE, &lightDepthMVP.a[0]);//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (!enableReflect)
	{
		glUniform1i(m_parameters[U_ENABLEREFLECT], 0);

		for (int i = 0; i < MAX_TEXTURES; ++i)
		{
			if (mesh->textureArray[i] > 0)
			{
				glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 1);
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
				glUniform1i(m_parameters[U_COLOR_TEXTURE + i], i);
			}
			else
				glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 0);

		}
	}

	if (enableReflect)
	{
		glUniform1i(m_parameters[U_ENABLEREFLECT], 1);
		glUniform3f(m_parameters[U_CAMERAPOS], camera[cameraID].position.x, camera[cameraID].position.y, camera[cameraID].position.z);
		glUniform1i(m_parameters[U_SKYDOME], 1);

		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + 1], 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[1]);
		glUniform1i(m_parameters[U_COLOR_TEXTURE + 1], 1);
	}
	mesh->Render();


	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (mesh->textureArray[i] > 0)
			glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (!mesh || mesh->textureArray[i] <= 0)
			return;

		glDisable(GL_DEPTH_TEST);
		glUniform1i(m_parameters[U_TEXT_ENABLED + i], 1);
		glUniform3fv(m_parameters[U_TEXT_COLOR + i], 1, &color.r);
		glUniform1i(m_parameters[U_LIGHTENABLED + i], 0);
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
		glUniform1i(m_parameters[U_COLOR_TEXTURE + i], 0);
	}
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureArray[0] <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureArray[0]);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMeshIn2D(Mesh *mesh, bool enableLight, float sizeX, float sizeY, float x, float y, float rotation, Vector3 rotateAxis)
{
	Mtx44 ortho;
	ortho.SetToOrtho(-80, 80, -60, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Scale(sizeX, sizeY, 0);
	modelStack.Rotate(rotation, rotateAxis.x, rotateAxis.y, rotateAxis.z);
	modelStack.Translate(x, y, 0);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (mesh->textureArray[i] > 0)
		{
			glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->textureArray[i]);
			glUniform1i(m_parameters[U_COLOR_TEXTURE + i], 0);
		}
		else
		{
			glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED + i], 0);
		}
	}
	mesh->Render();
	for (int i = 0; i < MAX_TEXTURES; ++i)
	{
		if (mesh->textureArray[i] > 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();

}

void SceneBase::RenderParticle() {

	for (auto go : ParticleEngine::GetInstance()->m_pList)
	{
		if (go->active)
		{

			switch (go->type)
			{
			case Particle::PA_NONE:
				break;
			case Particle::PA_RAINDROP:
				break;
			case Particle::PA_SPLATTER:
				break;
			case Particle::PA_SMOKE:
				break;
			case Particle::PA_SNOW:
				break;
			case Particle::PA_GUNSMOKE:
				modelStack.PushMatrix();
				modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
				modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
				RenderMesh(meshList[GEO_PA_GUNSMOKE], false, false, false);
				modelStack.PopMatrix();
				break;
			case Particle::PA_GUNSHATTER:
				modelStack.PushMatrix();
				modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
				modelStack.Rotate(go->angle, 0, 0, 1);
				modelStack.Rotate(Math::RadianToDegree(atan2(camera[1].position.x - (go->pos.x), camera[1].position.z - (go->pos.z))), 0, 1, 0);
				modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
				RenderMesh(meshList[GEO_PA_GUNSHATTER], false, false, false);
				modelStack.PopMatrix();
				break;
			case Particle::PA_BULLET:
				break;
			case Particle::PA_WALKING:
				break;
			case Particle::PA_ENEMYSHATTER:
				modelStack.PushMatrix();
				modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
				modelStack.Rotate(go->angle, 0, 0, 1);
				modelStack.Rotate(Math::RadianToDegree(atan2(camera[1].position.x - (go->pos.x), camera[1].position.z - (go->pos.z))), 0, 1, 0);
				modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
				{
				int i = rand() % 2;
				switch (i)
				{
				case 1:
				RenderMesh(meshList[GEO_PA_ENEMYSHATTER1], false, false, false);
					break;
				default:
				RenderMesh(meshList[GEO_PA_ENEMYSHATTER2], false, false, false);
					break;
				}
				}
				modelStack.PopMatrix();
				break;
			default:
				break;
			}
		}
	}
}

void SceneBase::RenderScreenText(int level, int counter)
{
	switch (level)
	{
	case 0:
		switch (counter)
		{
		case 1:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_TIME], false, 95, 50, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 98:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_SUPER], false, 125, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 99:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_COOL], false, 135, 60, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		}
		break;
	case 1:
		switch (counter)
		{
		case 1:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_GRAB], false, 95, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 2:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_THE], false, 85, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 3:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_GUN], false, 85, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 98:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_SUPER], false, 125, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 99:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_COOL], false, 135, 60, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		}
		break;
	case 2:
		switch (counter)
		{
		case 1:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_KILL], false, 85, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 2:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_THEM], false, 95, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 3:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_ALL], false, 85, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 4:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_12], false, 50, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 5:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_11], false, 50, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 6:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_10], false, 50, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 7:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_9], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 8:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_8], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 9:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_7], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 10:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_6], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 11:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_5], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 12:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_4], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 13:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_3], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 14:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_2], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 15:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_1], false, 25, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 98:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_SUPER], false, 125, 40, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		case 99:
			modelStack.PushMatrix();
			RenderMeshIn2D(meshList[GEO_COOL], false, 135, 60, 0, 0, 0, Vector3(0, 1, 0));
			modelStack.PopMatrix();
			break;
		}
		break;
	}
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	ParticleEngine::GetInstance()->Exit();

	// Cleanup VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
