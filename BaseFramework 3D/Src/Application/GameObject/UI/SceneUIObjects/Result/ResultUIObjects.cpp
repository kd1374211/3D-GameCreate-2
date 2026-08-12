#include "ResultUIObjects.h"

void ResultUIObject::DrawSprite()
{
	//仮画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_resultTex, 0, 0, 1280, 720);
}

void ResultUIObject::Init()
{
	//画像ロード
	m_resultTex = std::make_shared<KdTexture>();
	m_resultTex->Load("Asset/Textures/UI/SceneUI/Result/kResult.png");
}
