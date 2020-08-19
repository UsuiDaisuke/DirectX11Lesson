#include "ModelComponent.h"

void ModelComponent::SetModel(const std::shared_ptr<KdModel> model)
{
	m_spModel = model;

	m_coppiedNodes.clear();

	if (model)
	{
		m_coppiedNodes = model->GetOriginalNodes();
	}
}

void ModelComponent::Draw()
{
	//有効じゃない時はスキップ
	if (m_enable == false) { return; }

	//モデルがないときはスキップ
	if (m_spModel == nullptr) { return; }

	for (UINT i = 0; i < m_coppiedNodes.size(); i++)
	{
		auto& rNode = m_coppiedNodes[i];
		if (rNode.m_spMesh == nullptr) { continue; }
		SHADER.m_standardShader.SetWorldMatrix(rNode.m_localTransform * m_owner.GetMatrix());
		SHADER.m_standardShader.DrawMesh(rNode.m_spMesh.get(), m_spModel->GetMaterials());
	}
}