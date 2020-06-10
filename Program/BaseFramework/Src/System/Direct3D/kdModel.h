#pragma once

class KdModel
{
public:

	KdModel();
	~KdModel();

	bool Load(const std::string& filename);

	//アクセサ
	const std::shared_ptr <KdMesh> GetMesh() const { return m_spMesh; };
	const std::vector<KdMaterial>& GetMaterials() const { return m_materials; };

private:

	std::shared_ptr<KdMesh> m_spMesh;

	//マテリアル配列
	std::vector<KdMaterial> m_materials;

};