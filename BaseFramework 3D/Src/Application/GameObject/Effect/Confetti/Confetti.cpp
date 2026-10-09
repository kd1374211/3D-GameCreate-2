#include "Confetti.h"
#include "../../../Scene/SceneManager.h"

void Confetti::Update()
{
	// 本体のUpdateを呼ぶ
	EffectBase::Update();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 移動
	// Dirは0を真下と捉える
	Math::Matrix rot = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_param.m_fallDir));
	Math::Vector3 fixedMoveDir = Math::Vector3::TransformNormal(Math::Vector3(0.0f, -1.0f, 0.0f), rot);
	fixedMoveDir.Normalize();
	m_pos += fixedMoveDir * m_param.m_fallSpeed * gameDt;

	// 回転
	m_rot += m_param.m_rotatDir * m_param.m_rotatSpeed * gameDt;

	// 位置による消去チェック
	if (fabs(m_pos.x) >= ConfettiConsts::ExpirePosX || fabs(m_pos.y) >= ConfettiConsts::ExpirePosY)
	{
		SetExpire();
	}
}

void Confetti::DrawEffect()
{
	// 消滅済みか
	if (m_isExpired)return;

	// このDrawはスポナー内でレンダーを切り替えてから呼ばれる

	// カメラ基準に
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_data.m_polygon, m_mWorld, m_param.m_color);
}

void Confetti::SetConfettiParam(const ConfettiParam& param)
{
	m_param = param;

	// すぐに反映する値もある
	m_pos.x = param.m_startPosX;
	m_data.m_scale = param.m_scale;
}

void Confetti::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::Confetti);

	// 本体のInitを呼ぶ
	EffectBase::Init();

	// 初期位置Y
	m_pos.y = ConfettiConsts::StartPosY;
}
