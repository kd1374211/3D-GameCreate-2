#pragma once
#include "../EffectBase.h"

struct ParticleData
{
	// 色画像
	enum ColorType { Yellow, Orange, Number }m_type = ColorType::Yellow;

	// ローカル座標
	Math::Vector3 m_localPos = Math::Vector3::Zero;
	
	// 速度
	Math::Vector3 m_moveSpeed = Math::Vector3::Zero;
};

class PinHit :public EffectBase
{
public:

	PinHit() { Init(); }
	~PinHit()override {}

	void Update()override;
	void PreDraw()override;
	void DrawEffect()override;

private:

	// クラス内定数
	struct PinHitConsts
	{
		// 透明化速度
		static constexpr float AlphaChangeSpeed = -0.9f;

		// 拡大
		static constexpr float ScaleMultiChangeSpeed = 0.7f;

		// パーティクル召喚数
		static constexpr int HitParticleSpawnMin = 9;
		static constexpr int HitParticleSpawnMax = 17;

		// パーティクルの速度
		static constexpr float HitParticleSpeedMin = 0.75f;
		static constexpr float HitParticleSpeedMax = 2.5f;

		// パーティクルスケール
		static constexpr float HitParticleScale = 0.03f;
	};

	void Init()override;

	// 透明度
	float m_alpha = 1.0f;

	// パーティクル個々
	std::vector<ParticleData> m_particles;
	std::shared_ptr<KdSquarePolygon> m_particleColorTex = nullptr;
};