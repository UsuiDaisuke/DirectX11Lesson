#pragma once

// アニメーションキー(クォータニオン)
struct KdAnimKeyQuaternion
{
	float			m_time = 0;		// 時間
	KdQuaternion	m_quat;			// クォータニオンデータ
};

// アニメーションキー(ベクトル)
struct KdAnimKeyVector3
{
	float			m_time = 0;		// 時間
	KdVec3			m_vec;			// 3Dベクトルデータ
};

//==============================
//	アニメーションデータ
//==============================
struct KdAnimationData
{
	std::string		m_name;
	float			m_maxLength = 0;

	struct Node
	{
		bool InterpolateTranslations(KdVec3& result, float time);
		bool InterpolateRotations(KdQuaternion& result, float time);

		int		m_nodeOffset = -1;

		std::vector<KdAnimKeyVector3>	m_translations;
		std::vector<KdAnimKeyQuaternion>	m_rotations;
	};

	std::vector<Node> m_nodes;
};