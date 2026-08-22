#include "GameUIObjects.h"

#include "../../../../main.h"

void GameUIObjects::Update()
{
	//デルタタイム
	float dt = Application::Instance().GetDeltaTime();

	//流れるテキストアップデート
	if (m_movingTexts.m_isActive)
	{
		//未来を見る
		float currentActiveTime = m_movingTexts.m_activeTime;
		
		//移動距離用意
		float moveDist = 0.0f;

		if (currentActiveTime < GameUIConsts::SlowMoveStartSec)
		{
			float fastMoveTime = std::clamp(dt, 0.0f, GameUIConsts::SlowMoveStartSec - currentActiveTime);
			float slowMoveTime = dt - fastMoveTime;

			//高速
			moveDist += GameUIConsts::MoveSpeedFast * fastMoveTime;
			//低速
			moveDist += GameUIConsts::MoveSpeedSlow * slowMoveTime;
		}
		else if (currentActiveTime < GameUIConsts::SlowMoveEndSec)
		{
			float slowMoveTime = std::clamp(dt, 0.0f, GameUIConsts::SlowMoveEndSec - currentActiveTime);
			float fastMoveTime = dt - slowMoveTime;

			//高速
			moveDist += GameUIConsts::MoveSpeedFast * fastMoveTime;
			//低速
			moveDist += GameUIConsts::MoveSpeedSlow * slowMoveTime;
		}
		else
		{
			moveDist += GameUIConsts::MoveSpeedFast * dt;
		}

		//座標更新
		m_movingTexts.m_posX += moveDist;

		//時間更新
		m_movingTexts.m_activeTime += dt;

		//終了
		if (m_movingTexts.m_activeTime >= GameUIConsts::MovingTextActiveSec)m_movingTexts.m_isActive = false;
	}

	//ステージ終了テキスト更新
	if (m_isStageFinishTextDraw)
	{
		//拡大
		if (m_windowSize_stageFinish < 1.0f)
		{
			m_windowSize_stageFinish += GameUIConsts::WindowExpandSpeed_StageFinish * dt;
		}
	}
}

void GameUIObjects::DrawSprite()
{
	//流れテキスト
	if (m_movingTexts.m_isActive)
	{
		Math::Vector2 drawPos = Math::Vector2(m_movingTexts.m_posX, GameUIConsts::MovingTextPosY);
		KdShaderManager::Instance().m_spriteShader.DrawFont(GameUIConsts::MovingTextFontIndex, drawPos, &m_movingTexts.m_color, m_movingTexts.m_text.c_str());
	}

	//タイマー表示開始前はスキップ
	if (m_isTimerDraw) 
	{
		//仮に99は超えないようにする
		int drawTime = std::clamp(m_time, 0, 99);

		//位分解
		int digitTen = drawTime / 10;
		int digitOne = drawTime % 10;

		//10の位
		Math::Rectangle rec = Math::Rectangle(0, (long)(12 * digitTen), 12, 12);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, -42, 318, 84, 84, &rec);

		//1の位
		rec = Math::Rectangle(0, (long)(12 * digitOne), 12, 12);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, 42, 318, 84, 84, &rec);
	}

	//ステージ終了演出
	if (m_isStageFinishTextDraw)
	{
		std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

		//レンダー作成
		Math::Vector2 renderBase = Math::Vector2(1280.0f, 720.0f);
		tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

		//黒塗り
		KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

		//ターゲット設定
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

		// 背景黒塗り
		Math::Color color = Math::Color(0, 0, 0, 0.8f);
		KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, GameUIConsts::WindowSize.x, GameUIConsts::WindowSize.y, &color, true);

		// ここにクリアテキスト描画
		color = m_isStageClear ? kGreenColor : kRedColor;
		std::string text = m_isStageClear ? "STAGE  CLEAR!!" : "STAGE  FAILED...";
		KdShaderManager::Instance().m_spriteShader.DrawFont(GameUIConsts::FinishTextFontIndex, Math::Vector2::Zero, &color, text.c_str());

		//ターゲット戻す
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

		//tmpTexをサイズ変えて描画
		float drawSizeY = renderBase.y * m_windowSize_stageFinish;
		KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, 0, 0, renderBase.x, drawSizeY);
	}
}

void GameUIObjects::SpawnMovingText(std::string text, Math::Color color)
{
	//既にあっても上書き
	//値設定・有効化
	m_movingTexts.m_posX = GameUIConsts::MovingTextStartX;
	m_movingTexts.m_text = text;
	m_movingTexts.m_color = color;
	m_movingTexts.m_activeTime = 0.0f;
	m_movingTexts.m_isActive = true;
}

void GameUIObjects::SpawnStageFinishText(bool isClear)
{
	m_isStageClear = isClear;
	m_isStageFinishTextDraw = true;
	m_windowSize_stageFinish = 0.0f;
}

void GameUIObjects::Init()
{
	m_timerTex = std::make_shared<KdTexture>();
	m_timerTex->Load("Asset/Textures/UI/SceneUI/Game/Number.png");

	m_movingTexts.m_isActive = false;
}
