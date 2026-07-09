#pragma once
#include "../TerrainBase.h"

class Ground :public TerrainBase
{
public:

	Ground() { Init(); }
	~Ground()override {}

private:

	void Init()override;

};