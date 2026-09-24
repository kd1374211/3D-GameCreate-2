#pragma once

class ShotPowerBar :public KdGameObject
{
public:

	ShotPowerBar() { Init(); }
	~ShotPowerBar()override {}

	void DrawSprite()override;

	// セッター
	void SetPower(float power) { m_drawPower = power; }
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; }

private:

	void Init()override;

	// パワー
	float m_drawPower = 0.0f;

	// 画像
	std::shared_ptr<KdTexture> m_powerBarFrameTex;
	std::shared_ptr<KdTexture> m_powerBarTex;

	// 描画フラグ
	bool m_isDraw = false;

	struct ShotBarConsts
	{
		// 描画位置
		static constexpr Math::Vector2 DrawPos = Math::Vector2(-550, -120);
	};
};