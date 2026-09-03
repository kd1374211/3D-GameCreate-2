#pragma once
#include "../PinBase/PinBase.h"

class NormalPin :public PinBase
{
public:

	NormalPin(Math::Vector3 startPos, Math::Quaternion startRot);
	~NormalPin()override {}

private:

	// このクラスの定数
	struct NormalPinConsts
	{
		// 重さ
		static constexpr float PinMass = 1.6f;
	};

};