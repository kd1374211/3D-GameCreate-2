#include "CharaHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/Chara/Player/Player.h"
#include "../../GameObject/Chara/BowlingBall/BowlingBall.h"
#include "../../GameObject/Camera/CameraBase.h"
#include "../../GameObject/Camera/CameraManager.h"

void CharaHandler::Init()
{
	InitPlayerAndBall();
}

void CharaHandler::StartNextThrow(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	// プレイヤー・ボールリスポーン
	SetPlayerPosAndRot(pos, rot);

	// 投球状態リセット
	m_rollingState = RollingState::NotRolling;
}

void CharaHandler::ResetPlayerAndBall()
{
	m_bowlingBall->Reset();
}

void CharaHandler::SetPlayerPosAndRot(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	// 再召喚
	m_player->Respawn(pos, rot);
	// カメラのターゲットを戻す
	if (const auto& cam = CAMERAMGR.GetGameCamera().lock())
	{
		cam->SetTarget(m_player);
	}

	m_bowlingBall->Respawn(pos, rot);
}

void CharaHandler::CheckRollingState()
{
	// 現在の投球状態を確認
	switch (m_rollingState)
	{
	// 開始前
	case RollingState::NotRolling:
		// 投げ始めたかを確認する
		if (m_bowlingBall->IsRolling())
		{
			// ステート変更
			m_rollingState = RollingState::Rolling;
		}
		break;
	// 開始後
	case RollingState::Rolling:
		// 投げ終わりを確認
		if (!m_bowlingBall->IsRolling())
		{
			// 原因を確認
			if (m_bowlingBall->IsFall())
			{
				// ステート変更
				m_rollingState = RollingState::Fallen;
			}
			else
			{
				// ステート変更
				m_rollingState = RollingState::Stopped;
			}
		}
		break;
	}
}

void CharaHandler::InitPlayerAndBall()
{
	// プレイヤー召喚
	m_player = std::make_shared<Player>();
	m_player->Init();
	SCENEMGR.AddObject(m_player);

	// ボール召喚
	m_bowlingBall = std::make_shared<BowlingBall>();
	m_bowlingBall->Init(0.1f);
	SCENEMGR.AddObject(m_bowlingBall);
	
	// プレイヤーにボール登録
	m_player->SetBowlingBall(m_bowlingBall);
	//SetPlayerPosAndRot(Math::Vector3(0, 0.5f, 0), Math::Quaternion::Identity);
}
