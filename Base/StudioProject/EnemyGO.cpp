#include "EnemyGO.h"


EnemyGO::EnemyGO()
{
}


EnemyGO::~EnemyGO()
{
	if (root != NULL)
	{
		delete root;
		root == NULL;
	}
	if (sequence1 != NULL)
	{
		delete sequence1;
		sequence1 == NULL;
	}
	if (sequence2 != NULL)
	{
		delete sequence2;
		sequence2 == NULL;
	}
	if (selector1 != NULL)
	{
		delete selector1;
		selector1 == NULL;
	}
	if (selector2 != NULL)
	{
		delete selector2;
		selector2 == NULL;
	}
	if (aiStatus != NULL)
	{
		delete aiStatus;
		aiStatus == NULL;
	}
	if (CheckRangeNode != NULL)
	{
		delete CheckRangeNode;
		CheckRangeNode == NULL;
	}
	if (ApproachPlayerNode != NULL)
	{
		delete ApproachPlayerNode;
		ApproachPlayerNode == NULL;
	}
	if (idle != NULL)
	{
		delete idle;
		idle == NULL;
	}
}

void EnemyGO::Init()
{
	aiStatus = new AIBehaviour(false, 0);
	aiStatus->state = AIBehaviour::IDLE;

	root = new AITree::Sequence;
	sequence1 = new AITree::Sequence;
	sequence2 = new AITree::Sequence;
	sequence3 = new AITree::Sequence;
	sequence4 = new AITree::Sequence;
	selector1 = new AITree::Selector;
	selector2 = new AITree::Selector;
	selector3 = new AITree::Selector;
	selector4 = new AITree::Selector;
	selector5 = new AITree::Selector;
	CheckRangeNode = new CheckPlayerInRangeTask(aiStatus);
	ApproachPlayerNode = new ApproachPlayerTask(aiStatus);
	ShootPlayerNode = new ShootPlayerTask(aiStatus);
	ifGunHeld = new CheckIfGunHeldTask(aiStatus, false);
	DetectBulletNode = new CheckOncomingBulletTask(aiStatus);
	ChasePlayerNode = new ChasePlayerTask(aiStatus);
	SearchGunNode = new SearchNeabyGunTask(aiStatus);
	idle = new IdleTask(aiStatus);

	// If player in range
	root->addChild(selector1);


	selector1->addChild(sequence1);
	sequence1->addChild(CheckRangeNode);
	sequence1->addChild(selector2);

	selector2->addChild(DetectBulletNode);
	selector2->addChild(selector3);

	selector3->addChild(ApproachPlayerNode);
	selector3->addChild(selector4);

	selector4->addChild(sequence3);
	selector4->addChild(selector5);

	sequence3->addChild(ifGunHeld);
	sequence3->addChild(ShootPlayerNode);

	selector5->addChild(sequence4);
	selector5->addChild(ChasePlayerNode);
	
	sequence4->addChild(SearchGunNode);

	//selector5->addChild(Chase);


	selector1->addChild(sequence2);
	//sequence4->addChild


	m_bdoOnce = false;
	m_fROF = Math::RandFloatMinMax(0.f, 2.f);
	
	GunOnHand = NULL;
}

bool EnemyGO::Constrain(Vector3 futurepos, Collider box, double dt)
{
	GOList &gl = GOList::GetInstance();

	box.UpdatePos(futurepos);
	//box.SetScale(Vector3( box.Half_size.y));
	for (auto go : gl.m_goList)
	{
		if (go->type == GameObject::GO_ENEMY && static_cast<EnemyGO*>(go) != this)
		{
			Collider box2 = go->obb;
			Vector3 m_v3playerPos = gl.FetchGO(GameObject::GO_PLAYER)->transform.position;
			Vector3 m_v3dir1 = (m_v3playerPos - go->transform.position).Normalized();
			box2.UpdatePos((go->transform.position+(Vector3(m_v3dir1.x, 0, m_v3dir1.z) * 20 * dt) * 20));
			if (box.GetCollision(box2))
			{
				return true;
			}
		}
		
	}
	return false;
}

void EnemyGO::Update(double dt, PlayerGO* _player)
{
	m_fROF += dt;
	obb.RotateAxis(Math::RadianToDegree(atan2(m_v3dir.x, m_v3dir.z)), Vector3(0, 1, 0));

	aiStatus->m_fdistanceToPlayer = (transform.position - _player->transform.position).Length();

	if (ApproachPlayerNode->GetApproachBool())
	{
		m_v3dir = (_player->transform.position - transform.position).Normalized();

		aiStatus->state = AIBehaviour::WALK;

		if (!Constrain((transform.position+(Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt)* 20), obb, dt))
			transform.position += Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt;
		else
			transform.position += Vector3(Vector3(-m_v3dir.x, 0, m_v3dir.z) * 20 * dt);
	}	
	if (ChasePlayerNode->GetApproachBool())
	{
		m_v3dir = (_player->transform.position - transform.position).Normalized();

		aiStatus->state = AIBehaviour::WALK;

		if (!Constrain((transform.position+(Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt)* 20), obb, dt))
			transform.position += Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt;
		else
			transform.position += Vector3(Vector3(-m_v3dir.x, 0, m_v3dir.z) * 20 * dt);
	}

	if (ShootPlayerNode->GetShootStatus() && m_fROF >= 2)
	{
		GameObject *go = gl.FetchGO();
		go->SetActive(true);
		go->SetStatic(false);
		go->m_bGravity = false;
		go->type = GameObject::GO_EBULLET;
		go->transform.position = Vector3(transform.position.x, transform.position.y + 10, transform.position.z);
		go->transform.scale.Set(0.3, 0.3, 0.3);
		go->m_v3dir = m_v3dir;
		go->m_v3vel = go->m_v3dir * 200.f;

		go->obb.pos = transform.position;
		go->obb.SetScale(Vector3(0.15, 0.15, 0.15));
		go->obb.UpdateAxis(Vector3(1, 0, 0), Vector3(0, 0, 1));
		go->obb.RotateAxis(0, Vector3(0, 1, 0));
		m_fROF = Math::RandFloatMinMax(0.f, 1.f);
	}

	if (SearchGunNode->ReturnGunFound())
	{
		m_v3dir = (SearchGunNode->ReturnPistol()->transform.position - transform.position).Normalized();

		transform.position += Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt;
	}

	if (aiStatus->m_bdodge)
	{
		int decision = rand() % 3;
		if (decision == 1)
			transform.position += DetectBulletNode->ReturnBullet()->m_v3dir.Cross(Vector3(0, 1, 0)) * 30 * dt;
		if (decision == 2)
			transform.position -= DetectBulletNode->ReturnBullet()->m_v3dir.Cross(Vector3(0, 1, 0)) * 30 * dt;
	}

	if (aiStatus->state == AIBehaviour::WALK)
	{
		m_fwalkTime += dt;

		aiStatus->m_bstartWalk01 = false;
		aiStatus->m_bstartWalk02 = true;

		//if (m_fwalkTime >= 4.f && aiStatus->m_bstartWalk01)
		//{
		//	aiStatus->m_bstartWalk01 = false;
		//	aiStatus->m_bstartWalk02 = true;
		//	m_fwalkTime = 0.f;
		//}
		//else if (m_fwalkTime >= 4.f && aiStatus->m_bstartWalk02)
		//{
		//	aiStatus->m_bstartWalk01 = true;
		//	aiStatus->m_bstartWalk02 = false;
		//	m_fwalkTime = 0.f;
		//}
	}


	//ifGunHeld->SetGunHeld(true);

	//if (approachPlayer->GetApproachBool())
	//{

	//	aiStatus->state = AIBehaviour::WALK;

	//	if (!m_bdoOnce)
	//	{
	//		aiStatus->m_bstartWalk01 = true;
	//		m_bdoOnce = true;
	//	}

	//	if (!Constrain((transform.position+(Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt)* 20), obb, dt))
	//		transform.position += Vector3(m_v3dir.x, 0, m_v3dir.z) * 20 * dt;
	//	else
	//		transform.position += Vector3(Vector3(-m_v3dir.x, 0, m_v3dir.z) * 20 * dt);
	//}
	//else
	//{
	//	aiStatus->state = AIBehaviour::IDLE;
	//}

	//if (aiStatus->m_bstartWalk01)
	//{
	//	m_fwalkTime += dt;
	//	if (m_fwalkTime >= 0.4f)
	//	{
	//		aiStatus->m_bstartWalk01 = false;
	//		aiStatus->m_bstartWalk02 = true;
	//		m_fwalkTime = 0.f;
	//	}
	//}

	//if (aiStatus->m_bstartWalk02)
	//{
	//	m_fwalkTime += dt;
	//	if (m_fwalkTime >= 0.4f)
	//	{
	//		aiStatus->m_bstartWalk02 = false;
	//		aiStatus->m_bstartWalk01 = true;
	//		m_fwalkTime = 0.f;
	//	}
	//}

	if (GunOnHand != NULL)
	{
		ifGunHeld->SetGunHeld(true);
	}

	//std::cout << aiStatus->m_fdistanceToPlayer << std::endl;

	while (!root->run())
		std::cout << "======Break======" << std::endl;
}

void EnemyGO::SetGunOnHand(PistolGO* _gunOnHand)
{
	GunOnHand = _gunOnHand;
}