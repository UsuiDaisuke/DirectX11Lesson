﻿#include"ShootingGameProcess.h"
#include"../Scene.h"

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
}