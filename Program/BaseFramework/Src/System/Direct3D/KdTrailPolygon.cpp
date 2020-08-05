﻿#include "KdTrailPolygon.h"

void KdTrailPolygon::Draw(float width)
{
	// ポイントが2つ以下の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	// 軌道画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点行列
	std::vector<Vertex> vertex;
	// ポイント数分のサイズ確保
	vertex.resize(m_pointList.size() * 2);

	//------------------------------
	// 頂点データ作成
	//------------------------------
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		Math::Matrix& mat = m_pointList[i];

		// 2つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		// X方向
		Math::Vector3 axisX = mat.Right();
		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		float uvY = i / sliceCount;
		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };
	}

	// テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}

void KdTrailPolygon::DrawBillboard(float width)
{
	// ポイントが2つ以下の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	// カメラの情報
	Math::Matrix mCam = SHADER.m_cb7_Camera.GetWork().mV.Invert();

	// 軌道画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点行列
	std::vector<Vertex> vertex;
	// ポイント数分のサイズ確保
	vertex.resize(m_pointList.size() * 2);

	//------------------------------
	// 頂点データ作成
	//------------------------------
	Math::Vector3 prevPos;
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		Math::Matrix& mat = m_pointList[i];

		// 2つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		// ラインの向き
		Math::Vector3 vDir;
		if (i == 0)
		{
			// 初回時のみ、次のポイントを使用
			vDir = m_pointList[1].Translation() - mat.Translation();
		}
		else
		{
			vDir = mat.Translation() - prevPos;
		}

		Math::Vector3 v = mat.Translation() - mCam.Translation();
		Math::Vector3 axisX = DirectX::XMVector3Cross(vDir, v);
		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		float uvY = i / sliceCount;
		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };

		// 座標を記憶しておく
		prevPos = mat.Translation();
	}

	// テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}