#pragma once

struct MouseTexturesConsts
{
	enum class MouseTexType
	{
		LeftClick,
		LeftClickRelease,
		Move,
		Move_Horizonal,
		UpAndRightClickRelease,
		Number
	};

	static inline std::map<MouseTexType, std::string> MouseTypeTexPath =
	{
		{MouseTexType::LeftClick,"Asset/Textures/UI/Mouse/MouseLeftClick.png"},
		{MouseTexType::LeftClickRelease,"Asset/Textures/UI/Mouse/MouseLeftRelease.png"},
		{MouseTexType::Move,"Asset/Textures/UI/Mouse/MouseMove.png"},
		{MouseTexType::Move_Horizonal,"Asset/Textures/UI/Mouse/MouseMoveHorizonal.png"},
		{MouseTexType::UpAndRightClickRelease,"Asset/Textures/UI/Mouse/MouseUpAndRelease.png"},
	};
};

static inline const std::string& GetMouseTexPath(MouseTexturesConsts::MouseTexType type)
{
	// 捜索
	const auto& found = MouseTexturesConsts::MouseTypeTexPath.find(type);

	// 見つかれば
	if (found != MouseTexturesConsts::MouseTypeTexPath.end())
	{
		return found->second;
	}

	// 見つからなければ
	return "Error";
}