#include "GameScene.h"
#include"../SceneManager.h"
#include "../../main.h"

#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
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
	case SceneState::Waiting:
		UpdateWaiting();
		break;
	case SceneState::Playing:
		UpdatePlaying2();
		break;
	case SceneState::CheckAndClean:
		UpdateCheckAndClean();
		break;
	case SceneState::MiddleResult:
		UpdateMiddleResult();
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

void GameScene::Reset()
{
	// 各フラグと値を初期化
	m_isFadeInEnd = false;
	m_isSceneChangeReady = false;
	m_isRollEndWaiting = false;
	m_isFrameChangeReady = false;

	m_countdownTimer = 0.0f;
	
}

void GameScene::UpdateWaiting()
{
	// フェードの終了を確認してPlayingに移行
	if (m_isFadeInEnd)
	{
		m_currentSceneState = SceneState::Playing;
	}
}

void GameScene::UpdatePlaying2()
{
	// デバッグ用
	static bool isSkipKey = true;
	if (GetAsyncKeyState('S') & 0x8000)
	{
		if (!isSkipKey)
		{
			// 投球終了処理
			EndRolling();
			// ステート更新
			m_currentSceneState = SceneState::CheckAndClean;

			// 長押し対策
			isSkipKey = true;

			// リターン
			return;
		}
	}
	else isSkipKey = false;

	// ゲーム内時間
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// カウントダウン終了後EndRolling移行
	if (m_isRollEndWaiting)
	{
		m_countdownTimer -= gameDt;
		if (m_countdownTimer < 0.0f)
		{
			// 投球終了処理
			EndRolling();
			// ステート更新
			m_currentSceneState = SceneState::CheckAndClean;
			// リターン
			return;
		}
	}
	else
	{
		// 全ピンが倒れたかの確認
		if (m_cPinHandler->CheckIsAllPinsFallen())
		{
			// 投球終了待ちフラグ
			m_isRollEndWaiting = true;

			// カウントダウン設定
			m_countdownTimer = GameSceneConsts::CountDownOnRollEnd;
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
		case RollingState::RollEnd:
			// 原因を確認
			switch (m_cCharaHandler->GetPlayerBall()->GetRollEndReason())
			{
			case RollEndReason::Stop: // 停止
				m_countdownTimer = GameSceneConsts::CountDownOnRollEnd_Stopped;
				break;
			case RollEndReason::Fall: // 落下
			case RollEndReason::Finish: // ゴール
				m_countdownTimer = GameSceneConsts::CountDownOnRollEnd;
				break;
			default:	// エラー
				m_countdownTimer = 0.0f;
				break;
			}
			// 投球終了フラグ
			m_isRollEndWaiting = true;
			break;
		}

		// デバッグ
		KdDebugGUI::Instance().AddLog("RollingState : %d\n", static_cast<int>(m_cCharaHandler->GetRollingState()));
	}
}

void GameScene::UpdateCheckAndClean()
{
	// リセット
	Reset();

	// UIのリザルトが消滅するまで待機
	if (!m_wpUI.expired())
	{
		if (!m_wpUI.lock()->GetIsThrowRecordTextActive())
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
				// 中間リザルト召喚
				m_wpUI.lock()->SpawnMiddleResult();
				// 中間リザルト確認に移行
				m_currentSceneState = SceneState::MiddleResult;
				break;
			case NextActions::BonusThrow:
				// 未定
				STAGEMGR.BonusStage(m_cScoreHandler->GetCurrentFrame());
				m_currentSceneState = SceneState::Playing;
				break;
			case NextActions::GameEnd:
				// リザルト移行準備
				m_currentSceneState = SceneState::End;
				m_countdownTimer = GameSceneConsts::CountDownOnClear;
				break;
			}
		}
	}
}

void GameScene::UpdateMiddleResult()
{
	// フェードアウトが終了したら
	if (m_isFrameChangeReady)
	{
		// フェードイン召喚
		FADEMGR.StartFadeIn(&m_isFadeInEnd);

		// 新しいレーンを生成
		SetUpLane();

		// 待機ステートに移行
		m_currentSceneState = SceneState::Waiting;

		// 撤退
		return;
	}

	// 中間リザルト終了を確認してフェードアウト召喚
	if (auto spUI = m_wpUI.lock())
	{
		// 終了したらフェードアウトを呼ぶ
		if (!spUI->GetIsMiddleResultActive())
		{
			FADEMGR.StartFadeOut(&m_isFrameChangeReady);
		}
	}
}

void GameScene::UpdateEnd()
{
	//リザルト移行
	if (m_isSceneChangeReady)
	{
		// スコア仮転送用
		STAGEMGR.SetTotalScore(m_cScoreHandler->GetTotalScoreInt(BowlingSystemConsts::LastFrame));

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
		FADEMGR.StartFadeOut(&m_isSceneChangeReady);
	}

	//仮表示
	KdDebugGUI::Instance().AddLog("CountDown : %.2f\n", m_countdownTimer);
}

void GameScene::EndRolling()
{
	// 倒れたピン数を取得
	int fallenPins = m_cPinHandler->GetFallenPinCount();

	// スコア通知＆マーク受け取り
	FrameMark mark = m_cScoreHandler->RecordThrow(fallenPins);

	// 受け取った情報をUIに渡しながらスポーン
	if (!m_wpUI.expired())
	{
		m_wpUI.lock()->SpawnThrowResultText(fallenPins, mark);
	}
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
	camera->SetTarget(m_cCharaHandler->GetPlayerBall());
	m_cCharaHandler->GetPlayerBall()->SetCamera(camera);
	
	//追加
	AddObject(camera);
	
	//UI
	std::shared_ptr<GameUIObjects> UIObj = std::make_shared<GameUIObjects>();
	m_wpUI = UIObj;
	AddObject(UIObj);

	//フェードイン
	FADEMGR.StartFadeIn(&m_isFadeInEnd);

	//一応ゲームスピードリセット	
	SCENEMGR.SetGameSpeed(1.0f);

	// デバッグ用
	KdDebugGUI::Instance().RegistScoreHandler(m_cScoreHandler);
}
