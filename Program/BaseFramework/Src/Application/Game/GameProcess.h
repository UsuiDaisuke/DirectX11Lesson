#pragma once

#include "GameObject.h"

// ゲームシーンの管理者をカテゴリ分けするために、GameProcessにGameObjectを継承する
class GameProcess : public GameObject
{
public:
	GameProcess() {}
	virtual ~GameProcess() {}

};