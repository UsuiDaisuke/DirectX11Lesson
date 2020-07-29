#include "KdSquarePolygon.h"

void KdSquarePolygon::Init(float w, float h, const Math::Vector4& _color)
{
	// 頂点座標の設定(幅、高さ分中央基準で離す)
	m_vertex[0].pos = { -w / 2, -h / 2, 0 };	//左下
	m_vertex[1].pos = { -w / 2,  h / 2, 0 };	//左上
	m_vertex[2].pos = {  w / 2, -h / 2, 0 };	//右下
	m_vertex[3].pos = {  w / 2,  h / 2, 0 };	//右上

	m_vertex[0].color = _color;
	m_vertex[1].color = _color;
	m_vertex[2].color = _color;
	m_vertex[3].color = _color;

	m_vertex[0].UV = { 0, 1 };	//左下
	m_vertex[1].UV = { 0, 0 };	//左上
	m_vertex[2].UV = { 1, 1 };	//右下
	m_vertex[3].UV = { 1, 0 };	//右上
}

void KdSquarePolygon::Draw(int setTextureNo)
{
	//テクスチャが設定されていたらDirectX側に教える
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(setTextureNo, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		//無ければ白いテクスチャを張り付ける
		D3D.GetDevContext()->PSSetShaderResources(setTextureNo, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	//指定した頂点配列を描画
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &m_vertex, sizeof(Vertex));
}
