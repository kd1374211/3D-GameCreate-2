#include "EffectBase.h"
#include "../../Scene/SceneManager.h"
#include "../Camera/CameraManager.h"
#include "../Camera/CameraBase.h"

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
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale * m_scaleMulti);
	Math::Matrix rotat = Math::Matrix::CreateFromYawPitchRoll(m_rot);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix overallScale = GetOverAllScaleMat();
	m_mWorld = scale * rotat * trans * overallScale;
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

const Math::Vector3& EffectBase::SetRotationYToCamera()
{
	Math::Vector3 rot = Math::Vector3(0.0f, 0.0f, 1.0f);

	// 現在アクティブなカメラを取得
	if (const auto& camera = CAMERAMGR.GetActiveCamera().lock())
	{
		// カメラの方向に向くようにする
		rot = camera->GetRotationYMatrix().ToEuler();
	}

	return rot;
}

const Math::Vector3& EffectBase::SetAllRotationToCamera()
{
	Math::Vector3 rot = Math::Vector3(0.0f, 0.0f, 1.0f);

	// 現在アクティブなカメラを取得
	if (const auto& camera = CAMERAMGR.GetActiveCamera().lock())
	{
		// カメラの方向に向くようにする
		rot = camera->GetRotationMatrix().ToEuler();
	}

	return rot;
}
