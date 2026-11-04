#include "PinHitParticle.h"
#include "../../../Scene/SceneManager.h"

void PinHitParticle::Update()
{
	// 本体のUpdateを呼ぶ
	EffectBase::Update();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 移動
	m_pos += gameDt * m_ptData.m_moveSpeed;

	// アルファ減少
	m_alpha += gameDt * PinHitParticleConsts::AlphaChangeSpeed;
	m_alpha = std::clamp(m_alpha, 0.0f, 1.0f);
}

void PinHitParticle::PreDraw()
{
	// マトリックス設定
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromYawPitchRoll(GetAllRotationToCamera() + m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale);
	m_mWorld = scale * rotat * trans;
}

void PinHitParticle::DrawBright()
{
	// 消滅済みか
	if (m_isExpired)return;

	// UV設定
	m_data.m_polygon->SetUVRect(m_ptData.m_type);

	// 深度情報が切り替えられたか
	bool isDSSChanged = false;

	// Z描画計算無効化
	if (m_alpha < 0.9f)
	{
		KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
		isDSSChanged = true;
	}

	static float alpha = KdRandom::GetInt(3, 7) / 10.0f;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld, Math::Color(alpha, alpha, alpha, m_alpha));

	// 切り替えられていたら戻す
	if (isDSSChanged)
	{
		KdShaderManager::Instance().UndoDepthStencilState();
	}
}

void PinHitParticle::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::HitParticle);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
