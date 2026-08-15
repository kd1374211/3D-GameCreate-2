#pragma once

#include"../BaseScene/BaseScene.h"

class GameUIObjects;

class GameScene : public BaseScene
{
public :

	GameScene()  {}
	~GameScene() {}

	void Init()  override;

private:

	void Event() override;

	//ステージ時間管理
	float m_stageTimer = 0.0f;

	//ゲームUI
	std::weak_ptr<GameUIObjects> m_wpUI;
};
