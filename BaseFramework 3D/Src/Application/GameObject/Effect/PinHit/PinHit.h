#pragma once
#include "../EffectBase.h"

class PinHit :public EffectBase
{
public:

	PinHit() { Init(); }
	~PinHit()override {}

private:

	void Init()override;

};