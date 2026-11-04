#include "PinHit.h"
#include "../../../Scene/SceneManager.h"
#include "../EffectManager.h"
#include "../PinHitParticle/PinHitParticle.h"

void PinHit::PreDraw()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// マトリックス設定
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale * m_scaleMulti);
	Math::Matrix rotat = Math::Matrix::CreateFromYawPitchRoll(GetAllRotationToCamera() + m_rot);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scale * rotat * trans;
}

void PinHit::DrawBright()
{
	// 消滅済みか
	if (m_isExpired)return;

	// Rect合わせ
	SetUVRect();

	static float alpha = KdRandom::GetInt(3, 7) / 10.0f;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld, Math::Color(alpha, alpha, alpha, 1.0f));
}

void PinHit::PostSetInit()
{
	// ランダムにパーティクル召喚
	const int ParticleSpawnCnt = KdRandom::GetInt(PinHitConsts::HitParticleSpawnMin, PinHitConsts::HitParticleSpawnMax);
	for (int i = 0; i < ParticleSpawnCnt; i++)
	{
		// データ
		ParticleData data = {};

		// 速度ランダム
		const float MoveSpeed = KdRandom::GetFloat(PinHitConsts::HitParticleSpeedMin, PinHitConsts::HitParticleSpeedMax);

		// 方向ランダム
		Math::Vector3 MoveDir = Math::Vector3(KdRandom::GetFloat(-1.0f, 1.0f), KdRandom::GetFloat(-1.0f, 1.0f), KdRandom::GetFloat(-1.0f, 1.0f));
		MoveDir.Normalize();

		// 計算後速度
		data.m_moveSpeed = MoveDir * MoveSpeed;

		// 色ランダム
		data.m_type = (ParticleData::ColorType)KdRandom::GetInt(0, ParticleData::ColorType::Number);

		// サイズランダム
		int randIdx = KdRandom::GetInt(1, PinHitConsts::HitParticleScaleRandNum) - 1;
		float scale = PinHitConsts::HitParticleScale[randIdx];

		// 召喚とインスタンス回収
		const std::shared_ptr<PinHitParticle> particle = std::dynamic_pointer_cast<PinHitParticle>(EFFECTMGR.SpawnEffect(m_pos, m_rot, scale, EffectType::HitParticle));
		if (particle)
		{
			// 他の値を入れる
			particle->SetParticleData(data);
		}
	}
}

void PinHit::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::HitImpact);

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
