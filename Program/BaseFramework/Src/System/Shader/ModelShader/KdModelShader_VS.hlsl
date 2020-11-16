#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

VSOutput main(float4 pos : POSITION,	// 頂点座標
			float2 uv : TEXCOORD0,		// テクスチャUV座標
			float3 normal : NORMAL,		// 法線ベクトル座標
			float4 color : COLOR,
			float3 tangent : TANGENT    // 接線
)
{
	VSOutput Out;

    // 座標変換
    Out.Pos = mul(pos, g_mW); // ローカル座標系 -> ワールド座標系へ変換
    Out.wPos = Out.Pos.xyz;	// ワールド座標を出力
    Out.Pos = mul(Out.Pos, g_mV); // ワールド座標系 -> ビュー座標系へ変換
    Out.Pos = mul(Out.Pos, g_mP); // ビュー座標系 -> 射影座標系へ変換

    // 頂点色
    Out.Color = color;

    // 法線
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    // 接線
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    // 従接線
    float3 binormal = cross(normal, tangent);
    Out.wB = normalize(mul(binormal, (float3x3)g_mW));

	// UV座標
	Out.UV = uv;

	return Out;
}