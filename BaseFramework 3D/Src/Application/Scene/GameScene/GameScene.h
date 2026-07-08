#pragma once

#include"../BaseScene/BaseScene.h"

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init()  override;
};
