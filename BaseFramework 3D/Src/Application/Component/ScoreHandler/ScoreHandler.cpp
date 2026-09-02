#include "ScoreHandler.h"

void ScoreHandler::Init()
{
	Reset();
}

void ScoreHandler::Reset()
{
	// フレームデータ初期化
	for (size_t frameCnt = 0; frameCnt < BowlingSystemConsts::FrameCount; frameCnt++)
	{
		for (size_t throwCnt = 0; throwCnt < BowlingSystemConsts::MaxThrowCount; throwCnt++)
		{
			m_frameData[frameCnt].m_recordID[throwCnt] = ScoreHandlerConsts::EmptyDataID;
		}
		m_frameData[frameCnt].m_mark = FrameMark::None;
		m_frameData[frameCnt].m_frameTotalScore = 0;
		m_frameData[frameCnt].m_isCalcEnd = false;
		m_frameData[frameCnt].m_lastThrowID = ScoreHandlerConsts::EmptyDataID;
	}

	// 投球記録削除
	m_throwRecord.clear();

	// 投球数とフレーム番号初期化
	m_currentFrame = 0;
	m_currentThrow = 0;
	m_currentPinFallen = 0;

	// ボーナスリセット
	m_isBonusThrow = false;

	// 次のアクションも初期化
	m_nextAction = NextActions::None;
}

void ScoreHandler::RecordThrow(int fallenPins)
{
	FrameData* frameData = &m_frameData[m_currentFrame];
	
	// 1. ピン数の記録
	// 記録登録前のサイズをIDとして取得
	int ID = m_throwRecord.size();
	m_throwRecord.push_back(fallenPins);
	
	// IDをデータに追加
	frameData->m_recordID[m_currentThrow] = ID;
	// このフレーム最後のIDを更新
	frameData->m_lastThrowID = ID;

	m_currentPinFallen += fallenPins;

	// 2. マークの判定
	if (m_currentThrow == ScoreHandlerConsts::FirstThrow)
	{
		// 1投目で10本倒したら無条件でストライク
		if (fallenPins >= BowlingSystemConsts::PinCount)
		{
			frameData->m_mark = FrameMark::Strike;
		}
	}
	else if (m_currentThrow == ScoreHandlerConsts::SecondThrow)
	{
		// 2投目は「1投目でストライクを取っていない」かつ「2投目までの合計が10本」ならスペア
		if (frameData->m_mark != FrameMark::Strike &&
			m_currentPinFallen >= BowlingSystemConsts::PinCount)
		{
			frameData->m_mark = FrameMark::Spare;
		}
	}

	// 3. 次のアクション確認＆状態更新
	CheckNextAction();

	// 4. 得点計算の更新
	UpdateScore();
}

void ScoreHandler::UpdateScore()
{
	// 終了したフレームまで
	for (size_t checkFrame = 0; checkFrame < m_currentFrame; checkFrame++)
	{
		// 現在フレームデータ
		FrameData& currentData = m_frameData[checkFrame];

		// 得点未計算か
		if (!currentData.m_isCalcEnd)
		{
			// ここまでの点数
			int totalScore = 0;

			// 得点計算
			// 最初のフレームでないなら１つ前のフレームの得点計算が終わっているか
			// いなければこれ以上繰り返す必要がない（そもそも仕様上ここのreturnが呼ばれたらおかしい）
			if (checkFrame > 0)
			{
				// 前フレームデータ
				FrameData pastData = m_frameData[checkFrame - 1];

				if (!pastData.m_isCalcEnd)return;
				// 終わっているならそのスコアを取得
				else
				{
					totalScore += pastData.m_frameTotalScore;
				}
			}

			// このフレームのピン数を追加
			for (size_t checkThrow = 0; checkThrow < BowlingSystemConsts::MaxThrowCount; checkThrow++)
			{
				// もしIDが-1の場合これより後は投げていないので飛ばす
				if (currentData.m_recordID[checkThrow] == ScoreHandlerConsts::EmptyDataID)break;

				totalScore += m_throwRecord[currentData.m_recordID[checkThrow]];
			}

			// これが最後のフレームなら計算終了
			if (checkFrame == ScoreHandlerConsts::LastFrame)
			{
				currentData.m_frameTotalScore = totalScore;
				currentData.m_isCalcEnd = true;
				return;
			}

			// 現在の投球データのサイズを取得
			size_t currentThrowRecSize = m_throwRecord.size();
			size_t baseID = currentData.m_lastThrowID;
			size_t checkID = baseID;

			// このフレームのマークを確認
			switch (currentData.m_mark)
			{
			// ストライクなら２つ先までのデータを確認する
			case FrameMark::Strike:
				// データが足りない場合はリターン
				if (baseID + ScoreHandlerConsts::NextScoreAdd_Strike > currentThrowRecSize)return;

				// データ取得
				for (size_t check = 0; check < ScoreHandlerConsts::NextScoreAdd_Strike; check++)
				{
					checkID++;
					totalScore += m_throwRecord[checkID];
				}

				break;
				// スペアなら次のデータを確認する
			case FrameMark::Spare:
				// データが足りない場合はリターン
				if (baseID + ScoreHandlerConsts::NextScoreAdd_Spare > currentThrowRecSize)return;

				// データ取得
				for (size_t check = 0; check < ScoreHandlerConsts::NextScoreAdd_Spare; check++)
				{
					checkID++;
					totalScore += m_throwRecord[checkID];
				}

				break;
			// マーク無しなら何もしない
			case FrameMark::None:
				break;
			}

			// 計算後の得点を格納し計算完了フラグオン
			currentData.m_frameTotalScore = totalScore;
			currentData.m_isCalcEnd = true;
		}
	}
}

void ScoreHandler::CheckNextAction()
{
	// ストライクorスペア確認
	// かつ現在がボーナスでないとき
	if (m_frameData[m_currentFrame].m_mark != FrameMark::None && !m_isBonusThrow)
	{
		// 最終フレームならボーナス投球
		if (m_currentFrame == ScoreHandlerConsts::LastFrame)
		{
			BonusThrow();
			m_isBonusThrow = true;
		}
		// でないなら次のフレームに移行
		else
		{
			NextFrame();
		}
	}
	else
	{
		// 現在の投球番号を確認
		switch (m_currentThrow)
		{
			// 1投目は次の投球に進む
		case ScoreHandlerConsts::FirstThrow:
			NextThrow();
			break;
			// 2投目はボーナスかを確認する
		case ScoreHandlerConsts::SecondThrow:
			// 最終フレームかどうかを確認
			if (m_currentFrame == ScoreHandlerConsts::LastFrame)
			{
				// 最終フレームならボーナス確認
				if (m_isBonusThrow)
				{
					BonusThrow();
				}
				else
				{
					GameEnd();
				}
			}
			// でないなら次のフレーム
			else
			{
				NextFrame();
			}
			break;
			// 3投目はゲーム終了確定
		case ScoreHandlerConsts::BonusThrow:
			GameEnd();
			break;
		}
	}
}
