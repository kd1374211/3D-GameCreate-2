#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public :

	TitleScene()  {}
	~TitleScene() {}

	void Init()  override;

private :

	void Event() override;
};
