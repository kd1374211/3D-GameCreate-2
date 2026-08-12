#include "TitleUIObjects.h"

void TitleUIObject::DrawSprite()
{
	//仮画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_titleTex, 0, 0, 1280, 720);
}

void TitleUIObject::Init()
{
	//画像ロード
	m_titleTex = std::make_shared<KdTexture>();
	m_titleTex->Load("Asset/Textures/UI/SceneUI/Title/kTitle.png");
}
