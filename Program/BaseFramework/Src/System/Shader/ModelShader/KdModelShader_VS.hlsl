#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

VSOutput main(float4 pos : POSITION,	// ���_���W
			float2 uv : TEXCOORD0,		// �e�N�X�`��UV���W
			float3 normal : NORMAL,		// �@���x�N�g�����W
			float4 color : COLOR,
			float3 tangent : TANGENT    // �ڐ�
)
{
	VSOutput Out;

    // ���W�ϊ�
    Out.Pos = mul(pos, g_mW); // ���[�J�����W�n -> ���[���h���W�n�֕ϊ�
    Out.wPos = Out.Pos.xyz;	// ���[���h���W���o��
    Out.Pos = mul(Out.Pos, g_mV); // ���[���h���W�n -> �r���[���W�n�֕ϊ�
    Out.Pos = mul(Out.Pos, g_mP); // �r���[���W�n -> �ˉe���W�n�֕ϊ�

    // ���_�F
    Out.Color = color;

    // �@��
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    // �ڐ�
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    // �]�ڐ�
    float3 binormal = cross(normal, tangent);
    Out.wB = normalize(mul(binormal, (float3x3)g_mW));

	// UV���W
	Out.UV = uv;

	return Out;
}