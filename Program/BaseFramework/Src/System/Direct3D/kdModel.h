#pragma once

struct KdAnimationData;

class KdModel
{
public:

	KdModel();
	~KdModel();

	bool Load(const std::string& filename);

	//アクセサ
	const std::shared_ptr <KdMesh> GetMesh(UINT index) const 
	{
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr;
	};
	const std::vector<KdMaterial>& GetMaterials() const { return m_materials; };

	struct Node
	{
		std::string	m_name;
		KdMatrix	m_localTransform;
		std::shared_ptr<KdMesh>	m_spMesh;
	};

	inline Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_originalNodes)
		{
			if (node.m_name == name) { return &node; }
		}
		return nullptr;
	}

	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }

	const std::shared_ptr<KdAnimationData> GetAnimation(const std::string& animName) const;

private:

	void Release();							// 解放
	std::vector<Node>	m_originalNodes;	// データのノード配列

	//マテリアル配列
	std::vector<KdMaterial> m_materials;

	// アニメーションデータリスト
	std::vector<std::shared_ptr<KdAnimationData>>	m_spAnimations;
};