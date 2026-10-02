#pragma once
#include "../../../../Const/BowlingSystemConst.h"

class Button;

// Result内の仮定義
struct ResultUIConsts
{
	// ウィンドウ本体
	static constexpr float WindowExpandSpeed = 4.0f;
	static constexpr Math::Vector2 WindowSize = Math::Vector2(500.0f, 300.0f);

	// テキスト
	static constexpr Math::Vector2 ResultTopTextPos = Math::Vector2(0.0f, 230.0f);
	static constexpr Math::Vector2 StageNameTextPos = Math::Vector2(0.0f, 160.0f);
	
	// ボタン
	static constexpr Math::Vector2 ButtonScale = Math::Vector2(2.5f, 2.5f);
	static constexpr Math::Vector2 ButtonPosOfs = Math::Vector2(0.0f, -200.0f);
};

class ResultUIObject :public KdGameObject
{
public:

	ResultUIObject() { Init(); }
	~ResultUIObject()override {}

	void Update()override;
	void DrawSprite()override;

	// スコア保持
	void SetGameResult(ScoreDatas::GameResult result) { m_gameResult = result; }

	// ボタンが押されたか
	bool IsButtonPressed()const { return m_isButtonPressed; }

private:

	void Init()override;

	// ウィンドウ
	float m_windowSizeMulti = 0.0f;

	// スコア保持用
	ScoreDatas::GameResult m_gameResult;

	// ボタン
	std::shared_ptr<Button> m_spButton = nullptr;

	// ボタンを押したことを確認するためのフラグ
	bool m_isButtonPressed = false;
};