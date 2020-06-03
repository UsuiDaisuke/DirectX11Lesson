#include"Missile.h"

#include"Application/main.h"

Missile::Missile()
{

}

Missile::~Missile()
{
	Release();
}

void Missile::Deserialize()
{
	m_pModel = new KdModel();
	if (m_pModel == nullptr)
	{
		return;
	}

	if (m_pModel->Load("Data/Missile/Missile.gltf") == false)
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

void Missile::Draw()
{
	if (m_alive == false)
	{
		return;
	}

	if (m_pModel)
	{
		SHADER.m_standardShader.SetWorldMatrix(m_mWorld);
		SHADER.m_standardShader.DrawMesh(m_pModel->GetMesh(), m_pModel->GetMaterials());
	}
}

void Missile::Release()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}
}