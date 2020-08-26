#include"ActionGameProcess.h"
#include"../Scene.h"

void ActionGameProcess::Update()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (m_canChangeScene)
		{
			Scene::GetInstance().RequestChangeScene("Data/Scene/ShootingGame.json");
		}
		m_canChangeScene = false;
	}
	else {
		m_canChangeScene = true;
	}
}