#pragma once
#include "Actor.h"
class HudUI : public Actor
{
public:
	HudUI();
	virtual ~HudUI();

public:
	virtual void Update() override;
};

