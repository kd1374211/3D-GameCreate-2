#include "CharaHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/Chara/BowlingBall/BowlingBall.h"
#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Camera/CameraManager.h"

void CharaHandler::Init()
{
	InitPlayerBall();
}

void CharaHandler::StartNextThrow(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	// ボールリスポーン
	RespawnPlayerBall(pos, rot);

	// 投球状態リセット
	m_rollingState = RollingState::NotRolling;
}

void CharaHandler::ResetPlayerBall()
{
	m_playerBall->Reset();
}

void CharaHandler::RespawnPlayerBall(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	m_playerBall->Respawn(pos, rot);
}

void CharaHandler::CheckRollingState()
{
	// 現在の投球状態を確認
	switch (m_rollingState)
	{
	// 開始前
	case RollingState::NotRolling:
		// 投げ始めたかを確認する
		if (m_playerBall->IsRolling())
		{
			// ステート変更
			m_rollingState = RollingState::Rolling;
		}
		break;
	// 開始後
	case RollingState::Rolling:
		// 投げ終わりを確認
		if (!m_playerBall->IsRolling())
		{
			// ステート変更
			m_rollingState = RollingState::RollEnd;
		}
		break;
	}
}

void CharaHandler::StartThrow()
{
	// プレイヤーを投球可能に
	m_playerBall->SetIsInputEnabled(true);
}

void CharaHandler::InitPlayerBall()
{
	// ボール召喚
	m_playerBall = std::make_shared<BowlingBall>();
	m_playerBall->Init(0.1f);
	SCENEMGR.AddObject(m_playerBall);
}
