#pragma once

// マテリアル種
enum class PhysicMaterials
{
	NormalGround,
	NormalWall,
	Number
};

static std::string PhysicMaterialName[(size_t)PhysicMaterials::Number] =
{
	"Ground",
	"Wall"
};