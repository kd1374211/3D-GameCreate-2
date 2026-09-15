#pragma once

class ShotCursor :public KdGameObject
{
public:

	ShotCursor() { Init(); }
	~ShotCursor()override {}

	void Update()override;
	void DrawSprite()override;

	// 描画フラグセッター
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; }

	// 描画位置セッター
	void SetDrawPos(Math::Vector2 pos) { m_drawPos = pos; }

private:

	void Init()override;

	// リング
	std::shared_ptr<KdTexture> m_shotCursorTex = nullptr;

	// 描画フラグ
	bool m_isDraw = false;

	// 描画位置
	Math::Vector2 m_drawPos = Math::Vector2::Zero;
};