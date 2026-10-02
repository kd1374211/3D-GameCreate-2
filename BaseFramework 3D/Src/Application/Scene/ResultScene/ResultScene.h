#pragma once

#include"../BaseScene/BaseScene.h"

class ResultUIObject;

class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene() {}

	void Init()  override;

private:

	void Event() override;

	//フェードイン終了フラグ
	bool m_isFadeInEnd = false;

	// シーン移行確認
	bool m_isSceneChangeReady = false;

	// UI
	std::weak_ptr<ResultUIObject> m_wpUI;
};
