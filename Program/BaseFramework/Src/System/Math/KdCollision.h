#pragma once

struct KdRayResult
{
	float	m_distance = FLT_MAX;
	bool	m_hit = false;
	KdVec3 rHitPos = {};
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

bool KdSphereToMesh(
	const Math::Vector3& rSpherePos,
	float radius,
	const KdMesh& mesh,
	const DirectX::XMMATRIX& matrix,
	Math::Vector3& rPushedPos
);

// 点 vs 三角形との最近接点を求める
//	p		… 点の座標
//	a		… 三角形の点１
//	b		… 三角形の点２
//	c		… 三角形の点３
//	outPt	… 最近接点の座標が入る
void KdPointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a,
	const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt);