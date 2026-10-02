#pragma once

class Button :public KdGameObject
{
public:

	Button() { Init(); }
	~Button()override {}

	void Update()override;
	void DrawSprite()override;

	// 描画位置セッター
	void SetDrawPos(Math::Vector2 pos) { m_drawPos = pos; }

	// 描画サイズセッター
	void SetDrawScale(Math::Vector2 scale) { m_drawScale = scale; }

	// ボタンを押したときにオンにするフラグを設定
	void SetOnClickFlag(bool* flg) { m_onClickFlg = flg; }

	// ボタン上のテキストを設定
	void SetButtonText(const std::string& text) { m_buttonText = text; }

	// 判定の有効・無効を切り替え
	void SetIsEnable(bool flg) { m_isEnable = flg; }

private:

	struct ButtonConsts
	{
		// ボタン押下時の拡大率
		static constexpr float MaxSizeMulti = 1.05f;
		static constexpr float MinSizeMulti = 1.0f;

		// ボタン押下時の拡大率の変化速度
		static constexpr float SizeChangeSpeed = 1.0f;

		// ボタンの基礎サイズ
		static constexpr Math::Vector2 BaseButtonSize = Math::Vector2(128.0f, 32.0f);
	};

	// ボタンの当たり判定
	#define HITSIZE (ButtonConsts::BaseButtonSize * m_drawScale * 0.5f)

	void Init()override;

	// ボタンを押したときに遠隔でオンにするフラグ
	bool* m_onClickFlg = nullptr;

	// 描画位置
	Math::Vector2 m_drawPos = Math::Vector2::Zero;

	// 描画サイズ
	Math::Vector2 m_drawScale = Math::Vector2::Zero;

	// ボタンのテクスチャ
	std::shared_ptr<KdTexture> m_buttonTex = nullptr;

	// ボタンの拡大率
	float m_sizeMulti = ButtonConsts::MinSizeMulti;

	// ボタンにカーソルが乗っているか
	bool m_isCursor = false;

	// ボタン上のテキスト
	std::string m_buttonText = "Button";

	// ボタンが有効か
	bool m_isEnable = true;
};
