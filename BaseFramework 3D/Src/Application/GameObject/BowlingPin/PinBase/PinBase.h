#pragma once
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class PinBase :public KdGameObject
{
public:

	PinBase() {}
	~PinBase() {}

protected:

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	bool m_isRagdoll = false;	//ラグドール化しているかどうか
	std::shared_ptr<KdModelWork> m_model;

};