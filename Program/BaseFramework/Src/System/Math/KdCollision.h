
//レイによる当たり判定
bool KdRayToMesh(
	const DirectX::XMVECTOR& rRayPos, 
	const DirectX::XMVECTOR& rRayDir,
	float maxDistance, 
	const KdMesh& rMesh, 
	const KdMatrix& rMatrix);