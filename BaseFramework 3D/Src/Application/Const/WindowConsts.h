#pragma once

// ウィンドウサイズ
struct WindowSizeConsts
{
	// 位置関連
	static constexpr float WindowLeftX = -640.0f;
	static constexpr float WindowRightX = 640.0f;
	static constexpr float WindowTopY = 360.0f;
	static constexpr float WindowBottomY = -360.0f;

	// サイズ関連
	static constexpr Math::Vector2 WindowSize = Math::Vector2(1280.0f, 720.0f);
	static constexpr Math::Vector2 WindowSizeHalf = Math::Vector2(640.0f, 360.0f);
};
