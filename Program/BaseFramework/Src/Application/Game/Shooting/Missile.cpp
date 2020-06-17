#include"Missile.h"

#include"Application/main.h"
#include"../../Component/ModelComponent.h"

void Missile::Deserialize()
{
	m_spModelComponent->SetModel(KdResFac.GetModel("Data/Missile/Missile.gltf"));

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