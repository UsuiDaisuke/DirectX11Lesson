#include"KdCollision.h"

using namespace DirectX;

bool KdRayToMesh(
	const XMVECTOR& rRayPos, 
	const XMVECTOR& rRayDir, 
	float maxDistance, 
	const KdMesh& rMesh, 
	const KdMatrix& rMatrix,
	KdRayResult& rResult
)
{
	//モデルの逆行列でレイを変換
	XMMATRIX invMat = XMMatrixInverse(0, rMatrix);

	//レイの判定開始位置を逆変換
	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos, invMat);

	// 発射方向は正規化されていないと正しく判定できないので正規化
	XMVECTOR rayDir = XMVector3TransformNormal(rRayDir, invMat);

	//逆行列に拡縮が入っていると
	//レイが当たった距離にも拡縮を反映されてしまうので
	//判定用の最大距離にも拡縮を反映させておく

	float dirLength = XMVector3Length(rayDir).m128_f32[0];
	float rayCheckRange = maxDistance * dirLength;

	rayDir = XMVector3Normalize(rayDir);

	//--------------------------------------------------
	// ブロードフェイズ
	//  比較的軽量なAABB vs レイな判定で、
	//  あきらかにヒットしない場合は、これ以降の判定を中止
	//--------------------------------------------------
	{
		float AABBdist = FLT_MAX;
		if (rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		if (AABBdist > rayCheckRange) { return false; }
	}

	//--------------------------------------------------
	// ナローフェイズ
	//  レイ vs 全ての面
	//--------------------------------------------------

	bool ret = false;				//当たったかどうか
	float closestDist = FLT_MAX;	//当たった面との距離

	// 面情報の取得
	const KdMeshFace* pFaces = &rMesh.GetFaces()[0];
	UINT faceNum = rMesh.GetFaces().size();

	// すべての面(三角形)と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構成する3つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// レイと三角形の当たり判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects(
			rayPos,			// 発射場所
			rayDir,			// 発射方向

			// 判定する三角形の頂点情報
			rMesh.GetVertexPositions()[idx[0]],
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],

			triDist	// 当たった場合の距離
		);

		// ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		// 最大距離以内か
		if (triDist <= rayCheckRange)
		{
			ret = true;//当たったとする

			triDist /= dirLength;

			if (triDist < closestDist)
			{
				closestDist = triDist;
			}
		}
	}

	rResult.m_distance = closestDist;
	rResult.m_hit = ret;
	return ret;
}