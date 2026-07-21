#include "Ground.h"
#include "../../../../Framework/Direct3D/KdGLTFLoader.h"

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Ground::Init()
{
	std::string path = "Asset/Models/Terrain/TestGround/TestGround.gltf";
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(path);

	m_cPhysics = std::make_shared<PhysicsComponent>();

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->Init(path)) {
		// エラー処理
	}

	//m_pCollider = std::make_unique<KdCollider>();
	//m_pCollider->RegisterCollisionShape("Ground", m_model, KdCollider::TypeGround);
}
