#pragma once

// マテリアル種
enum class PhysicMaterials
{
	NormalGround,
	NormalWall,
	GutterGround,
	GutterWall,
	Number
};

// マテリアル毎の物理ステータス
struct MatPhysicsConst
{
	static constexpr float Friction[(size_t)PhysicMaterials::Number] = { 0.05f,0.05f,0.05f,0.05f };
	static constexpr float Restitution[(size_t)PhysicMaterials::Number] = { 0.0f,0.9f,0.0f,0.0f };
};

static std::string PhysicMaterialName[(size_t)PhysicMaterials::Number] =
{
	"NormalGround",
	"NormalWall",
	"GutterGround",
	"GutterWall"
};

struct MaterialNameCheck
{
	// 地面チェック
 	static inline const bool IsGround(const std::string& name)
	{
		return name == PhysicMaterialName[(size_t)PhysicMaterials::NormalGround]
			|| name == PhysicMaterialName[(size_t)PhysicMaterials::GutterGround];
	}

	// 壁チェック
	static inline const bool IsWall(const std::string& name)
	{
		return name == PhysicMaterialName[(size_t)PhysicMaterials::NormalWall]
			|| name == PhysicMaterialName[(size_t)PhysicMaterials::GutterWall];
	}
};

