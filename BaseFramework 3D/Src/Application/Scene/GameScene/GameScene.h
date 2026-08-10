#pragma once

#include"../BaseScene/BaseScene.h"

class GameScene : public BaseScene
{
public :

	GameScene()  {}
	~GameScene() {}

	void Init()  override;

private:

	void Event() override;
};
