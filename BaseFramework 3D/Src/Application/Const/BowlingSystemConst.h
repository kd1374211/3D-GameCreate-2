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
	static constexpr int MaxThrowCount_LastFrame = 3;
	static constexpr int MaxThrowCount_NotLastFrame = 2;


};

// フレームマーク
enum class FrameMark
{
	Strike,
	Spare,
	None
};

// スコア関連
struct ScoreDatas
{
	// 各フレームのデータ
	struct FrameScoreData
	{
		std::vector<std::string> m_throwRecord;		// 各投球データ
		std::string m_totalScore;					// このフレームまでの合計
		int m_frameNumber;							// フレーム番号
	};

	// ゲーム全体のデータ
	struct GameResult
	{
		std::array<FrameScoreData, BowlingSystemConsts::FrameCount> m_frameResult;		// 各フレームのリザルト
	};
};
