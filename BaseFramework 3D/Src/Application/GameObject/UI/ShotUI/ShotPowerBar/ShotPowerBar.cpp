#include "ShotPowerBar.h"

void ShotPowerBar::DrawSprite()
{
	// 描画オフならリターン
	if (!m_isDraw)return;

	// フレーム描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_powerBarFrameTex, ShotBarConsts::DrawPos.x, ShotBarConsts::DrawPos.y);

	// パワーから描画切り取り
	float UVStartY = m_powerBarTex->GetHeight() * (1.0f - m_drawPower);
	float DrawPosY = ShotBarConsts::DrawPos.y - UVStartY / 2.0f;
	Math::Vector2 DrawSize = Math::Vector2(m_powerBarTex->GetWidth(), m_powerBarTex->GetHeight() - UVStartY);
	Math::Rectangle rec = Math::Rectangle(0, UVStartY, DrawSize.x, DrawSize.y);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_powerBarTex, ShotBarConsts::DrawPos.x, DrawPosY, DrawSize.x, DrawSize.y, &rec);
}

void ShotPowerBar::Init()
{
	m_powerBarFrameTex = std::make_shared<KdTexture>();
	m_powerBarFrameTex->Load("Asset/Textures/ShotUI/PowerBarFrame.png");

	m_powerBarTex = std::make_shared<KdTexture>();
	m_powerBarTex->Load("Asset/Textures/ShotUI/PowerBar.png");
}
