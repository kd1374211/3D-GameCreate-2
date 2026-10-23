#include "TitleUIObjects.h"
#include "../../Button/Button.h"
#include "../../../../Scene/SceneManager.h"
#include "../../../../Const/MouseTexturesConst.h"

void TitleUIObject::DrawSprite()
{
	//タイトルロゴ画像
	Math::Vector2 texSize = Math::Vector2(m_titleLogoTex->GetWidth(), m_titleLogoTex->GetHeight());
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_titleLogoTex, 0, 150, texSize.x, texSize.y);

	// 操作ガイド
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_mouseIconTex_Select, TitleUIConsts::MouseGuideIconPos_Select.x, TitleUIConsts::MouseGuideIconPos_Select.y);
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Title_Guide, TitleUIConsts::MouseGuideTextPos_Select, &kWhiteColor, ": 決定", TextAlign::Right);
}

void TitleUIObject::Init()
{
	//画像ロード
	m_titleLogoTex = std::make_shared<KdTexture>();
	m_titleLogoTex->Load("Asset/Textures/UI/SceneUI/Title/TitleLogo.png");

	m_mouseIconTex_Select = std::make_shared<KdTexture>();
	m_mouseIconTex_Select->Load(GetMouseTexPath(MouseTexturesConsts::MouseTexType::LeftClick));

	// スタートボタン生成
	std::shared_ptr<Button> startButton = std::make_shared<Button>();
	startButton->SetDrawPos(TitleUIConsts::StartButtonPos);
	startButton->SetDrawScale(TitleUIConsts::StartButtonScale);
	startButton->SetButtonText("START");
	startButton->SetOnClickFlag(&m_isStartButtonPressed);
	SCENEMGR.AddObject(startButton);
}
