#include "SpeedUpArea.h"
#include "../../../../Const/Function.h"
#include "../../../../StageManager/StageManager.h"

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

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;

	Init();
}

void SpeedUpArea::Init()
{
	m_model = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/StageGimmicks/SpeedUpArea/SpeedUpArea.gltf");

	// デバッグ用
	m_frameModel = std::make_shared<KdModelData>();
	m_frameModel->Load("Asset/Models/Terrain/StageGimmicks/SpeedUpArea_Draw/SpeedUpArea_Draw.gltf");
}

void SpeedUpArea::DrawLit()
{
	if (!m_model)return;
	
	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void SpeedUpArea::DrawUnLit()
{
	// デバッグ中でないならリターン
	if (!STAGEMGR.IsEditMode())return;

	// デバッグ用フレームの描画
	if (m_frameModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_frameModel, m_mWorld);
	}
}
