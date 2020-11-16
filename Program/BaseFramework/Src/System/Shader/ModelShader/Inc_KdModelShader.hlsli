//==================================================
//
// �萔�o�b�t�@
//  �Q�[���v���O���������玝���Ă���f�[�^
//
//==================================================

cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW;
}

struct VSOutput
{
	float4 Pos		: SV_Position;		// �ˉe���W
	float2 UV		: TEXCOORD0;		// UV���W
	float4 Color	: TEXCOORD1;		// �F
	float3 wN		: TEXCOORD2;		// ���[���h�@��
	float3 wPos     : TEXCOORD3;		// ���[���h3D���W
	float3 wT       : TEXCOORD4;		// ���[���h�ڐ�(Tangent)
	float3 wB       : TEXCOORD5;		// ���[���h�]�@��(Binormal)

};

//==================================================
//
// �萔�o�b�t�@(�}�e���A��)
//
//==================================================

struct Material
{
	float4 BaseColor; // �x�[�X�F
	float Metallic; // �����x
	float Roughness; // �e��
};

cbuffer cbMaterial : register(b1)
{
	Material g_Material;
}