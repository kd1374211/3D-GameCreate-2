#include "IslandDecoration.h"
#include "../../../Const/Function.h"
#include "../../../GameObject/Camera/CameraManager.h"
#include "../../../GameObject/Camera/LookingDownCamera/LookingDownCamera.h"

IslandDecoration::IslandDecoration(const std::string& modelPath, const Math::Vector3& pos, const Math::Vector3& rot, const Math::Vector3& scale)
{
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	m_model = KdAssets::Instance().m_modeldatas.GetData(modelPath);
	
	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = quat;
	initData.scale = scale;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.layer = Layers::TERRAIN;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->InitTerrainModel(modelPath, initData)) {
		// エラー処理
	}

	// 情報設定
	m_pos = pos;
	m_rot = quat;
	m_scale = scale;

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scaleMat * rotat * trans;

	// カメラに登録
	RegistToCamera();
}

void IslandDecoration::DrawLit()
{
	// フラグを確認
	if (!m_isActive)return;
	if (!m_isDraw)return;

	// 描画するなら本体のDrawLitを呼ぶ
	TerrainBase::DrawLit();
}

void IslandDecoration::GenerateDepthMapFromLight()
{
	// フラグを確認
	if (!m_isActive)return;
	if (!m_isDraw)return;

	// 描画するなら本体のGenerateを呼ぶ
	TerrainBase::GenerateDepthMapFromLight();
}

void IslandDecoration::SetIsActive(bool flg)
{
	// フラグが変わった場合のみ処理
	if (m_isActive != flg)
	{
		m_isActive = flg;
		
		// 世界に復活させる
		if (flg)
		{
			m_cPhysics->AddBody();
		}
		// 世界から除外する
		else
		{
			m_cPhysics->RemoveBody();
		}
	}
}
