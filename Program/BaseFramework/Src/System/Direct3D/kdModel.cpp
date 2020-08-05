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
		// src = source略
		// dst = destinationの略
		const KdGLTFMaterial& rSrcMaterial = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		// 名前
		rDstMaterial.Name = rSrcMaterial.Name;

		// 基本色
		rDstMaterial.BaseColor = rSrcMaterial.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		//テクスチャの読み込み
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterial.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}

	return true;
}