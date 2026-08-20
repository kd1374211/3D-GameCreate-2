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
	static constexpr float MovingTextSize = 200.0f;
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

	//時間描画
	void SetIsDrawTimer(bool flg) { m_isTimerDraw = flg; }

private:

	void Init()override;

	//タイマー画像
	std::shared_ptr<KdTexture> m_timerTex = nullptr;

	//タイマー描画フラグ
	bool m_isTimerDraw = false;

	//時間
	int m_time;

	//流れる文字
	MovingText m_movingTexts;
};