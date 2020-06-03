#pragma once

class KdModel
{
public:

	KdModel();
	~KdModel();

	bool Load(const std::string& filename);

	//アクセサ
	const KdMesh* GetMesh() const { return m_pMesh; };
	const std::vector<KdMaterial>& GetMaterials() const { return m_materials; };

private:

	KdMesh* m_pMesh;

	//マテリアル配列
	std::vector<KdMaterial> m_materials;

};