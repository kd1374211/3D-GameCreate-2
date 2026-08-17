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
	static constexpr float CountDownStart = 3.0f;
};

class GameScene : public BaseScene
{
public :

	GameScene()  {}
	~GameScene() {}

	void Init()  override;

private:

	void Event() override;

	// カウントダウン
	float m_countdownTimer = GameSceneConsts::CountDownStart;

	// ステージ時間管理
	float m_stageTimer = 0.0f;

	// ゲームUI
	std::weak_ptr<GameUIObjects> m_wpUI;

	// 現在のシーン状況
	SceneState m_currentSceneState = SceneState::CountDown;
};
