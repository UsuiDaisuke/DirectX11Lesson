#pragma once

struct KdRayResult
{
	float	m_distance = -1;
	bool	m_hit = false;
};

//レイによる当たり判定
bool KdRayToMesh(
	const DirectX::XMVECTOR& rRayPos, 
	const DirectX::XMVECTOR& rRayDir,
	float maxDistance, 
	const KdMesh& rMesh, 
	const KdMatrix& rMatrix,

	KdRayResult& rResult
);