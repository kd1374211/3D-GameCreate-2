#pragma once
#include "../TerrainBase.h"

class Ground :public TerrainBase
{
public:

	Ground(std::string modelPath, Math::Vector3 pos, Math::Quaternion rot);
	~Ground()override {}

	void DrawLit()override;

private:

};