#include "NormalPin.h"

NormalPin::NormalPin(Math::Vector3 pos, Math::Quaternion rotat)
{
	//本体
	PinBase::Init();

	std::string path_hit = "Asset/Models/Pin/Hit/pin.gltf";
	std::string path_draw = "Asset/Models/Pin/Draw/pin.gltf";
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(path_draw);

	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = rotat;
	initData.motionType = JPH::EMotionType::Dynamic;
	initData.isStatic = false;
	initData.layer = Layers::PIN_STATIC;
	initData.mass = 10000.0f;
	initData.friction = 0.2f;
	initData.restitution = 0.0f;
	initData.linearDamping = 1.0f;
	initData.angularDamping	= 1.0f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->Init(path_hit, initData)) {
		// エラー処理
	}
}
