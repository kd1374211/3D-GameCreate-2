#pragma once

#include"../BaseScene/BaseScene.h"

class StageSelectUIObject;

class StageSelectScene : public BaseScene
{
public:

	StageSelectScene() {}
	~StageSelectScene() {}

	void Init()  override;

private:

	void Event() override;

	//ステージセレクトUI
	std::weak_ptr<StageSelectUIObject> m_wpUI;
};
