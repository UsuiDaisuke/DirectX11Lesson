#include"ShootingGameProcess.h"
#include"../Scene.h"

#include"../AnimationCamera.h"

void ShootingGameProcess::Update()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_canChangeScene)
		{
			// キャラクターを回している最中のロードは禁止！
			// Scene::GetInstance().LoadScene("Data/Scene/ActionGame.json");

			Scene::GetInstance().RequestChangeScene("Data/Scene/ActionGame.json");
		}
		m_canChangeScene = false;
	}
	else {
		m_canChangeScene = true;
	}

	if (GetAsyncKeyState('C') & 0x8000)
	{
		ChangeCamTarget();
	}
}

void ShootingGameProcess::ChangeCamTarget()
{
	auto animCam = Scene::GetInstance().FindObjectWithName("AnimationCamera");
	if (animCam) { return; }

	auto now = Scene::GetInstance().FindObjectWithName(m_strCameraTarget);
	if (!now) { return; }

	// 新しいカメラターゲットの名前を設定
	if (m_strCameraTarget == "Player")
	{
		m_strCameraTarget = "Enemy00";
	}
	else
	{
		m_strCameraTarget = "Player";
	}

	auto target = Scene::GetInstance().FindObjectWithName(m_strCameraTarget);
	if (!target) { return; }

	auto newAnimCam = std::make_shared<AnimationCamera>();
	newAnimCam->Set(now, target);

	Scene::GetInstance().SetTargetCamera(newAnimCam->GetCameraComponent());
	Scene::GetInstance().AddObject(newAnimCam);
}
