#pragma once
#include "../EffectBase.h"

class Landing :public EffectBase
{
public:

	Landing() { Init(); }
	~Landing()override {}

	void Update()override;
	void DrawEffect()override;

private:

	// クラス内定数
	struct LandingConsts
	{
		// 透明化速度
		static constexpr float AlphaChangeSpeed = -0.9f;
	};

	void Init()override;

	// 透明度
	float m_alpha = 1.0f;
};