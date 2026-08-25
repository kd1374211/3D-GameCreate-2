#pragma once
#include "../TerrainBase.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class FinishArea :public TerrainBase
{
public:

	FinishArea(Math::Vector3 pos, Math::Quaternion rot, Math::Vector3 size);
	~FinishArea()override {}

	void Init()override;

	//デバッグ用
	void DrawLit()override {}
	void DrawUnLit()override;

private:

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	//サイズ
	Math::Vector3 m_scale = Math::Vector3::One;
};