#pragma once
#include "../Const/BowlingSystemConst.h"

enum class NextActions
{
	None,
	NextThrow,
	NextFrame,
	BonusThrow,
	GameEnd
};

class ScoreManager
{
public:

	void Init();

	// スコアリセット
	void Reset();

	// 倒れたピンから現在の残りピン状況を推測し、ゲームの進行を制御する
	void RecordThrow(int fallenPins);

	// 次何かを取得
	NextActions GetNextAction()const { return m_nextAction; }

private:

	// 得点計算の更新
	void UpdateScore();

	// 次何かを確認
	void CheckNextAction();

	// 次の投球
	void NextThrow()
	{
		m_nextAction = NextActions::NextThrow;
		m_currentThrow++; 
	}

	// 次のフレーム
	void NextFrame()
	{
		m_nextAction = NextActions::NextFrame;
		m_currentFrame++;
		m_currentThrow = 0;
		m_currentPinFallen = 0;
	}

	// ボーナス投球
	void BonusThrow()
	{
		m_nextAction = NextActions::BonusThrow;
		m_currentThrow++;
		m_currentPinFallen = 0;
	}

	// ゲーム終了
	void GameEnd()
	{
		m_nextAction = NextActions::GameEnd;
		m_currentFrame++;
	}

	// このクラス内の定数
	struct ScoreManagerConsts
	{
		// 呼び方
		static constexpr int FirstThrow = 0;	//1投目（ストライク）
		static constexpr int SecondThrow = 1;	//2投目（スペア）
		static constexpr int BonusThrow = 2;	//3投目（ボーナス）
		static constexpr int LastFrame = 9;	//10フレーム目（最後）

		// データ
		static constexpr int EmptyDataID = -1;		// 投げてないデータに入れる値

		// スコア計算
		static constexpr int NextScoreAdd_Strike = 2;		// ストライクを取った時いくつ先までの得点を加えるか
		static constexpr int NextScoreAdd_Spare = 1;			// スペアを取った時いくつ先までの得点を加えるか
	};

	// フレームマーク
	enum class FrameMark
	{
		Strike,
		Spare,
		None
	};

	// 各フレームのデータ
	struct FrameData
	{
		// 投球関連
		size_t m_recordID[BowlingSystemConsts::MaxThrowCount];						// ピン数を記録しているvectorの番号
		FrameMark m_mark = FrameMark::None;											// このフレームのマーク
		size_t m_lastThrowID = ScoreManagerConsts::EmptyDataID;

		// スコア関連
		int m_frameTotalScore = 0;									// このフレームまでの合計点数
		bool m_isCalcEnd = false;									// このフレームの得点計算が終わったか
	};
	std::array<FrameData, BowlingSystemConsts::FrameCount> m_frameData;

	// 投球記録
	std::vector<int> m_throwRecord;

	// 現在のフレーム
	int m_currentFrame = 0;
	int m_currentThrow = 0;
	int m_currentPinFallen = 0;
	
	// ボーナス投球
	bool m_isBonusThrow = false;

	// 次のアクション
	NextActions m_nextAction = NextActions::None;

	ScoreManager() {}
	~ScoreManager() {}

public:

	static ScoreManager& Instance()
	{
		static ScoreManager instance;
		return instance;
	}

};

#define SCOREMGR ScoreManager::Instance()