#pragma once

#include "../Game/GameObject.h"

//========================================
//  モデルコンポーネント
//========================================
class ModelComponent
{
public:

	ModelComponent(GameObject& owner) : m_owner(owner) {}

	//有効フラグ
	bool isEnable()const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

	//モデル取得
	// inline const std::shared_ptr<KdModel> GetModel() const { return m_spModel; }

	//メッシュ取得
	inline const std::shared_ptr<KdMesh> GetMesh(UINT index) const
	{
		if (index >= m_coppiedNodes.size()) { return nullptr; }
		return m_coppiedNodes[index].m_spMesh;
	}

	inline KdModel::Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_coppiedNodes)
		{
			if (node.m_name == name) 
			{ 
				return &node;
			}
		}
		return nullptr;
	}

	const std::shared_ptr<KdAnimationData> GetAnimation(const std::string& animName)const
	{
		if (!m_spModel) { return nullptr; }
		return m_spModel->GetAnimation(animName);
	}

	const std::vector<KdModel::Node>& GetNodes() const { return m_coppiedNodes; }
	std::vector<KdModel::Node>& GetChangeableNodes(){ return m_coppiedNodes; }

	//モデルセット
	void SetModel(const std::shared_ptr<KdModel> model);

	//StandardShaderで描画
	void Draw();

private:

	//個別管理のため、オリジナルからコピーして保持する入れ物
	std::vector<KdModel::Node> m_coppiedNodes;

	//有効
	bool m_enable = true;

	//モデルデータの参照
	std::shared_ptr<KdModel> m_spModel;

	GameObject& m_owner;

};