#pragma once
#include "../../TerrainBase.h"
#include "../../../../Const/VariantParamData.h"

class RotatingTerrain :public TerrainBase
{
public:

	RotatingTerrain(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, RotatingParams param);
	~RotatingTerrain()override {}

	void PostUpdate()override;

private:

};
