#pragma once

#include"../BaseScene/BaseScene.h"

class TitleUIObject;

class TitleScene : public BaseScene
{
public :

	TitleScene()  {}
	~TitleScene() {}

	void Init()  override;

private :

	// シーン移行検知
	bool m_isSceneChangeReady = false;

	// UIの弱参照
	std::weak_ptr<TitleUIObject> m_wpUI;

	void Event() override;
};
