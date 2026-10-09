#pragma once
#include "../EffectBase.h"

// パラメーター
struct ConfettiParam
{
	float m_fallSpeed = 100.0f;									// 落下速度
	float m_fallDir = 0.0f;										// 落下方向
	float m_rotatSpeed = 20.0f;									// 回転速度
	Math::Vector3 m_rotatDir = Math::Vector3(1.0f, 0.0f, 0.0f);	// 回転方向
	Math::Color m_color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f);	// 色
	float m_startPosX = 0.0f;									// 初期X座標
	float m_scale = 1.0f;										// サイズ
};

class Confetti :public EffectBase
{
public:

	Confetti() { Init(); }
	~Confetti()override {}

	void Update()override;
	void DrawEffect()override;

	// 各パラメータのセット
	void SetConfettiParam(const ConfettiParam& param);

private:

	struct ConfettiConsts
	{
		// 消去位置
		static constexpr float ExpirePosX = 670.0f;
		static constexpr float ExpirePosY = 390.0f;

		// 初期Y座標
		static constexpr float StartPosY = 380.0f;
	};

	void Init()override;

	// パラメータ
	ConfettiParam m_param;
};