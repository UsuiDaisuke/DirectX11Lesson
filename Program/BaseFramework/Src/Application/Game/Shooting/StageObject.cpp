#include"StageObject.h"

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