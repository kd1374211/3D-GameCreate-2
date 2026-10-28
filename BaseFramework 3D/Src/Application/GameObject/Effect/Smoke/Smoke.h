#pragma once
#include "../EffectBase.h"

class Smoke :public EffectBase
{
public:

	Smoke() { Init(); }
	~Smoke()override {}

	void Update()override;
	void PreDraw()override;
	void DrawEffect()override;

private:

	// クラス内定数
	struct SmokeConsts
	{
		static constexpr float MoveSpeedY = 0.1f;
	};

	void Init()override;

};
