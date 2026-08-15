#include "TitleUIObjects.h"

void TitleUIObject::Update()
{
	//スタートボタン透明度
	const float alphaMax = 1.0f; //最大
	const float alphaMin = 0.2f; //最小
	const float alphaChange = 0.01f; //変化値

	//変化
	m_startAlpha += alphaChange * m_startAlphaChangeMulti;

	//反転チェック
	if (m_startAlpha >= alphaMax || m_startAlpha <= alphaMin)
	{
		m_startAlphaChangeMulti *= -1;
	}
}

void TitleUIObject::DrawSprite()
{
	//タイトルロゴ画像
	Math::Vector2 texSize = Math::Vector2(m_titleLogoTex->GetWidth(), m_titleLogoTex->GetHeight());
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_titleLogoTex, 0, 150, texSize.x, texSize.y);

	//スタートボタン
	texSize = Math::Vector2(m_startTex->GetWidth(), m_startTex->GetHeight());
	Math::Color color = Math::Color(1.0f, 1.0f, 1.0f, m_startAlpha);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_startTex, 0, -250, texSize.x, texSize.y, nullptr, &color);
}

void TitleUIObject::Init()
{
	//画像ロード
	m_titleLogoTex = std::make_shared<KdTexture>();
	m_titleLogoTex->Load("Asset/Textures/UI/SceneUI/Title/TitleLogo.png");

	m_startTex = std::make_shared<KdTexture>();
	m_startTex->Load("Asset/Textures/UI/SceneUI/Title/PressStart.png");
}
