#include "MouseCursor.h"

void MouseCursor::DrawCursor(const std::shared_ptr<KdTexture>& spTex, Math::Vector2 drawPos)
{
	// 描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(spTex, drawPos.x, drawPos.y);
}
