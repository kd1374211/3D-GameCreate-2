#pragma once

class TitleUIObject :public KdGameObject
{
public:

	TitleUIObject() { Init(); }
	~TitleUIObject()override {}

	void DrawSprite()override;
	
	// スタートボタンが押されたか
	bool IsStartButtonPressed()const { return m_isStartButtonPressed; }

private:

	// クラス内の定数
	struct TitleUIConsts
	{
		// スタートボタン描画位置
		static constexpr Math::Vector2 StartButtonPos = Math::Vector2(0, -200.0f);

		// スタートボタン描画サイズ
		static constexpr Math::Vector2 StartButtonScale = Math::Vector2(2.0f, 2.0f);
	};

	void Init()override;

	//タイトル画像
	std::shared_ptr<KdTexture> m_titleLogoTex = nullptr;

	// スタートボタンが押されたかを判定するためのフラグ
	bool m_isStartButtonPressed = false;

};