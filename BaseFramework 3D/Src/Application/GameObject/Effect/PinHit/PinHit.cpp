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

	// サイズ拡大
	m_scaleMulti += gameDt * PinHitConsts::ScaleMultiChangeSpeed;

	// パーティクル更新
	for (auto& particle : m_particles)
	{
		// 移動
		particle.m_localPos += gameDt * particle.m_moveSpeed;
	}
}

void PinHit::PreDraw()
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

	// パーティクルを表示する
	for (const auto& particle : m_particles)
	{
		Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos + particle.m_localPos);
		Math::Matrix rotat = Math::Matrix::CreateFromYawPitchRoll(m_mWorld.Backward());
		Math::Matrix scale = Math::Matrix::CreateScale(PinHitConsts::HitParticleScale * m_scaleMulti);
		Math::Matrix mat = scale * rotat * trans;

		// UV変更
		m_particleColorTex->SetUVRect(particle.m_type);

		// 描画
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_particleColorTex, mat, Math::Color(1.0f, 1.0f, 1.0f, m_alpha));
	}

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

	// ランダムにパーティクル召喚
	const int ParticleSpawnCnt = KdRandom::GetInt(PinHitConsts::HitParticleSpawnMin, PinHitConsts::HitParticleSpawnMax);
	for (int i = 0; i < ParticleSpawnCnt; i++)
	{
		// データ
		ParticleData data = {};
		data.m_localPos = Math::Vector3::Zero;

		// 速度ランダム
		const float MoveSpeed = KdRandom::GetFloat(PinHitConsts::HitParticleSpeedMin, PinHitConsts::HitParticleSpeedMax);

		// 方向ランダム
		Math::Vector3 MoveDir = Math::Vector3(KdRandom::GetFloat(-1.0f, 1.0f), KdRandom::GetFloat(-1.0f, 1.0f), KdRandom::GetFloat(-1.0f, 1.0f));
		MoveDir.Normalize();

		// 計算後速度
		data.m_moveSpeed = MoveDir * MoveSpeed;

		// 色ランダム
		data.m_type = (ParticleData::ColorType)KdRandom::GetInt(0, ParticleData::ColorType::Number);

		// 格納
		m_particles.push_back(data);
	}

	// パーティクル用
	m_particleColorTex = std::make_shared<KdSquarePolygon>();
	m_particleColorTex->SetMaterial("Asset/Textures/Effect/PinHit/PinHitParticles.png");
	m_particleColorTex->SetSplit(2, 1);
}
