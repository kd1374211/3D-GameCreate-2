#include "Ground.h"
#include "../../../../Framework/Direct3D/KdGLTFLoader.h"

Ground::Ground(std::string modelPath, Math::Vector3 pos, Math::Quaternion rot)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(modelPath);

	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = rot;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.layer = Layers::TERRAIN;
	initData.friction = 0.2f;
	initData.restitution = 0.0f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->Init(modelPath, initData)) {
		// エラー処理
	}

	// 情報設定
	m_pos = pos;
	m_rot = rot;

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	m_mWorld = rotat * trans;
}

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
