#pragma once
#include "../TerrainBase.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class Ground :public TerrainBase
{
public:

	Ground() { Init(); }
	~Ground()override {}

	void DrawLit()override;

private:

	void Init()override;

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

};