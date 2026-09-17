#pragma once
#include "../TerrainBase.h"

class RotatingTerrain :public TerrainBase
{
public:

	RotatingTerrain(std::string modelPath, Math::Vector3 pos, Math::Quaternion rot, Math::Vector3 sca, float degPerSec);
	~RotatingTerrain()override {}

	void PostUpdate()override;

private:

};
