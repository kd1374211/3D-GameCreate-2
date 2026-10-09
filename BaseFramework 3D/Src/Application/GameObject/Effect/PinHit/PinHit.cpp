#include "PinHit.h"
#include "../../../Scene/SceneManager.h"

void PinHit::Update()
{
	// 本体のUpdateを呼ぶ
	EffectBase::Update();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// アルファ減少
	m_alpha += gameDt * PinHitConsts::AlphaChangeSpeed;
	m_alpha = std::clamp(m_alpha, 0.0f, 1.0f);
}

void PinHit::DrawEffect()
{
	// 消滅済みか
	if (m_isExpired)return;

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

void PinHit::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::HitImpact);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
