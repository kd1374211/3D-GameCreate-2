#include "Smoke.h"
#include "../../../Scene/SceneManager.h"

void Smoke::Update()
{
	// 本体のUpdateを呼ぶ
	EffectBase::Update();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 少し上昇
	m_pos.y += SmokeConsts::MoveSpeedY * gameDt;
}

void Smoke::PreDraw()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// Rectを合わせる
	m_data.m_polygon->SetUVRect((int)m_animCnt);

	// マトリックス設定
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale * m_scaleMulti);
	Math::Matrix rotat = Math::Matrix::CreateFromYawPitchRoll(EffectBase::SetAllRotationToCamera() + m_rot);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scale * rotat * trans;
}

void Smoke::DrawEffect()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// Rect合わせ
	SetUVRect();

	// 描画
	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld);
	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZEnable);
}

void Smoke::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::Smoke);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
