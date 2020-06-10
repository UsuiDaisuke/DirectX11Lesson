#include"Missile.h"

#include"Application/main.h"

void Missile::Deserialize()
{
	m_spModel = std::make_shared<KdModel>();
	if (m_spModel == nullptr)
	{
		return;
	}

	if (m_spModel->Load("Data/Missile/Missile.gltf") == false)
	{
		Release();
	}

	m_lifeSpan = APP.m_maxFps * 10;
}

void Missile::Update()
{
	if (m_alive == false)
	{
		return;
	}

	if (--m_lifeSpan <= 0)
	{
		Destroy();
	}

	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= m_speed;

	m_mWorld.Move(move);
}