#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

// �e�N�X�`��(�摜�f�[�^ �����̂܂܂ł͎g���Ȃ�)
Texture2D g_baseTex : register(t0);         // �x�[�X�J���[�e�N�X�`��
Texture2D g_mrTex : register(t1);            // ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTex : register(t2);        // �@���}�b�v

// �T���v��(�e�N�X�`���̃f�[�^����������)
SamplerState g_ss : register(s0);

// BlinnPhong NDF
// �ElightDir    �c ���C�g�̕���
// �EvCam        �c �s�N�Z������J�����ւ̕���
// �Enormal      �c �@��
// �EspecPower   �c ���˂̉s��
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // �J�����̊p�x��(0�`1)
	float spec = pow(NdotH, specPower);

	// ���K��Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//==============================
// �s�N�Z���V�F�[�_
//==============================

float4 main(VSOutput In) : SV_Target0
{
	// �J�����ւ̕���
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);       // �J���� - �s�N�Z������
	vCam = normalize(vCam);

	// �R��̖@������@���s����쐬
	row_major float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN),
	};

	// �@���}�b�v����@���x�N�g���擾
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;

	// 0�`1�@���� -1�`1�@�֕ϊ�
	wN = wN * 2.0 - 1.0;
	// �@���x�N�g�������̃s�N�Z����Ԃ֕ϊ�
	wN = normalize(mul(wN, mTBN));

	//------------------------------------------
	// �ގ��F
	//------------------------------------------
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_Material.BaseColor * In.Color;

	float4 mr = g_mrTex.Sample(g_ss, In.UV);
	// ������
	float metallic = mr.b * g_Material.Metallic;

	// �e��
	float roughness = mr.g * g_Material.Roughness;

	// ���t�l�X����A���ˌ��p��SpecularPower�����߂�
	// Call of Duty : Black Ops��FarCry3�ł̎�@���Q�l
	// specPower = 2^(13*g)�@�O���X�lg = 0�`1��1�`8192�̃X�y�L�������x�����߂�
	// �����ł� specPower = 2^(11*g)�@1�`2048���g�p���܂��B
	// �Q�l�Fhttps://hanecci.hatenadiary.org/entry/20130511/p1
	float smoothness = 1.0 - roughness; // ���t�l�X���t�]�����u���炩�v���ɂ���
	float specPower = pow(2, 11 * smoothness); // 1�`2048


	//------------------------------------------
	// ���C�e�B���O
	//------------------------------------------
	// �ŏI�I�ȐF
	float3 color = 0;

	// �ގ��̊g�U�F�@������قǍގ��̐F�ɂȂ�A�����قǊg�U�F�͖����Ȃ�
	const float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
	// �ގ��̔��ːF�@������قǌ��̐F�����̂܂ܔ��˂��A�����قǍގ��̐F�����
	const float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);


	//-------------------------
	// ���s��
	//-------------------------

	// Diffuse(�g�U��) 
	{
		// ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
		float lightDiffuse = dot(-g_DL_Dir, wN);
		lightDiffuse = saturate(lightDiffuse);  // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�

		// ���K��Lambert
		lightDiffuse /= 3.1415926535;

		// ���̐F * �ގ��̊g�U�F * ������
		color += (g_DL_Color * lightDiffuse) * baseDiffuse * baseColor.a;
	}

	// Specular(���ːF) 
	{
		// ���˂������̋��������߂�
		// Blinn-Phong NDF
		float spec = BlinnPhong(g_DL_Dir, vCam, wN, specPower);

		// ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������ * �K���Ȓ����l
		color += (g_DL_Color * spec) * baseSpecular * baseColor.a;
	}

	return float4(color, baseColor.a);
}