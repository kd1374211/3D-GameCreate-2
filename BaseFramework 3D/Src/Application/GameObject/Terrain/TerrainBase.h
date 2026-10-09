#pragma once
#include "../../Component/PhysicsComponent/PhysicsComponent.h"

enum class PhysicMaterials
{
	NormalGround,
	NormalWall,
	Number
};

// マテリアル毎の物理ステータス
struct MatPhysicsConst
{
	static constexpr float Friction[(size_t)PhysicMaterials::Number] = { 0.05f,0.05f };
	static constexpr float Restitution[(size_t)PhysicMaterials::Number] = { 0.0f,0.9f };
};

class TerrainBase :public KdGameObject
{
public:

	TerrainBase() {}
	virtual ~TerrainBase()override {}

	void Update()override;
	virtual void DrawLit()override;
	virtual void GenerateDepthMapFromLight()override;

	//セッター
	void SetPos(const Math::Vector3& a_pos) { m_pos = a_pos; }

protected:

	std::shared_ptr<KdModelData> m_model = nullptr;
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Quaternion m_rot = Math::Quaternion::Identity;
	Math::Vector3 m_scale = Math::Vector3::One;

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;
};