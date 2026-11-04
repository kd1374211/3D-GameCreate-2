#pragma once
#include "../../TerrainBase.h"
#include "../../../../Component/PhysicsComponent/PhysicsComponent.h"

class SpeedUpArea :public TerrainBase
{
public:

	SpeedUpArea(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 size);
	~SpeedUpArea()override {}

	void Init()override;

private:

	//サイズ
	Math::Vector3 m_scale = Math::Vector3::One;

};