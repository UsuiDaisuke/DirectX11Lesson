#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

// テクスチャ(画像データ ※そのままでは使えない)
Texture2D g_baseTex : register(t0);         // ベースカラーテクスチャ
Texture2D g_mrTex : register(t1);            // メタリック/ラフネステクスチャ
Texture2D g_normalTex : register(t2);        // 法線マップ

// サンプラ(テクスチャのデータを扱うもの)
SamplerState g_ss : register(s0);

// BlinnPhong NDF
// ・lightDir    … ライトの方向
// ・vCam        … ピクセルからカメラへの方向
// ・normal      … 法線
// ・specPower   … 反射の鋭さ
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // カメラの角度差(0～1)
	float spec = pow(NdotH, specPower);

	// 正規化Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//==============================
// ピクセルシェーダ
//==============================

float4 main(VSOutput In) : SV_Target0
{
	// カメラへの方向
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);       // カメラ - ピクセル距離
	vCam = normalize(vCam);

	// ３種の法線から法線行列を作成
	row_major float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN),
	};

	// 法線マップから法線ベクトル取得
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;

	// 0～1　から -1～1　へ変換
	wN = wN * 2.0 - 1.0;
	// 法線ベクトルをこのピクセル空間へ変換
	wN = normalize(mul(wN, mTBN));

	//------------------------------------------
	// 材質色
	//------------------------------------------
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_Material.BaseColor * In.Color;

	float4 mr = g_mrTex.Sample(g_ss, In.UV);
	// 金属性
	float metallic = mr.b * g_Material.Metallic;

	// 粗さ
	float roughness = mr.g * g_Material.Roughness;

	// ラフネスから、反射光用のSpecularPowerを求める
	// Call of Duty : Black OpsやFarCry3での手法を参考
	// specPower = 2^(13*g)　グロス値g = 0～1で1～8192のスペキュラ強度を求める
	// ここでは specPower = 2^(11*g)　1～2048を使用します。
	// 参考：https://hanecci.hatenadiary.org/entry/20130511/p1
	float smoothness = 1.0 - roughness; // ラフネスを逆転させ「滑らか」さにする
	float specPower = pow(2, 11 * smoothness); // 1～2048


	//------------------------------------------
	// ライティング
	//------------------------------------------
	// 最終的な色
	float3 color = 0;

	// 材質の拡散色　非金属ほど材質の色になり、金属ほど拡散色は無くなる
	const float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
	// 材質の反射色　非金属ほど光の色をそのまま反射し、金属ほど材質の色が乗る
	const float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);


	//-------------------------
	// 平行光
	//-------------------------

	// Diffuse(拡散光) 
	{
		// 光の方向と法線の方向との角度差が光の強さになる
		float lightDiffuse = dot(-g_DL_Dir, wN);
		lightDiffuse = saturate(lightDiffuse);  // マイナス値は0にする　0(暗)～1(明)になる

		// 正規化Lambert
		lightDiffuse /= 3.1415926535;

		// 光の色 * 材質の拡散色 * 透明率
		color += (g_DL_Color * lightDiffuse) * baseDiffuse * baseColor.a;
	}

	// Specular(反射色) 
	{
		// 反射した光の強さを求める
		// Blinn-Phong NDF
		float spec = BlinnPhong(g_DL_Dir, vCam, wN, specPower);

		// 光の色 * 反射光の強さ * 材質の反射色 * 透明率 * 適当な調整値
		color += (g_DL_Color * spec) * baseSpecular * baseColor.a;
	}

	return float4(color, baseColor.a);
}