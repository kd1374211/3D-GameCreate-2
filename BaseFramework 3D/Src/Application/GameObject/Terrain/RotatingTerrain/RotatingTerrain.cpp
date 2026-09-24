#include "RotatingTerrain.h"
#include "../../../StageManager/StageManager.h"
#include "../../../Const/Function.h"

RotatingTerrain::RotatingTerrain(std::string modelPath, Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, float degPerSec)
{
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	m_model = std::make_shared<KdModelData>();
	m_model->Load(modelPath);

	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = quat;
	initData.scale = sca;
	initData.motionType = JPH::EMotionType::Kinematic;
	initData.isStatic = false;
	initData.layer = Layers::TERRAIN;
	initData.friction = 0.05f;
	initData.restitution = 0.0f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->Init(modelPath, initData)) {
		// エラー処理
	}

	// 回転を加える
	m_cPhysics->SetAngularVelocity(JPH::Vec3(0, DirectX::XMConvertToRadians(degPerSec), 0));

	// 情報設定
	m_pos = pos;
	m_rot = quat;
	m_scale = sca;

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;
}

void RotatingTerrain::PostUpdate()
{
	// 停止中ならリターン
	if (STAGEMGR.IsEditMode() && STAGEMGR.GetIsGimmickStop())return;

	//  物理座標の同期（Joltから最新座標を反映）
	m_cPhysics->Sync(m_pos, m_rot);

	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	//  ワールド行列の合成（旋回 → 移動)
	m_mWorld = scale * rotat * trans;
}
