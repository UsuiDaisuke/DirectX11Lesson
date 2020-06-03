#include"StageObject.h"

StageObject::StageObject()
{

}

StageObject::~StageObject()
{
	Release();
}

void StageObject::Deserialize()
{
	m_pModel = new KdModel();

	if (m_pModel == nullptr)
	{
		return;
	}

	if (m_pModel->Load("Data/StageMap/StageMap.gltf") == false)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}
}

void StageObject::Update()
{

}

void StageObject::Draw()
{
	if (m_pModel == nullptr)
	{
		return;
	}

	SHADER.m_standardShader.SetWorldMatrix(m_mWorld);

	SHADER.m_standardShader.DrawMesh(m_pModel->GetMesh(), m_pModel->GetMaterials());
}

void StageObject::Release()
{
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = nullptr;
	}
}