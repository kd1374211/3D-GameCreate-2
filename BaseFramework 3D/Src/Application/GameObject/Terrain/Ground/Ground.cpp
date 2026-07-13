#include "Ground.h"

void Ground::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Terrain/TestGround/TestGround.gltf");

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_model, KdCollider::TypeGround);
}
