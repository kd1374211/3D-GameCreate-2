#pragma once
#include "../EffectBase.h"

class PinHit :public EffectBase
{
public:

	PinHit() { Init(); }
	~PinHit()override {}

	void PreDraw()override;
	void DrawEffect()override {};
	void DrawBright()override;

	// セット後に行う処理
	void PostSetInit()override;

private:

	// クラス内定数
	struct PinHitConsts
	{
		// パーティクル召喚数
		static constexpr int HitParticleSpawnMin = 15;
		static constexpr int HitParticleSpawnMax = 30;

		// パーティクルの速度
		static constexpr float HitParticleSpeedMin = 1.5f;
		static constexpr float HitParticleSpeedMax = 15.0f;

		// パーティクルスケール
		static constexpr int HitParticleScaleRandNum = 3;
		static constexpr float HitParticleScale[HitParticleScaleRandNum] = { 0.5f,0.75f,1.0f };
	};

	void Init()override;
};