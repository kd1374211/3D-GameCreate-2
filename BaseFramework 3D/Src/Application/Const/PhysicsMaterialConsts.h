#pragma once

// マテリアル種
enum class ModelMaterialType
{
	NormalGround,
	NormalWall,
	GutterGround,
	GutterWall,
	IslandGround,
	Number
};

// マテリアル毎の物理ステータス
struct MatPhysicsConst
{
	static constexpr float Friction[(size_t)ModelMaterialType::Number] = { 0.05f,0.05f,0.05f,0.05f,2.4f };
	static constexpr float Restitution[(size_t)ModelMaterialType::Number] = { 0.0f,0.9f,0.0f,0.0f,0.0f };
	static constexpr float RollingResistance[(size_t)ModelMaterialType::Number] = { -1.0f,-1.0f,1.2f,1.2f,4.0f };
};

static std::string PhysicMaterialName[(size_t)ModelMaterialType::Number] =
{
	"NormalGround",
	"NormalWall",
	"GutterGround",
	"GutterWall",
	"IslandGround"
};

static std::string PhysicMaterialKeyWord[(size_t)ModelMaterialType::Number] =
{
	"normalground",
	"normalwall",
	"gutterground",
	"gutterwall",
	"islandground"
};

struct MaterialNameCheck
{
	// 種類を取得
	static inline const ModelMaterialType GetMaterialType(const std::string& name)
	{
		// 全部を見て合った種類を返す
		for (size_t type = 0; type < (size_t)ModelMaterialType::Number; type++)
		{
			if (name == PhysicMaterialName[type])
			{
				return (ModelMaterialType)type;
			}
		}

		// 見つからなかったら通常地面を返す
		return ModelMaterialType::NormalGround;
	}

	// 地面チェック
 	static inline const bool IsGround(const std::string& name)
	{
		return name == PhysicMaterialName[(size_t)ModelMaterialType::NormalGround]
			|| name == PhysicMaterialName[(size_t)ModelMaterialType::GutterGround]
			|| name == PhysicMaterialName[(size_t)ModelMaterialType::IslandGround];
	}

	// 壁チェック
	static inline const bool IsWall(const std::string& name)
	{
		return name == PhysicMaterialName[(size_t)ModelMaterialType::NormalWall]
			|| name == PhysicMaterialName[(size_t)ModelMaterialType::GutterWall];
	}

	// 衝突音が鳴るかチェック
	static inline const bool IsImpactSound(const std::string& name)
	{
		return name == PhysicMaterialName[(size_t)ModelMaterialType::NormalGround]
			|| name == PhysicMaterialName[(size_t)ModelMaterialType::NormalWall]
			|| name == PhysicMaterialName[(size_t)ModelMaterialType::IslandGround];
	}
};

