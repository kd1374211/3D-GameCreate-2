#pragma once
#include "../TerrainBase.h"

class Island :public TerrainBase
{
public:

	Island(const std::string& modelPath, const Math::Vector3& pos);
	~Island()override {}

private:


};