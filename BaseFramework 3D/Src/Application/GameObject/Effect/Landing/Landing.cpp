#include "Landing.h"
#include "../../../Scene/SceneManager.h"

void Landing::Update()
{
	// 本体のUpdateを呼ぶ
	EffectBase::Update();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// アルファ減少
	m_alpha += gameDt * LandingConsts::AlphaChangeSpeed;
	m_alpha = std::clamp(m_alpha, 0.0f, 1.0f);
}

void Landing::DrawEffect()
{
	// 消滅済みか
	if (m_isExpired)return;

	// Rect合わせ
	SetUVRect();

	// 深度情報が切り替えられたか
	bool isDSSChanged = false;

	// Z描画計算無効化
	if (m_alpha < 0.9f)
	{
		KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
		isDSSChanged = true;
	}

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld, Math::Color(1.0f, 1.0f, 1.0f, m_alpha));

	// 切り替えられていたら戻す
	if (isDSSChanged)
	{
		KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZEnable);
	}
}

void Landing::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::LandingAndWallHit);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
