#include"kdModel.h"
#include"KdGLTFLoader.h"

// コンストラクタ
KdModel::KdModel()
{

}


// デストラクタ
KdModel::~KdModel()
{
	Release();
	

}

const std::shared_ptr<KdAnimationData> KdModel::GetAnimation(const std::string& animName) const
{
	for (auto&& anim : m_spAnimations)
	{
		if (anim->m_name == animName)
		{
			return anim;
		}
	}
	return nullptr;
}

void KdModel::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}

// ロード関数
bool KdModel::Load(const std::string& filename)
{
	// ファイルの完全パスを取得
	std::string fileDir = KdGetDirFromPath(filename);

	// GLTFの読み込み
	std::shared_ptr<KdGLTFModel> spGltfModel = KdLoadGLTFModel(filename);
	if (spGltfModel == nullptr)
	{
		return false;
	}

	// ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	//メッシュの受け取り
	for (UINT i = 0; i < spGltfModel->Nodes.size(); i++)
	{
		// 入力元ノード
		const KdGLTFNode& rSrcNode = spGltfModel->Nodes[i];

		// 出力先ノード
		Node& rDstNode = m_originalNodes[i];

		// ノード情報のセット
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;

		// ノード内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			rDstNode.m_spMesh = std::make_shared<KdMesh>();

			if (rDstNode.m_spMesh)
			{
				//				頂点情報配列			面情報配列		サブセット情報配列
				rDstNode.m_spMesh->Create(
					rSrcNode.Mesh.Vertices, rSrcNode.Mesh.Faces, rSrcNode.Mesh.Subsets);
			}
		}
	}

	// マテリアル配列を受け取れるサイズのメモリを確保
	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); i++) 
	{
		//src = sourceの略
		//dst = destinationの略
		const KdGLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterial.Name;

		//基本色
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);

		if (!rDstMaterial.BaseColorTex)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}

		// 金属性・粗さ
		rDstMaterial.Metallic = rSrcMaterial.Metallic;
		rDstMaterial.Roughness = rSrcMaterial.Roughness;
		rDstMaterial.MetallicRoughnessTex = std::make_shared<KdTexture>();
		if (rDstMaterial.MetallicRoughnessTex->Load(fileDir + rSrcMaterial.MetallicRoughnessTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			rDstMaterial.MetallicRoughnessTex = D3D.GetWhiteTex();
		}

		// 法線マップ
		rDstMaterial.NormalTex = std::make_shared<KdTexture>();
		if (rDstMaterial.NormalTex->Load(fileDir + rSrcMaterial.NormalTexture) == false)
		{
			// 読み込めなかった場合は、代わりにZ向き法線マップを使用
			rDstMaterial.NormalTex = D3D.GetNormalTex();
		}

	}

	// アニメーションデータ
	m_spAnimations.resize(spGltfModel->Animations.size());

	for (UINT i = 0; i < m_spAnimations.size(); ++i)
	{
		const KdGLTFAnimationData& rSrcAnimation = *spGltfModel->Animations[i];

		m_spAnimations[i] = std::make_shared<KdAnimationData>();
		KdAnimationData& rDstAnimation = *(m_spAnimations[i]);

		rDstAnimation.m_name = rSrcAnimation.m_name;

		rDstAnimation.m_maxLength = rSrcAnimation.m_maxLength;

		rDstAnimation.m_nodes.resize(rSrcAnimation.m_nodes.size());

		for (UINT i = 0; i < rDstAnimation.m_nodes.size(); ++i)
		{
			rDstAnimation.m_nodes[i].m_nodeOffset = rSrcAnimation.m_nodes[i]->m_nodeOffset;
			rDstAnimation.m_nodes[i].m_translations = rSrcAnimation.m_nodes[i]->m_translations;
			rDstAnimation.m_nodes[i].m_rotations = rSrcAnimation.m_nodes[i]->m_rotations;
		}
	}

	return true;
}