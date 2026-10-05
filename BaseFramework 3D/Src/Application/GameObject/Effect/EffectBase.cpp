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

	// 回転を合わせる
	Math::Matrix rotat = SetRotationToCamera();

	// マトリックス設定
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scale * rotat * trans;
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

Math::Matrix EffectBase::SetRotationToCamera()
{
	Math::Matrix rot = Math::Matrix::Identity;

	// 現在アクティブなカメラを取得
	if (const auto& camera = CAMERAMGR.GetActiveCamera().lock())
	{
		// カメラの方向に向くようにする
		rot = camera->GetRotationYMatrix();
	}

	return rot;
}

void EffectBase::UpdateRotate()
{
	
}
