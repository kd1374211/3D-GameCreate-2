#include "ResultUIObjects.h"

#include "../../../../main.h"
#include "../../../../StageManager/StageManager.h"

void ResultUIObject::Update()
{
	//デルタタイム
	float dt = Application::Instance().GetDeltaTime();

	//ウィンドウ拡大
	if (m_windowSizeMulti < 1.0f)
	{
		m_windowSizeMulti += ResultUIConsts::WindowExpandSpeed * dt;
		if (m_windowSizeMulti >= 1.0f)m_windowSizeMulti = 1.0f;
	}

	//進捗度の制御
	m_progress += ResultUIConsts::AlphaChangeSpeed * dt * (m_isReverse * -2 + 1);

	if (m_progress >= ResultUIConsts::ProgMax)
	{
		m_isReverse = true;
	}
	else if (m_progress <= ResultUIConsts::ProgMin)
	{
		m_isReverse = false;
	}

	m_resultButtomTextAlpha = std::clamp(m_progress, ResultUIConsts::AlphaMin, ResultUIConsts::AlphaMax);
	KdDebugGUI::Instance().AddLog("Prog : %.2f\n", m_progress);
}

void ResultUIObject::DrawSprite()
{	
	//リザルト取得
	GameResult result = STAGEMGR.GetLastGameResult();
	//星数取得
	int starCount = STAGEMGR.GetLastGameStarCount();
	// ステージデータ
	auto* info = STAGEMGR.GetStageInfo();

	//ウィンドウ作る
	std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

	//レンダー作成
	Math::Vector2 renderBase = Math::Vector2(1280.0f, 720.0f);
	tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

	//透明塗り
	KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

	//ターゲット設定
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

	//ウィンドウ背景
	Math::Color color = Math::Color(0, 0, 0, 0.95f);
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, ResultUIConsts::WindowSize.x, ResultUIConsts::WindowSize.y, &color);

	// ここにクリアテキスト描画
	color = result.m_isCleared ? kGreenColor : kRedColor;
	std::string text = result.m_isCleared ? "STAGE  CLEAR!!" : "STAGE  FAILED...";
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_ResultTop, ResultUIConsts::ResultTopTextPos, &color, text.c_str(), TextAlign::Center);

	// ステージ名
	text = info->m_stageName;
	color = kWhiteColor;
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_StageName, ResultUIConsts::StageNameTextPos , &color, text.c_str(), TextAlign::Center);

	// 残り時間
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_clockTex, ResultUIConsts::TimeIconPos.x, ResultUIConsts::TimeIconPos.y, m_clockTex->GetWidth(), m_clockTex->GetWidth());
	// タイムアップチェック
	if (result.m_stageTimer <= 0.0f)
	{
		text = "Time Up...";
	}
	else
	{
		int timeLeft = std::ceill(result.m_stageTimer);
		text = std::to_string(timeLeft);
	}
	color = kWhiteColor;
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_TimeLeft, ResultUIConsts::TimeLeftTextPos, &color, text.c_str(), TextAlign::Right);

	// 全体・残りのピン
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_pinTex, ResultUIConsts::PinIconPos.x, ResultUIConsts::PinIconPos.y, m_pinTex->GetWidth(), m_pinTex->GetWidth());
	text = std::to_string(result.m_fallenPinCnt) + " / " + std::to_string(result.m_totalPinCnt);
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_PinCount, ResultUIConsts::PinCountTextPos, &color, text.c_str(), TextAlign::Right);

	// 星数
	Math::Vector2 starTexBaseSize = Math::Vector2(m_starTex->GetWidth() / (float)Stars::Number, m_starTex->GetHeight());
	for (int i = 0; i < StageManagerConsts::StarCountMax; i++)
	{
		// Y座標
		float drawPosY = ResultUIConsts::StarListBasePosY - ResultUIConsts::StarPosDiffY * i;

		// i(+1)番目の星が取れたか
		bool isStarGet = i < starCount;

		//星アイコン
		Math::Rectangle rec = Math::Rectangle((long)(starTexBaseSize.x * (int)(isStarGet ? Stars::Gold : Stars::Empty)), 0, (long)starTexBaseSize.x, (long)starTexBaseSize.y);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_starTex, ResultUIConsts::StarIconPosX, drawPosY, starTexBaseSize.x, starTexBaseSize.y, &rec);

		// 条件テキスト
		text = info->m_starTexts[i];
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_StarList, Math::Vector2(ResultUIConsts::StarListTextPosX, drawPosY), &kWhiteColor, text.c_str(), TextAlign::Left);
	}

	//リザルト下
	text = "[SPACE] 戻る";
	color = Math::Color(1.0f, 1.0f, 1.0f, m_resultButtomTextAlpha);
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_Buttom, ResultUIConsts::ResultEndTextPos, &color, text.c_str(), TextAlign::Center);

	//ターゲット戻す
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

	//tmpTexをサイズ変えて描画
	float drawSizeY = renderBase.y * m_windowSizeMulti;
	KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, 0, 0, renderBase.x, drawSizeY);
}

void ResultUIObject::Init()
{
	//画像
	m_clockTex = std::make_shared<KdTexture>();
	m_clockTex->Load("Asset/Textures/UI/SceneUI/ClockIcon.png");

	m_pinTex = std::make_shared<KdTexture>();
	m_pinTex->Load("Asset/Textures/UI/SceneUI/PinIcon.png");

	m_starTex = std::make_shared<KdTexture>();
	m_starTex->Load("Asset/Textures/UI/SceneUI/Result/ResultStars.png");
}
