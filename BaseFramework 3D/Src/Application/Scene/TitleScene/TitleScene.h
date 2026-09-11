#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public :

	TitleScene()  {}
	~TitleScene() {}

	void Init()  override;

private :

	// シーン移行検知
	bool m_isSceneChangeReady = false;

	void Event() override;
};
