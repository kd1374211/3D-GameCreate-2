#pragma once

enum class Stars
{
	Gold,
	Empty,
	Number
};

// Result内の仮定義
struct ResultUIConsts
{
	// ウィンドウ本体
	static constexpr float WindowExpandSpeed = 4.0f;
	static constexpr Math::Vector2 WindowSize = Math::Vector2(500.0f, 300.0f);

	// テキスト
	static constexpr Math::Vector2 ResultTopTextPos = Math::Vector2(0.0f, 230.0f);
	static constexpr Math::Vector2 StageNameTextPos = Math::Vector2(0.0f, 160.0f);
	static constexpr Math::Vector2 TimeLeftTextPos = Math::Vector2(-70.0f, 50.0f);
	static constexpr Math::Vector2 PinCountTextPos = Math::Vector2(450.0f, 50.0f);
	static constexpr float StarListTextPosX = -150.0f;
	
	// 画像
	static constexpr Math::Vector2 TimeIconPos = Math::Vector2(-400.0f, 50.0f);
	static constexpr Math::Vector2 PinIconPos = Math::Vector2(120.0f, 50.0f);
	static constexpr float StarIconPosX = -200.0f;

	// 星
	static constexpr float StarListBasePosY = -30.0f;
	static constexpr float StarPosDiffY = 60.0f;

	// ボタン
	static constexpr Math::Vector2 ButtonSize = Math::Vector2(0.0f, 170.0f);
	static constexpr Math::Vector2 RestartButtonPos = Math::Vector2(0.0f, 170.0f);
	static constexpr Math::Vector2 BackButtonPos = Math::Vector2(0.0f, 170.0f);
};

class ResultUIObject :public KdGameObject
{
public:

	ResultUIObject() { Init(); }
	~ResultUIObject()override {}

	void Update()override;
	void DrawSprite()override;

private:

	void Init()override;

	// ウィンドウ
	float m_windowSizeMulti = 0.0f;

	// 時計アイコン
	std::shared_ptr<KdTexture> m_clockTex = nullptr;

	// ピンアイコン
	std::shared_ptr<KdTexture> m_pinTex = nullptr;

	// ★アイコン
	std::shared_ptr<KdTexture> m_starTex = nullptr;

};