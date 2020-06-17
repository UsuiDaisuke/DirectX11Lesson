#include "ModelComponent.h"

void ModelComponent::Draw()
{
	//有効じゃない時はスキップ
	if (m_enable == false) { return; }

	//モデルがないときはスキップ
	if (m_spModel == nullptr) { return; }

	SHADER.m_standardShader.SetWorldMatrix(m_owner.GetMatrix());
	SHADER.m_standardShader.DrawMesh(m_spModel->GetMesh().get(), m_spModel->GetMaterials());
}