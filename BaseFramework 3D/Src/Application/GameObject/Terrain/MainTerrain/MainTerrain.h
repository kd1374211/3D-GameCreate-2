#pragma once
#include "../TerrainBase.h"

class MainTerrain :public TerrainBase
{
public:

	MainTerrain(std::string modelPath, Math::Vector3 pos, Math::Quaternion rot);
	~MainTerrain()override {}

private:

};