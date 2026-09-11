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

	//フェードイン終了待ち
	bool m_isFadeInEnd = false;

	// 次シーンへの移行待ち
	bool m_isSceneChangeReady = false;
};
