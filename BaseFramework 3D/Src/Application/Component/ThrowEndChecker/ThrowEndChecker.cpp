#include "ThrowEndChecker.h"
#include "../PinHandler/PinHandler.h"
#include "../CharaHandler/CharaHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../Const/DeviceAndKey.h"

bool ThrowEndChecker::CheckThrowEnd()
{
	// ピンハンドラーとキャラハンドラーの有効性を確認
	const auto& pinHandler = m_wpPinHandler.lock();
	const auto& charaHandler = m_wpCharaHandler.lock();
	if (!pinHandler || !charaHandler)
	{
		// どちらかないなら終了とみなす（詰み防止）
		return true;
	}

	// 右クリックが押されたら終了にする
	// 途中終了
	if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_RBUTTON)))
	{
		return true;
	}

	// キャラが投球中かを確認
	if (charaHandler->IsBallRolling())
	{
		m_throwEndCheckTimer = 0.0f; // タイマーリセット
	}
	// 各ピンが停止状態かを確認
	else if (pinHandler->CheckIsAnyPinMoving())
	{
		m_throwEndCheckTimer = 0.0f; // タイマーリセット
	}
	else
	{
		//タイマーを加算
		m_throwEndCheckTimer += SCENEMGR.GetDeltaGameTime();

		// タイマーが一定時間を超えたら投球終了とみなす
		if (m_throwEndCheckTimer >= ThrowEndCheckerConsts::ThrowEndCheckTime)
		{
			return true;
		}
	}

	// 投球終了条件を満たしていない
	return false;
}
