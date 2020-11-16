#pragma once

//========================================
//
// モデルシェーダー
//
//========================================

class KdModelShader
{
public:

	//==============================
	// 設定・取得
	//==============================

	// ワールド行列セット
	void SetWorldMatrix(const Math::Matrix& m)
	{
		m_cb0.Work().mW = m;
	}

	//==============================
	// 描画
	//==============================

	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	// mesh			… 描画するメッシュ
	// materials	… 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const std::vector<KdMaterial>& materials);

	//==============================
	// 初期化・解放
	//==============================

	// 初期化
	bool Init();
	// 解放
	void Release();

	~KdModelShader()
	{
		Release();
	}

private:

	// 3Dモデル用シェーダ
	ID3D11VertexShader* m_VS = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;

	ID3D11PixelShader* m_PS = nullptr;

	// 定数バッファ(オブジェクト単位の更新)
	struct cbObject
	{
		Math::Matrix mW;
	};
	KdConstantBuffer<cbObject>m_cb0;

	// 定数バッファ(マテリアル単位の更新)
	struct cbMaterial {
		Math::Vector4	    BaseColor;
		float			Metallic;
		float			Roughness;
		float			tmp[2];
	};

	KdConstantBuffer<cbMaterial>m_cb1_Material;
};