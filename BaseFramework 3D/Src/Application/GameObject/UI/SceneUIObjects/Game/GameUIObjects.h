#pragma once

enum class FrameMark;

class GameUIObjects :public KdGameObject
{
public:

	GameUIObjects() { Init(); }
	~GameUIObjects()override {}

	void Update()override;
	void DrawSprite()override;

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

private:

	// GameUI内の仮定義
	struct GameUIConsts
	{
		//ピン数
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

		//ステージ終了演出
		static constexpr float WindowExpandSpeed_StageFinish = 10.0f;
		static constexpr Math::Vector2 WindowSize = Math::Vector2(640.0f, 120.0f);
		static constexpr float WindowAlpha = 0.95f;

		// 操作ガイド
		static constexpr Math::Vector2 KeyGuideTextPos = Math::Vector2(-635.0f, -330.0f);
	};

	void Init()override;

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

	//ステージ終了演出用
	bool m_isStageFinishTextDraw = false;
	bool m_isStageClear = false;
	float m_windowSize_stageFinish = 0.0f;
};