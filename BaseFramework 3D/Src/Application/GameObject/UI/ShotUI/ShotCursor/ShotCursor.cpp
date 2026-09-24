#include "ShotCursor.h"

void ShotCursor::Update()
{}

void ShotCursor::DrawSprite()
{
	// 描画フラグが無いならリターン
	if (!m_isDraw)return;

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_shotCursorTex, m_drawPos.x, m_drawPos.y);
}

void ShotCursor::Init()
{
	m_shotCursorTex = std::make_shared<KdTexture>();
	m_shotCursorTex->Load("Asset/Textures/ShotUI/Ring.png");
}
