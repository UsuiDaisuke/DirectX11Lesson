//==================================================
//
// 定数バッファ
//  ゲームプログラム側から持ってくるデータ
//
//==================================================

cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;
}

struct VSOutput
{
	float4 Pos		: SV_Position;		// 射影座標
	float2 UV		: TEXCOORD0;		// UV座標
	float4 Color	: TEXCOORD1;		// 色
	float3 wN		: TEXCOORD2;		// ワールド法線
	float3 wPos     : TEXCOORD3;		// ワールド3D座標
	float3 wT       : TEXCOORD4;		// ワールド接線(Tangent)
	float3 wB       : TEXCOORD5;		// ワールド従法線(Binormal)

};

//==================================================
//
// 定数バッファ(マテリアル)
//
//==================================================

struct Material
{
	float4 BaseColor; // ベース色
	float Metallic; // 金属度
	float Roughness; // 粗さ
};

cbuffer cbMaterial : register(b1)
{
	Material g_Material;
}