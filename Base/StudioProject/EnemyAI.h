#ifndef ENEMY_AI_H
#define ENEMY_AI_H

#include "../Core/GameObject.h"
#include "GOList.h"
#include "AIBehaviour.h"

class EnemyAI
{
private:
	float m_fwalkTime;
	bool m_bdoOnce;
public:



	Vector3 m_v3playerPos;


	GOList &gl = GOList::GetInstance();

	GameObject* enemy;

	AITree::Sequence *root;
	AITree::Sequence *sequence1;
	AITree::Selector *selector1;
	AITree::Sequence *sequence2;
	AIBehaviour *aiStatus;
	CheckPlayerInSightTask *checkSight;
	ApproachPlayerTask *approachPlayer;
	IdleTask *idle;

	EnemyAI();
	~EnemyAI();
	void Init();
	void Update(double dt);
	void CheckPlayerInSight(GameObject* _player);

};

#endif