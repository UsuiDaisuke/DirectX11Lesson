#pragma once

#include "../GameProcess.h"

class ActionGameProcess : public GameProcess
{
public:
	ActionGameProcess() {}
	virtual ~ActionGameProcess() {}

	void Update() override;

};