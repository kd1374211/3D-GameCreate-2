#include "GameScene.h"
#include"../SceneManager.h"
#include "../../main.h"

#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Chara/Player/Player.h"
#include "../../GameObject/Chara/CharaManager.h"
#include "../../GameObject/UI/SceneUIObjects/Game/GameUIObjects.h"
#include "../../FadeManager/FadeManager.h"
#include "../../GameObject/Camera/CameraManager.h"

void GameScene::Event()
{
	//シーンステート分岐
	switch (m_currentSceneState)
	{
	case SceneState::CountDown:
		UpdateCountDown();
		break;
	case SceneState::Playing:
		UpdatePlaying();
		break;
	case SceneState::GameOver:
		UpdateGameOver();
		break;
	case SceneState::GameClear:
		UpdateGameClear();
		break;
	}

	//ピン残数確認(DEBUG)
	KdDebugGUI::Instance().AddLog("Total Pin :%d\n", STAGEMGR.GetTotalPinCount());
	KdDebugGUI::Instance().AddLog("Remaining Pin :%d\n", STAGEMGR.GetRemainingPinCount());
}

void GameScene::UpdateCountDown()
{
	//デルタタイム取得
	float dt = Application::Instance().GetDeltaTime();

	//フェードイン終了待ち
	if (FADEMGR.IsFadeInEnd())
	{
		m_isFadeInEnd = true;
	}

	//終了後
	if (m_isFadeInEnd)
	{
		//時間経過
		m_countdownTimer -= dt;

		//流れるテキスト召喚
		for (int i = 0; i < GameSceneConsts::MovingTextCount; i++)
		{
			if (m_countdownTimer < GameSceneConsts::MovingTextsSpawn[i] && !m_isMovingTextSpawned[i])
			{
				//タイマー表示
				if (!m_wpUI.expired())
				{
					Math::Color color = i == GameSceneConsts::MovingTextColorGreen ? kGreenColor : kRedColor;
					m_wpUI.lock()->SpawnMovingText(MovingTexts[i], color);
				}

				m_isMovingTextSpawned[i] = true;
			}
		}

		//開始
		if (m_countdownTimer < 0.0f)
		{
			//Playingに切り替え
			m_currentSceneState = SceneState::Playing;

			//プレイヤーの移動操作解禁
			if (!CHARAMGR.GetPlayer().expired())
			{
				std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();

				player->SetIsMovable(true);
				player->SetIsInputEnabled(true);
			}

			//タイマー表示
			if (!m_wpUI.expired())
			{
				m_wpUI.lock()->SetIsDrawTimer(true);
			}
		}

		//仮表示
		KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
	}
}

void GameScene::UpdatePlaying()
{
	//デルタタイム取得
	float dt = Application::Instance().GetDeltaTime();
	float gameDt = SCENEMGR.GetDeltaGameTime();

	//ピンが全て倒れたらクリア移行
	if (!CHARAMGR.GetPlayer().expired() && CHARAMGR.GetPlayer().lock()->GetIsFinish())
	{
		//リザルトをセット
		STAGEMGR.SetGameResult(CalcResult(true));

		//ステージ終了演出召喚
		if (!m_wpUI.expired())
		{
			m_wpUI.lock()->SpawnStageFinishText(true);
		}

		//仮置きタイマーセット
		m_countdownTimer = GameSceneConsts::CountDownOnClear;

		//ゲーム速度ダウン
		SCENEMGR.SetGameSpeed(0.1f);

		//プレイヤーの操作ストップ
		if (!CHARAMGR.GetPlayer().expired())
		{
			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();

			player->SetIsInputEnabled(false);
		}

		//移行
		m_currentSceneState = SceneState::GameClear;
	}
	// タイムアップでゲームオーバー
	else if (m_stageTimer < 0.0)
	{
		STAGEMGR.SetGameResult(CalcResult(false));

		//ステージ終了演出召喚
		if (!m_wpUI.expired())
		{
			m_wpUI.lock()->SpawnStageFinishText(false);
		}

		//仮置きタイマーセット
		m_countdownTimer = GameSceneConsts::CountDownOnFail;

		//ゲーム速度ダウン
		SCENEMGR.SetGameSpeed(0.1f);

		//プレイヤーの操作ストップ
		if (!CHARAMGR.GetPlayer().expired())
		{
			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();

			player->SetIsInputEnabled(false);
		}

		//移行
		m_currentSceneState = SceneState::GameOver;
	}
	// プレイヤーの落下もゲームオーバー
	else
	{
		//プレイヤーの操作ストップ
		if (!CHARAMGR.GetPlayer().expired())
		{
			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();

			// プレイヤーのY座標が一定値より下なら
			if (player->GetIsFall())
			{
				STAGEMGR.SetGameResult(CalcResult(false));

				//ステージ終了演出召喚
				if (!m_wpUI.expired())
				{
					m_wpUI.lock()->SpawnStageFinishText(false);
				}

				//仮置きタイマーセット
				m_countdownTimer = GameSceneConsts::CountDownOnFail;

				//ゲーム速度ダウン
				SCENEMGR.SetGameSpeed(0.1f);

				//プレイヤーの操作ストップ
				player->SetIsInputEnabled(false);

				//移行
				m_currentSceneState = SceneState::GameOver;
			}
		}
	}

	//移行済ならこの先飛ばす
	if (m_currentSceneState != SceneState::Playing)return;

	// デバッグ用加速
	static bool isShiftKey = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		SCENEMGR.SetGameSpeed(2.0f);
		isShiftKey = true;
	}
	else
	{
		if(isShiftKey)
		{
			SCENEMGR.SetGameSpeed(1.0f);
		}
		isShiftKey = false;
	}

	// デバッグ用時間停止
	static bool isTimeStopKey = false;
	static bool isTimeStop = false;
	if (GetAsyncKeyState('R') & 0x8000)
	{
		if (!isTimeStopKey)
		{
			isTimeStop = !isTimeStop;
		}
		isTimeStopKey = true;
	}
	else isTimeStopKey = false;
	if (isTimeStop)return;

	//時間経過
	m_stageTimer -= gameDt;
	//UIタイマーに適応
	if (!m_wpUI.expired())
	{
		m_wpUI.lock()->SetTimer((int)std::ceil(m_stageTimer));
	}
}

void GameScene::UpdateGameOver()
{
	//リザルト移行
	if (FADEMGR.IsFadeOutEnd())
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Result
		);

		return;
	}

	//デルタタイム取得
	float dt = Application::Instance().GetDeltaTime();

	//時間経過
	m_countdownTimer -= dt;

	//暗転
	if (m_countdownTimer < 0.0f)
	{
		FADEMGR.StartFadeOut();
	}

	//仮表示
	KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
}

void GameScene::UpdateGameClear()
{
	//リザルト移行
	if (FADEMGR.IsFadeOutEnd())
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Result
		);

		return;
	}

	//デルタタイム取得
	float dt = Application::Instance().GetDeltaTime();

	//時間経過
	m_countdownTimer -= dt;

	//暗転
	if (m_countdownTimer < 0.0f)
	{
		FADEMGR.StartFadeOut();
	}

	//仮表示
	KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
}

GameResult GameScene::CalcResult(bool isClear) const
{
	GameResult result = {};
	result.m_isCleared = isClear;
	result.m_stageTimer = m_stageTimer;

	//ここSTAGEMGR側でやるのもあり
	result.m_fallenPinCnt = STAGEMGR.GetTotalPinCount() - STAGEMGR.GetRemainingPinCount();
	result.m_totalPinCnt = STAGEMGR.GetTotalPinCount();

	return result;
}

void GameScene::Init()
{
	//ステージ番号
	int stageNumber = SCENEMGR.GetStageNo();

	//ステージデータ読み込み（ステージ番号取得）
	if (STAGEMGR.LoadStage(stageNumber))
	{
		//ステージ生成
		STAGEMGR.BuildStage();
	}

	//カメラとプレイヤー生成
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();
	std::shared_ptr<Player> player = std::make_shared<Player>(Math::Vector3(0, 1.0f, 0), 0.1f);
	
	//取得用にセット
	CHARAMGR.SetPlayer(player);

	//マネージャーに追加
	CAMERAMGR.SetGameCamera(camera);
	CAMERAMGR.SetDefaultCamera(CameraType::Game);

	//リンク
	camera->SetTarget(player);
	player->SetCamera(camera);
	
	//追加
	AddObject(camera);
	AddObject(player);

	//UI
	std::shared_ptr<GameUIObjects> UIObj = std::make_shared<GameUIObjects>();
	m_wpUI = UIObj;
	AddObject(UIObj);

	//時間制限取得
	m_stageTimer = STAGEMGR.GetStageInfo(stageNumber)->m_timeLimit;
	UIObj->SetTimer(m_stageTimer);

	//フェードイン
	FADEMGR.StartFadeIn();

	for (int i = 0; i < GameSceneConsts::MovingTextCount; i++)
	{
		m_isMovingTextSpawned[i] = false;
	}

	//一応ゲームスピードリセット	
	SCENEMGR.SetGameSpeed(1.0f);
}
