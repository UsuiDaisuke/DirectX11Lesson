#pragma once

#include "../GameProcess.h"

class ShootingGameProcess : public GameProcess
{
public:
	ShootingGameProcess() {}
	virtual ~ShootingGameProcess() {}

	void Update() override;

};