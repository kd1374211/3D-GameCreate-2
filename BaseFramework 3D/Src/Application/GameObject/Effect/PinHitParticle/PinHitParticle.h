#pragma once
#include "../EffectBase.h"

struct ParticleData
{
	// 色画像
	enum ColorType { Yellow, Orange, Number }m_type = ColorType::Yellow;

	// 速度
	Math::Vector3 m_moveSpeed = Math::Vector3::Zero;
};

class PinHitParticle :public EffectBase
{
public:

	PinHitParticle() { Init(); }
	~PinHitParticle()override {}

	void Update()override;
	void PreDraw()override;
	void DrawEffect()override {};
	void DrawBright()override;

	// データセット
	void SetParticleData(ParticleData data) { m_ptData = data; }

private:

	// クラス内定数
	struct PinHitParticleConsts
	{
		// 透明化速度
		static constexpr float AlphaChangeSpeed = -0.9f;
	};

	void Init()override;

	// 透明度
	float m_alpha = 1.0f;

	// パーティクルデータ
	ParticleData m_ptData = {};
};