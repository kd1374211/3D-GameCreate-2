#pragma once
#include "../EffectBase.h"

// パラメーター
struct ConfettiParam
{
	float m_fallSpeed = 100.0f;									// 落下速度
	float m_fallDir = 0.0f;										// 落下方向
	float m_rotatSpeed = 20.0f;									// 回転速度
	Math::Color m_color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f);	// 色
	Math::Vector3 m_startPos = Math::Vector3::Zero;				// 初期座標
	float m_scale = 1.0f;										// サイズ
};

class Confetti :public EffectBase
{
public:

	Confetti() { Init(); }
	~Confetti()override {}

	void Update()override;
	void PreDraw()override;
	void DrawSprite()override;

	// 各パラメータのセット
	void SetConfettiParam(const ConfettiParam& param);

private:

	struct ConfettiConsts
	{
		// 消去位置
		static constexpr float ExpirePosX = 670.0f;
		static constexpr float ExpirePosY = 390.0f;
	};

	void Init()override;

	// パラメータ
	ConfettiParam m_param;

	// 角度
	float m_degAng = 0.0f;
};