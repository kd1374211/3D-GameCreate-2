#include "GameScene.h"
#include"../SceneManager.h"
#include "../../main.h"

#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Chara/Player/Player.h"
#include "../../GameObject/Chara/BowlingBall/BowlingBall.h"
#include "../../Component/CharaHandler/CharaHandler.h"
#include "../../GameObject/UI/SceneUIObjects/Game/GameUIObjects.h"
#include "../../FadeManager/FadeManager.h"
#include "../../GameObject/Camera/CameraManager.h"
#include "../../Component/PinHandler/PinHandler.h"
#include "../../Component/ScoreHandler/ScoreHandler.h"
#include "../../UserSave/UserSaveManager.h"

void GameScene::Event()
{
	//シーンステート分岐
	switch (m_currentSceneState)
	{
	case SceneState::SetUp:
		UpdateSetUp();
		break;
	case SceneState::Playing:
		UpdatePlaying2();
		break;
	case SceneState::Clean:
		UpdateClean();
		break;
	case SceneState::End:
		UpdateEnd();
		break;
	}

	// デバッグ用
	m_cScoreHandler->AddDebugScoreLog();
}

void GameScene::SetUpLane()
{
	//ステージ生成
	STAGEMGR.BuildStage(m_cScoreHandler->GetCurrentFrame());
}
//
//void GameScene::UpdateCountDown()
//{
//	//デルタタイム取得
//	float dt = Application::Instance().GetDeltaTime();
//
//	//フェードイン終了待ち
//	if (FADEMGR.IsFadeInEnd())
//	{
//		m_isFadeInEnd = true;
//	}
//
//	//終了後
//	if (m_isFadeInEnd)
//	{
//		//時間経過
//		m_countdownTimer -= dt;
//
//		//流れるテキスト召喚
//		for (int i = 0; i < GameSceneConsts::MovingTextCount; i++)
//		{
//			if (m_countdownTimer < GameSceneConsts::MovingTextsSpawn[i] && !m_isMovingTextSpawned[i])
//			{
//				//タイマー表示
//				if (!m_wpUI.expired())
//				{
//					Math::Color color = i == GameSceneConsts::MovingTextColorGreen ? kGreenColor : kRedColor;
//					m_wpUI.lock()->SpawnMovingText(MovingTexts[i], color);
//				}
//
//				m_isMovingTextSpawned[i] = true;
//			}
//		}
//
//		//開始
//		if (m_countdownTimer < 0.0f)
//		{
//			//Playingに切り替え
//			m_currentSceneState = SceneState::Playing;
//
//			//プレイヤーの移動操作解禁
//			if (!CHARAMGR.GetPlayer().expired())
//			{
//				std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();
//
//				player->SetIsMovable(true);
//				player->SetIsInputEnabled(true);
//			}
//
//			//タイマー表示
//			if (!m_wpUI.expired())
//			{
//				m_wpUI.lock()->SetIsDrawTimer(true);
//			}
//		}
//
//		//仮表示
//		KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
//	}
//}
//
//void GameScene::UpdatePlaying()
//{
//	//デルタタイム取得
//	float dt = Application::Instance().GetDeltaTime();
//	float gameDt = SCENEMGR.GetDeltaGameTime();
//
//	//ピンが全て倒れたらクリア移行
//	if (!CHARAMGR.GetPlayer().expired() && CHARAMGR.GetPlayer().lock()->GetIsFinish())
//	{
//		//リザルトをセット
//		//STAGEMGR.SetGameResult(CalcResult(true));
//
//		//ステージ終了演出召喚
//		if (!m_wpUI.expired())
//		{
//			m_wpUI.lock()->SpawnStageFinishText(true);
//		}
//
//		//仮置きタイマーセット
//		m_countdownTimer = GameSceneConsts::CountDownOnClear;
//
//		//ゲーム速度ダウン
//		SCENEMGR.SetGameSpeed(0.1f);
//
//		//プレイヤーの操作ストップ
//		if (!CHARAMGR.GetPlayer().expired())
//		{
//			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();
//
//			player->SetIsInputEnabled(false);
//		}
//
//		//移行
//		m_currentSceneState = SceneState::GameClear;
//	}
//	// タイムアップでゲームオーバー
//	else if (m_stageTimer < 0.0)
//	{
//		//STAGEMGR.SetGameResult(CalcResult(false));
//
//		//ステージ終了演出召喚
//		if (!m_wpUI.expired())
//		{
//			m_wpUI.lock()->SpawnStageFinishText(false);
//		}
//
//		//仮置きタイマーセット
//		m_countdownTimer = GameSceneConsts::CountDownOnFail;
//
//		//ゲーム速度ダウン
//		SCENEMGR.SetGameSpeed(0.1f);
//
//		//プレイヤーの操作ストップ
//		if (!CHARAMGR.GetPlayer().expired())
//		{
//			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();
//
//			player->SetIsInputEnabled(false);
//		}
//
//		//移行
//		m_currentSceneState = SceneState::GameOver;
//	}
//	// プレイヤーの落下もゲームオーバー
//	else
//	{
//		//プレイヤーの操作ストップ
//		if (!CHARAMGR.GetPlayer().expired())
//		{
//			std::shared_ptr<Player> player = CHARAMGR.GetPlayer().lock();
//
//			// プレイヤーのY座標が一定値より下なら
//			if (player->GetIsFall())
//			{
//				//STAGEMGR.SetGameResult(CalcResult(false));
//
//				//ステージ終了演出召喚
//				if (!m_wpUI.expired())
//				{
//					m_wpUI.lock()->SpawnStageFinishText(false);
//				}
//
//				//仮置きタイマーセット
//				m_countdownTimer = GameSceneConsts::CountDownOnFail;
//
//				//ゲーム速度ダウン
//				SCENEMGR.SetGameSpeed(0.1f);
//
//				//プレイヤーの操作ストップ
//				player->SetIsInputEnabled(false);
//
//				//移行
//				m_currentSceneState = SceneState::GameOver;
//			}
//		}
//	}
//
//	//移行済ならこの先飛ばす
//	if (m_currentSceneState != SceneState::Playing)return;
//
//	// デバッグ用加速
//	static bool isShiftKey = false;
//	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
//	{
//		SCENEMGR.SetGameSpeed(2.0f);
//		isShiftKey = true;
//	}
//	else
//	{
//		if(isShiftKey)
//		{
//			SCENEMGR.SetGameSpeed(1.0f);
//		}
//		isShiftKey = false;
//	}
//
//	// デバッグ用超加速
//	static bool isTurbo = false;
//	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
//	{
//		isTurbo = true;
//	}
//	else
//	{
//		isTurbo = false;
//	}
//
//	// デバッグ用時間停止
//	static bool isTimeStopKey = false;
//	static bool isTimeStop = false;
//	if (GetAsyncKeyState('R') & 0x8000)
//	{
//		if (!isTimeStopKey)
//		{
//			isTimeStop = !isTimeStop;
//		}
//		isTimeStopKey = true;
//	}
//	else isTimeStopKey = false;
//	if (isTimeStop)return;
//
//	//時間経過
//	m_stageTimer -= gameDt * (isTurbo ? 40.0f : 1.0f);
//	//UIタイマーに適応
//	if (!m_wpUI.expired())
//	{
//		m_wpUI.lock()->SetTimer((int)std::ceil(m_stageTimer));
//	}
//}

void GameScene::UpdateSetUp()
{
	SetUpLane();
	m_currentSceneState = SceneState::Playing;
}

void GameScene::UpdatePlaying2()
{
	// 全ピンが倒れたかの確認
	if (m_cPinHandler->CheckIsAllPinsFallen())
	{
		// 投球終了処理
		EndRolling();
		// ステート更新
		m_currentSceneState = SceneState::Clean;

		// リターン
		return;
	}

	// ステート更新
	m_cCharaHandler->CheckRollingState();

	// ステート取得
	switch (m_cCharaHandler->GetRollingState())
	{
	case RollingState::NotRolling:
		break;
	case RollingState::Rolling:
		break;
	case RollingState::Stopped:
		// 投球終了処理
		EndRolling();
		// ステート更新
		m_currentSceneState = SceneState::Clean;
		break;
	case RollingState::Fallen:
		// 投球終了処理
		EndRolling();
		// ステート更新
		m_currentSceneState = SceneState::Clean;
		break;
	}

	// デバッグ
	KdDebugGUI::Instance().AddLog("RollingState : %d\n",static_cast<int>(m_cCharaHandler->GetRollingState()));
}

void GameScene::UpdateClean()
{
	// 現在は次への確認だけ
	switch (m_cScoreHandler->GetNextAction())
	{
	case NextActions::NextThrow:
		// 同じレーンの再配置
		STAGEMGR.RespawnStage(m_cScoreHandler->GetCurrentFrame());
		m_currentSceneState = SceneState::Playing;
		break;
	case NextActions::NextFrame:
		// 新しいレーンの準備
		m_currentSceneState = SceneState::SetUp;
		break;
	case NextActions::BonusThrow:
		// 未定
		STAGEMGR.RespawnStage(m_cScoreHandler->GetCurrentFrame());
		m_currentSceneState = SceneState::Playing;
		break;
	case NextActions::GameEnd:
		// リザルト移行準備
		m_currentSceneState = SceneState::End;
		m_countdownTimer = GameSceneConsts::CountDownOnClear;
		break;
	}
}

void GameScene::UpdateEnd()
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

void GameScene::EndRolling()
{
	// 倒れたピン数を取得
	int fallenPins = m_cPinHandler->GetFallenPinCount();

	// スコア通知
	m_cScoreHandler->RecordThrow(fallenPins);
}
//
//void GameScene::UpdateGameOver()
//{
//	//リザルト移行
//	if (FADEMGR.IsFadeOutEnd())
//	{
//		SceneManager::Instance().SetNextScene
//		(
//			SceneManager::SceneType::Result
//		);
//
//		return;
//	}
//
//	//デルタタイム取得
//	float dt = Application::Instance().GetDeltaTime();
//
//	//時間経過
//	m_countdownTimer -= dt;
//
//	//暗転
//	if (m_countdownTimer < 0.0f)
//	{
//		FADEMGR.StartFadeOut();
//	}
//
//	//仮表示
//	KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
//}
//
//void GameScene::UpdateGameClear()
//{
//	//リザルト移行
//	if (FADEMGR.IsFadeOutEnd())
//	{
//		SceneManager::Instance().SetNextScene
//		(
//			SceneManager::SceneType::Result
//		);
//
//		return;
//	}
//
//	//デルタタイム取得
//	float dt = Application::Instance().GetDeltaTime();
//
//	//時間経過
//	m_countdownTimer -= dt;
//
//	//暗転
//	if (m_countdownTimer < 0.0f)
//	{
//		FADEMGR.StartFadeOut();
//	}
//
//	//仮表示
//	KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
//}

GameResult GameScene::CalcResult(bool isClear) const
{
	GameResult result = {};
	result.m_isCleared = isClear;
	result.m_stageTimer = m_stageTimer;

	return result;
}

void GameScene::Init()
{
	// スコアハンドラー生成
	m_cScoreHandler = std::make_shared<ScoreHandler>();
	m_cScoreHandler->Init();

	// ピンハンドラー生成
	m_cPinHandler = std::make_shared<PinHandler>();

	// キャラハンドラー生成
	m_cCharaHandler = std::make_shared<CharaHandler>();
	m_cCharaHandler->Init();

	// ステージマネージャーにハンドラー設定
	STAGEMGR.RegistPinHandler(m_cPinHandler);
	STAGEMGR.RegistCharaHandler(m_cCharaHandler);

	//ステージ番号
	int stageNumber = SCENEMGR.GetStageNo();

	//ステージデータ読み込み（ステージ番号取得）
	if (!STAGEMGR.LoadStage(stageNumber))
	{
		// 失敗時の処理
	}
	// ピンプール用意
	STAGEMGR.CreatePinPool();

	// レーン生成
	SetUpLane();

	//カメラとプレイヤー生成
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();

	//マネージャーに追加
	CAMERAMGR.SetGameCamera(camera);
	CAMERAMGR.SetDefaultCamera(CameraType::Game);

	//リンク
	camera->SetTarget(m_cCharaHandler->GetPlayer());
	
	//追加
	AddObject(camera);
	
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
