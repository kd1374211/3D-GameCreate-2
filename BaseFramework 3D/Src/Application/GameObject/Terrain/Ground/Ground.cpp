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

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = Math::Vector3::Zero;
	initData.rot = Math::Quaternion::Identity;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.layer = Layers::TERRAIN;
	initData.friction = 0.2f;
	initData.restitution = 0.0f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);
	
	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->Init(path, initData)) {
		// エラー処理
	}
}
