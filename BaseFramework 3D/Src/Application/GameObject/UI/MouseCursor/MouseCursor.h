#pragma once

class MouseCursor
{
public:

	MouseCursor() {}
	~MouseCursor(){}

	// カーソル描画
	void DrawCursor(const std::shared_ptr<KdTexture>& spTex, Math::Vector2 drawPos);

private:

};