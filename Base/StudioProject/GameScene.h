#ifndef OUR_SCENE_H
#define OUR_SCENE_H

//#include "../Core/Scene.h"
#include "SceneBase.h"

#include "Mtx44.h"
#include "../Source/Mesh.h"
#include "MatrixStack.h"
#include "../Source/Particle.h"
#include <vector>
#include "../Source/LoadHmap.h"
#include "../Source/DepthFBO.h"

#include "PlayerController.h"
#include "WeaponClass.h"
#include "GOList.h"

class GameScene : public SceneBase
{
public:
	GameScene();
	~GameScene();

	virtual void Init();
	virtual void Update(double dt);
	void RenderGO(GameObject * go);
	virtual void Render();
	virtual void Exit();


	void RenderWorld();
	void RenderWater();
	void RenderPassGPass();
	void RenderPassMain();
	//void RenderTerrain();

private:

	GOList &gl = GOList::GetInstance();

	PlayerController Player;
	WeaponClass Pistol;
};

#endif