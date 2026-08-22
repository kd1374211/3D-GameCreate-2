#include "ResultUIObjects.h"

void ResultUIObject::DrawSprite()
{	
	//ウィンドウ背景
	Math::Color color = Math::Color(0, 0, 0, 0.92f);
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 250, 300, &color);

	//リザルト
	KdShaderManager::Instance().m_spriteShader.DrawFont(4, Math::Vector2(0, 250.0f), & kWhiteColor, "リザルト");
}

void ResultUIObject::Init()
{
	
}
