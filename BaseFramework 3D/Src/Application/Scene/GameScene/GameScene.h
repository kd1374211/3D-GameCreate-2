#pragma once

#include"../BaseScene/BaseScene.h"

struct GameResult;
class GameUIObjects;
class PinHandler;
class ScoreHandler;
class CharaHandler;

//シーン状況
enum class SceneState
{
	Waiting,
	Playing,
	CheckAndClean,
	MiddleResult,
	End
};

// Game内の仮定義
struct GameSceneConsts
{
	// カウントダウン
	static constexpr float CountDownOnRollEnd = 2.0f;
	static constexpr float CountDownOnRollEnd_Stopped = 0.2f;
	static constexpr float CountDownOnClear = 2.0f;
	static constexpr float CountDownOnFail = 2.0f;

	//流れるテキスト
	static constexpr int MovingTextCount = 4;
	static constexpr int MovingTextColorGreen = 3;
	static constexpr float MovingTextsSpawn[MovingTextCount] = { 3.2f,2.2f,1.2f,0.2f };
};

class GameScene : public BaseScene
{
public :

	GameScene()  {}
	~GameScene() {}

	void Init()  override;

private:

	void Event() override;

	// レーンの準備
	void SetUpLane();

	// 投球準備
	void ReadyThrow();

	// フラグのリセット
	void Reset();

	//各ステートの更新
	//void UpdateCountDown();
	//void UpdatePlaying();
	//void UpdateGameOver();
	//void UpdateGameClear();

	//各ステートの更新
	void UpdateWaiting();
	void UpdatePlaying2();
	void UpdateCheckAndClean();
	void UpdateMiddleResult();
	void UpdateEnd();

	// 投球終了処理
	void EndRolling();

	// カウントダウン
	float m_countdownTimer = 0.0f;

	// 投球終了確認フラグ
	bool m_isRollEndWaiting = false;
	
	// ゲームUI
	std::weak_ptr<GameUIObjects> m_wpUI;

	// 現在のシーン状況
	SceneState m_currentSceneState = SceneState::Waiting;

	// 投球開始テキスト消滅確認
	bool m_isThrowStartTextEnd = false;

	//フェードイン終了確認
	bool m_isFadeInEnd = false;

	// 次レーンの準備
	bool m_isFrameChangeReady = false;

	// シーン変更準備
	bool m_isSceneChangeReady = false;

	// ピン制御用
	std::shared_ptr<PinHandler> m_cPinHandler = nullptr;

	// スコア管理・投球管理・フレーム管理
	std::shared_ptr<ScoreHandler> m_cScoreHandler = nullptr;

	// プレイヤー制御用
	std::shared_ptr<CharaHandler> m_cCharaHandler = nullptr;
};
