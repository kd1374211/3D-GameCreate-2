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
	SetUp,
	Playing,
	Clean,
	End
};

// Game内の仮定義
struct GameSceneConsts
{
	// カウントダウン
	static constexpr float CountDownOnReady = 3.5f;
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

	//各ステートの更新
	//void UpdateCountDown();
	//void UpdatePlaying();
	//void UpdateGameOver();
	//void UpdateGameClear();

	//各ステートの更新
	void UpdateSetUp();
	void UpdatePlaying2();
	void UpdateClean();
	void UpdateEnd();

	// 投球終了処理
	void EndRolling();

	// リザルト計算
	GameResult CalcResult(bool isClear)const;

	// カウントダウン
	float m_countdownTimer = GameSceneConsts::CountDownOnReady;
	//流れる数字召喚フラグ
	const std::string MovingTexts[GameSceneConsts::MovingTextCount] = { "3","2","1","GO!" };
	bool m_isMovingTextSpawned[GameSceneConsts::MovingTextCount];

	// ステージ時間管理
	float m_stageTimer = 0.0f;

	// ゲームUI
	std::weak_ptr<GameUIObjects> m_wpUI;

	// 現在のシーン状況
	SceneState m_currentSceneState = SceneState::Playing;

	//フェードイン終了確認
	bool m_isFadeInEnd = false;

	// ピン制御用
	std::shared_ptr<PinHandler> m_cPinHandler = nullptr;

	// スコア管理・投球管理・フレーム管理
	std::shared_ptr<ScoreHandler> m_cScoreHandler = nullptr;

	// プレイヤー制御用
	std::shared_ptr<CharaHandler> m_cCharaHandler = nullptr;
};
