#pragma once

//流れる文字（位置やサイズ固定）
struct MovingText
{
	bool m_isActive;		//有効フラグ
	float m_activeTime;		//有効時間
	float m_posX;			//X座標
	Math::Color m_color;	//色
	std::string m_text = "";//テキスト
};

// GameUI内の仮定義
struct GameUIConsts
{
	// 流れる文字
	static constexpr float MovingTextStartX = -1020.0f;
	static constexpr float MovingTextActiveSec = 0.9f;
	static constexpr float MoveSpeedFast = 5000.0f;
	static constexpr float MoveSpeedSlow = 80.0f;
	static constexpr float SlowMoveStartSec = 0.2f;
	static constexpr float SlowMoveEndSec = 0.7f;
	static constexpr float MovingTextPosY = 0.0f;

	//タイマーとピン共通
	static constexpr float TimerPinPosY = 318.0f;

	//タイマー
	static constexpr float ClockIconPosX = -100.0f;
	static constexpr float TimeNumberTextPosX = 50.0f;

	//ピン数
	static constexpr float PinIconPosX = 350.0f;
	static constexpr float PinTextPosX = 630.0f;

	//ステージ終了演出
	static constexpr float WindowExpandSpeed_StageFinish = 10.0f;
	static constexpr Math::Vector2 WindowSize = Math::Vector2(640.0f, 120.0f);
	static constexpr float WindowAlpha = 0.95f;

	// 操作ガイド
	static constexpr Math::Vector2 KeyGuideTextPos = Math::Vector2(-635.0f, -330.0f);
};

class GameUIObjects :public KdGameObject
{
public:

	GameUIObjects() { Init(); }
	~GameUIObjects()override {}

	void Update()override;
	void DrawSprite()override;

	//残り時間セット
	void SetTimer(int time) { m_time = time; }

	// 流れる文字召喚（カウントダウン用）
	void SpawnMovingText(std::string text, Math::Color color);

	// ステージクリア・クリア失敗演出召喚(trueクリアfalse失敗)
	void SpawnStageFinishText(bool isClear);

	//時間描画
	void SetIsDrawTimer(bool flg) { m_isTimerDraw = flg; }

private:

	void Init()override;

	//時計画像
	std::shared_ptr<KdTexture> m_clockTex = nullptr;

	//ピン画像
	std::shared_ptr<KdTexture> m_pinTex = nullptr;

	//タイマー描画フラグ
	bool m_isTimerDraw = false;

	//時間
	int m_time;

	//流れる文字
	MovingText m_movingTexts;

	//ステージ終了演出用
	bool m_isStageFinishTextDraw = false;
	bool m_isStageClear = false;
	float m_windowSize_stageFinish = 0.0f;
};