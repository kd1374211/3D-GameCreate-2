#pragma once
#include "../EffectBase.h"

class PinHit :public EffectBase
{
public:

	PinHit() { Init(); }
	~PinHit()override {}

	void Update()override;
	void DrawEffect()override;

private:

	// クラス内定数
	struct PinHitConsts
	{
		// 透明化速度
		static constexpr float AlphaChangeSpeed = -0.9f;
	};

	void Init()override;

	// 透明度
	float m_alpha = 1.0f;
};