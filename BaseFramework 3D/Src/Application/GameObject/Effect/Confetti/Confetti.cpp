#include "Confetti.h"
#include "../../../Scene/SceneManager.h"
#include "../EffectManager.h"

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
	m_degAng += m_param.m_rotatSpeed * gameDt;
	if (m_degAng >= 360.0f)m_degAng -= 360.0f;
	else if (m_degAng <= 0.0f)m_degAng += 360.0f;

	// 位置による消去チェック
	if (fabs(m_pos.x) >= ConfettiConsts::ExpirePosX || fabs(m_pos.y) >= ConfettiConsts::ExpirePosY)
	{
		SetExpire();
	}
}

void Confetti::PreDraw()
{
	// 消滅済みならストップ
	if (m_isExpired)return;

	// Rectを合わせる
	m_data.m_polygon->SetUVRect((int)m_animCnt);

	// マトリックス設定
	Math::Matrix scale = Math::Matrix::CreateScale(m_data.m_scale * m_scaleMulti);
	Math::Matrix rotat = Math::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(m_degAng));
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = scale * rotat * trans;
}

void Confetti::DrawSprite()
{
	// 消滅済みか
	if (m_isExpired)return;

	// このDrawはスポナー内でレンダーを切り替えてから呼ばれる

	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
	KdShaderManager::Instance().m_spriteShader.DrawTex(EFFECTMGR.GetSquareTexture(), 0, 0, nullptr, &m_param.m_color);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void Confetti::SetConfettiParam(const ConfettiParam& param)
{
	m_param = param;

	// すぐに反映する値もある
	m_data.m_scale = param.m_scale;
	m_pos = param.m_startPos;
}

void Confetti::Init()
{
	// マネージャー経由でデータ取得
	m_data = EFFECTMGR.GetEffectData(EffectType::Confetti);
	m_degAng = 0.0f;

	// 本体のInitを呼ぶ
	EffectBase::Init();
}
