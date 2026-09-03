#pragma once

struct BowlingSystemConsts
{
	// ピン数
	static constexpr int PinCount = 10;

	// フレーム数
	static constexpr int FrameCount = 10;
	static constexpr int StartFrame = 0;
	static constexpr int LastFrame = 9;

	// 投球数
	static constexpr int MaxThrowCount = 3;
};
