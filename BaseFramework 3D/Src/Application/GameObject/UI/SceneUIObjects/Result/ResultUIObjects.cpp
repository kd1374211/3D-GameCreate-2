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

	// ここにリザルト描画
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_ResultTop, ResultUIConsts::ResultTopTextPos, &kWhiteColor, "リザルト", TextAlign::Center);

	// ステージ名
	std::string text = info->m_stageName;
	color = kWhiteColor;
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Result_StageName, ResultUIConsts::StageNameTextPos , &color, text.c_str(), TextAlign::Center);

	// 各フレームごとに
	for (const auto& data : m_gameResult.m_frameResult)
	{
		int frameNumber = data.m_frameNumber;
		int throwNumber = 0;

		// フレーム番号表示
		std::string frameNumText = std::to_string(frameNumber + 1);
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_MiddleResultFrameNo, Math::Vector2(-360.0f + frameNumber * 80.0f + (frameNumber == BowlingSystemConsts::LastFrame ? 17.5f : 0), 20.0f), &kWhiteColor, frameNumText.c_str(), TextAlign::Center);

		// 投球スコア表示
		for (const auto& throwRec : data.m_throwRecord)
		{
			// スコア取得
			KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_MiddleResultThrowRecord, Math::Vector2(-377.5f + frameNumber * 80.0f + throwNumber * 35.0f, -20.0f), &kWhiteColor, throwRec.c_str(), TextAlign::Center);

			// ずらすためのカウント
			throwNumber++;
		}

		// フレームスコア表示
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_MiddleResultFrameScore, Math::Vector2(-360.0f + frameNumber * 80.0f + (frameNumber == BowlingSystemConsts::LastFrame ? 17.5f : 0), -100.0f), &kWhiteColor, data.m_totalScore.c_str(), TextAlign::Center);
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
