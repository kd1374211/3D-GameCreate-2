#pragma once

#include"../BaseScene/BaseScene.h"

class GameUIObjects;

//シーン状況
enum class SceneState
{
	CountDown,
	Playing,
	GameOver,
	GameClear
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

	//各ステートの更新
	void UpdateCountDown();
	void UpdatePlaying();
	void UpdateGameOver();
	void UpdateGameClear();

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
	SceneState m_currentSceneState = SceneState::CountDown;

	//フェードイン終了確認
	bool m_isFadeInEnd = false;
};
