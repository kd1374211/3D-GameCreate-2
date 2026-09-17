#pragma once
#include <variant>

// 各ギミック固有のパラメータ構造体
struct FinishAreaParams
{
	// 位置・サイズ・回転のみなので固有パラメータは空でOK
};

struct RotatingParams
{
	std::string m_modelPath = "";
	float m_rotateSpeed = 1.0f;
};

struct MovingParams
{
	enum class Mode { PingPong, Waypoint } m_mode = Mode::PingPong;
	std::string m_modelPath = "";
	float m_speed = 1.0f;
	int m_movePattern = 0;

	// 往復用
	Math::Vector3 m_startPos = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 m_goalPos = { 0.0f, 0.0f, 0.0f };

	// 中継用（任意数のポイント）
	std::vector<Math::Vector3> m_waypoints;
};

// 3. バリアント型の定義
using GimmickParam = std::variant<FinishAreaParams, RotatingParams, MovingParams>;