#pragma once

//四角形ポリゴン管理クラス
class KdSquarePolygon
{
public:
	//四角形ポリゴンの設定
	//w:幅 h:高さ _color:色
	void Init(float w, float h, const Math::Vector4& _color);

	//ポリゴンの描画
	void Draw(int setTextureNo);

	//使用するテクスチャの設定
	inline void SetTexture(const std::shared_ptr<KdTexture>& tex)
	{
		m_texture = tex;
	}

	// アニメーション分割数の設定
	inline void SetAnimationInfo(int splitX, int splitY)
	{
		m_animSplitX = splitX;
		m_animSplitY = splitY;
	}

	// 分割設定と渡されたコマ数をもとにUV座標の計算
	void SetAnimationPos(float no);
	void Animation(float speed, bool loop);
	bool IsAnimationEnd();

private:

	// アニメーション関連
	int m_animSplitX = 1;	// 横の分割数
	int m_animSplitY = 1;	// 縦の分割数

	float m_animPos = 0;	// 現在のコマ市

	struct Vertex
	{
		Math::Vector3 pos;		//3D空間上の座標
		Math::Vector2 UV;		//テクスチャ座標(0-1)
		Math::Vector4 color;	//頂点の色(2点間は補完される)
	};

	Vertex m_vertex[4];		//四角形の情報

	std::shared_ptr<KdTexture> m_texture;	//貼り付けるテクスチャの情報
};