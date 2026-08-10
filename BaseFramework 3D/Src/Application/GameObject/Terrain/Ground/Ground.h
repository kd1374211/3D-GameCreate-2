#pragma once
#include "../TerrainBase.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class Ground :public TerrainBase
{
public:

	Ground(std::string modelPath, Math::Vector3 pos, Math::Quaternion rot);
	~Ground()override {}

	void DrawLit()override;

private:
	
	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

};