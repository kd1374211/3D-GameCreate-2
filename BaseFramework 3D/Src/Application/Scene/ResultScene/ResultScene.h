#pragma once

#include"../BaseScene/BaseScene.h"

class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene() {}

	void Init()  override;

private:

	void Event() override;
};
