#include "CharaHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/Chara/BowlingBall/BowlingBall.h"
#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Camera/CameraManager.h"

void CharaHandler::Init()
{
	InitPlayerBall();
}

void CharaHandler::StartNextThrow(const Math::Vector3& pos, const Math::Vector3& rot)
{
	// ボールリスポーン
	RespawnPlayerBall(pos, rot);

	// 投球後フラグをリセット
	m_isBallThrown = false;
}

void CharaHandler::CheckBallThrow()
{
	// 投球後フラグがオンならリターン
	if (m_isBallThrown)return;

	// ボールが投げられたかを確認
	if (m_playerBall->IsThrown())
	{
		// 投球後フラグをオンにする
		m_isBallThrown = true;
	}
}

void CharaHandler::ResetPlayerBall()
{
	m_playerBall->Reset();
}

void CharaHandler::RespawnPlayerBall(const Math::Vector3& pos, const Math::Vector3& rot)
{
	m_playerBall->Respawn(pos, rot);
}

bool CharaHandler::IsBallRolling() const
{
	// DEBUG
	KdDebugGUI::Instance().AddLog("IsRolling: %d\n", m_playerBall->IsRolling());

	return m_playerBall->IsRolling();
}

void CharaHandler::StartThrow()
{
	// プレイヤーを投球可能に
	m_playerBall->SetIsInputEnabled(true);
}

void CharaHandler::EndThrow()
{
	// 終了時処理(音を止める)
	m_playerBall->EndThrow();
}

void CharaHandler::InitPlayerBall()
{
	// ボール召喚
	m_playerBall = std::make_shared<BowlingBall>();
	m_playerBall->Init(0.1f);
	SCENEMGR.AddObject(m_playerBall);
}
