#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	
}

void GameObject::Deserialize()
{

}

void GameObject::Update()
{

}

void GameObject::Draw()
{
	if (m_pModel == nullptr)
	{
		return;
	}

	SHADER.m_standardShader.SetWorldMatrix(m_mWorld);
	SHADER.m_standardShader.DrawMesh(m_pModel->GetMesh(), m_pModel->GetMaterials());
}

void GameObject::Release()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}
}