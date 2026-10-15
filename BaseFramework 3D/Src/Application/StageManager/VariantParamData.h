#pragma once
#include <variant>

// 各ギミック固有のパラメータ構造体
struct FinishAreaParams
{
	// 位置・サイズ・回転のみなので固有パラメータは空でOK
};

struct RotatingParams
{
	std::string m_modelPath = "Asset/Models/Terrain/StageGimmicks/RotatingWall1/RotatingWall1.gltf";
	float m_rotateSpeed = 1.0f;
};

struct MovingParams
{
	enum class MovePattern { Linear, Easing } m_movePattern = MovePattern::Linear;
	std::string m_modelPath = "Asset/Models/Terrain/StageGimmicks/MovingWall1/MovingWall1.gltf";
	float m_speed = 1.0f;

	// 往復用
	Math::Vector3 m_startPos = { 0.0f, 0.0f, 0.0f };
	std::vector<Math::Vector3> m_waypoints;	// 中継点（任意数）
	Math::Vector3 m_goalPos = { 0.0f, 0.0f, 0.0f };
	float m_stopTime = 0.0f; // 各チェックポイントで停止している時間

};

// 3. バリアント型の定義
using GimmickParam = std::variant<FinishAreaParams, RotatingParams, MovingParams>;