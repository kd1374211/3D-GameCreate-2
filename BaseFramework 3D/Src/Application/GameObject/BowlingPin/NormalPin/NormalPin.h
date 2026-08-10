#pragma once
#include "../PinBase/PinBase.h"

class NormalPin :public PinBase
{
public:

	NormalPin(Math::Vector3 pos, Math::Quaternion rotat);
	~NormalPin()override {}

private:
};