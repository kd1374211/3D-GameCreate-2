#pragma once
#include "../TerrainBase.h"

class IslandDecoration :public TerrainBase
{
public:

	IslandDecoration(const std::string& modelPath, const Math::Vector3& pos, const Math::Vector3& rot, const Math::Vector3& scale);
	~IslandDecoration()override {}

private:


};