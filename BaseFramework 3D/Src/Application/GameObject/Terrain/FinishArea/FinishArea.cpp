#include "FinishArea.h"
#include "../../../StageManager/StageManager.h"

FinishArea::FinishArea(Math::Vector3 pos, Math::Quaternion rot, Math::Vector3 size)
{
	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = Math::Quaternion::Identity;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.isSensor = true;
	initData.layer = Layers::FINISHAREA;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	m_cPhysics->Init(size * 0.5f, initData);

	// 情報設定
	m_pos = pos;
	m_rot = rot;
	m_scale = size;

	Init();
}

void FinishArea::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Test/Cube/Cube.gltf");

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;
}

void FinishArea::DrawUnLit()
{
	if (STAGEMGR.GetMode() == StageMode::Play)return;
	//Math::Color color = Math::Color(1, 1, 1, 0.7f);

	//KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
	//KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZEnable);
}
