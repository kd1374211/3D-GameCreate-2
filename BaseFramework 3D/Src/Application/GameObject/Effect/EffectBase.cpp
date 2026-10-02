#include "EffectBase.h"
#include "../../Scene/SceneManager.h"

void EffectBase::Update()
{
	// GameDt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// アニメーションを進める
	m_animCnt += m_data.m_animSpeed * gameDt;

	// ループ確認
	if (m_animCnt >= m_data.m_animMax)
	{
		// ループしないなら消滅
		if (m_data.m_isLoop)
		{
			m_animCnt -= m_data.m_animMax;
		}
		else
		{
			SetExpire();
		}
	}
}

void EffectBase::PreDraw()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// Rectを合わせる
	m_data.m_polygon->SetUVRect((int)m_animCnt);

	// マトリックス設定
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scale * trans;
}

void EffectBase::DrawEffect()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld);
}

void EffectBase::Init()
{
}
