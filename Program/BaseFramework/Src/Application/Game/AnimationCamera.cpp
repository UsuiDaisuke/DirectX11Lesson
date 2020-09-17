#include "AnimationCamera.h"

#include "Scene.h"
#include "../Component/CameraComponent.h"

void AnimationCamera::Update()
{
	auto start = m_wpStart.lock();
	auto end = m_wpEnd.lock();
	if (!start || !end)
	{
		m_alive = false;
		return;
	}

	auto& mStart = start->GetCameraComponent()->GetCameraMatrix();
	auto& mEnd = end->GetCameraComponent()->GetCameraMatrix();

	// 座標の補完
	const KdVec3& vStart = mStart.GetTranslation();
	const KdVec3& vEnd = mEnd.GetTranslation();

	KdVec3 vTo = vEnd - vStart;
	KdVec3 vNow = vStart + vTo * m_progress;

	// 回転の補完
	DirectX::XMVECTOR qSt = DirectX::XMQuaternionRotationMatrix(mStart);
	DirectX::XMVECTOR qEd = DirectX::XMQuaternionRotationMatrix(mEnd);

	DirectX::XMVECTOR qOTW = DirectX::XMQuaternionSlerp(qSt, qEd, m_progress);

	KdMatrix mRot = DirectX::XMMatrixRotationQuaternion(qOTW);

	KdMatrix mCam = mRot;
	mCam.SetTranslation(vNow);

	m_spCameraComponent->SetCameraMatrix(mCam);

	m_progress += m_speed;
	if (m_progress > 1.0f)
	{
		m_progress = 1.0f;
		m_alive = false;
		Scene::GetInstance().SetTargetCamera(end->GetCameraComponent());
	}
}
