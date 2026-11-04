#include "SpeedUpArea.h"
#include "../../../../Const/Function.h"

SpeedUpArea::SpeedUpArea(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 size)
{
	m_cPhysics = std::make_shared<PhysicsComponent>();
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = quat;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.isSensor = true;
	initData.layer = Layers::SPEEDUPAREA;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	m_cPhysics->Init(size * 0.5f, initData);

	// 情報設定
	m_pos = pos;
	m_rot = quat;
	m_scale = size;

	Init();
}

void SpeedUpArea::Init()
{
	m_model = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/StageGimmicks/SpeedUpArea/SpeedUpArea.gltf");
}
