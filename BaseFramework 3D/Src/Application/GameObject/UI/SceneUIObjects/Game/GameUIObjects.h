#pragma once

enum class FrameMark;
class ScoreHandler;

//流れる文字（位置やサイズ固定）
struct MovingText
{
	bool m_isActive = false;		//有効フラグ
	float m_activeTime = 0.0f;		//有効時間
	float m_posX = 0.0f;			//X座標
	std::string m_text = "";//テキスト
	bool* m_endChecker = nullptr;	// 終了を受け取るための変数
};

class GameUIObjects :public KdGameObject
{
public:

	GameUIObjects() { Init(); }
	~GameUIObjects()override {}

	void Update()override;
	void DrawSprite()override;

	// 投球開始テキスト召喚
	void SpawnThrowStartText(int frameNo, int throwNo, bool* endCheck);

	// 中間リザルトテキスト召喚
	void SpawnMiddleResult();

	// 中間リザルト消滅確認
	bool GetIsMiddleResultActive()const { return m_isMiddleResultActive; }

	// 投球終了リザルト演出表示開始
	void SpawnThrowResultText(int fallenPins, FrameMark mark);

	// 投球終了テキストの消滅確認
	bool GetIsThrowRecordTextActive()const { return m_isThrowResultTextActive; }

	// ステージクリア・クリア失敗演出召喚(trueクリアfalse失敗)
	void SpawnStageFinishText(bool isClear);

	// スコアハンドラー登録
	void RegistScoreHandler(std::shared_ptr<ScoreHandler> spScoreHandler) { m_wpScoreHandler = spScoreHandler; }

private:

	// 各アップデート
	void UpdateThrowStartText(float dt);
	void UpdateThrowResult(float dt);
	void UpdateMiddleResult(float dt);

	// 描画
	void DrawScoreTexts();

	// GameUI内の仮定義
	struct GameUIConsts
	{
		// スタート演出
		static constexpr float ThrowStartTextStartX = -1020.0f;
		static constexpr float ThrowStartTextActiveSec = 0.9f;
		static constexpr float MoveSpeedFast = 5000.0f;
		static constexpr float MoveSpeedSlow = 80.0f;
		static constexpr float SlowMoveStartSec = 0.2f;
		static constexpr float SlowMoveEndSec = 0.7f;
		static constexpr float ThrowStartTextPosY = 0.0f;

		// ピン数
		static constexpr float PinPosY = 318.0f;
		static constexpr float PinIconPosX = 350.0f;
		static constexpr float PinTextPosX = 630.0f;

		// 投球リザルト演出
		static constexpr Math::Vector2 ThrowResultTestPos = Math::Vector2(0.0f, 0.0f);	// テキスト出現位置
		static constexpr float ThrowResultScaleExpandSpeed = 8.0f;						// テキスト拡大速度
		static constexpr float ThrowResultTextMaxScale = 1.0f;							// 最大スケール
		static constexpr float ThrowResultActiveEnd = 2.0f;								// テキスト表示時間

		// 中間リザルト
		static constexpr float MiddleResultPosX = 0.0f;
		static constexpr float MiddleResultStartY = -500.0f;
		static constexpr float MiddleResultEndY = 0.0f;
		static constexpr float MiddleResultMoveSpeed = 1500.0f;

		// 中間リザルト文字
		static constexpr float MiddleResultTextDrawPosDiffX_Frame = 80.0f;		// １フレームごとの描画位置ずれX
		static constexpr float DrawPosDiff_LastFrame = 20.0f;								// 最終フレームの描画位置ずれX
		static constexpr float FrameNumberDrawPosY = 80.0f;						// フレーム番号描画位置Y
		static constexpr float FrameNumberDrawStartPosX = -360.0f;				// フレーム番号描画位置Xスタート
		static constexpr float ThrowRecordDrawPosY = 20.0f;						// 投球スコアの描画位置Y
		static constexpr float ThrowRecordDrawStartPosX = -380.0f;				// 投球スコアの描画位置Xスタート
		static constexpr float ThrowRecordDrawPosDiffX_Throw = 40.0f;			// 投球スコアのX差(1-2-3投目)
		static constexpr float FrameScoreDrawPosY = -60.0f;						// フレームスコア描画位置Y
		static constexpr float FrameScoreDrawStartPosX = -360.0f;				// フレームスコア描画位置Xスタート

		//ステージ終了演出
		static constexpr float WindowExpandSpeed_StageFinish = 10.0f;
		static constexpr Math::Vector2 WindowSize = Math::Vector2(640.0f, 120.0f);
		static constexpr float WindowAlpha = 0.95f;

		// 操作ガイド
		static constexpr Math::Vector2 KeyGuideTextPos = Math::Vector2(-635.0f, -330.0f);
	};

	void Init()override;

	// 流れる文字
	MovingText m_throwStartText;

	//ピン画像
	std::shared_ptr<KdTexture> m_pinTex = nullptr;

	// 投球リザルト演出用テキスト
	bool m_isThrowResultTextActive = false;
	float m_throwResultTextScale = 0.0f;
	std::string m_throwRecordText = "";
	Math::Color m_throwRecordColor = {};
	float m_throwResultTextActiveTime = 0.0f;

	// 中間リザルト用
	bool m_isMiddleResultActive = false;
	float m_middleResultPosY = GameUIConsts::MiddleResultStartY;
	bool m_isMiddleResultUp = true;
	bool m_isMiddleResultUpEnd = false;

	std::shared_ptr<KdTexture> m_ResultWindowFrameTex = nullptr;

	//ステージ終了演出用
	bool m_isStageFinishTextDraw = false;
	bool m_isStageClear = false;
	float m_windowSize_stageFinish = 0.0f;

	// スコアハンドラー保持
	std::weak_ptr<ScoreHandler> m_wpScoreHandler;
};