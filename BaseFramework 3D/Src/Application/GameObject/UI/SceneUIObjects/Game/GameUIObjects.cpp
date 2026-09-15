#include "GameUIObjects.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../Const/BowlingSystemConst.h"
#include "../../../../main.h"
#include "../../../../Const/DeviceAndKey.h"

void GameUIObjects::Update()
{
	//デルタタイム
	float dt = Application::Instance().GetDeltaTime();

	//流れるテキストアップデート
	if (m_throwStartText.m_isActive)
	{
		UpdateThrowStartText(dt);
	}

	// 投球終了テキスト更新
	if (m_isThrowResultTextActive)
	{
		UpdateThrowResult(dt);
	}

	// 中間リザルト更新
	if (m_isMiddleResultActive)
	{
		UpdateMiddleResult(dt);
	}

	//ステージ終了テキスト更新
	if (m_isStageFinishTextDraw)
	{
		//拡大
		if (m_windowSize_stageFinish < 1.0f)
		{
			m_windowSize_stageFinish += GameUIConsts::WindowExpandSpeed_StageFinish * dt;
			if (m_windowSize_stageFinish >= 1.0f)m_windowSize_stageFinish = 1.0f;
		}
	}
}

void GameUIObjects::DrawSprite()
{
	//流れテキスト
	if (m_throwStartText.m_isActive)
	{
		Math::Vector2 drawPos = Math::Vector2(m_throwStartText.m_posX, GameUIConsts::ThrowStartTextPosY);
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_CountDown, drawPos, &kWhiteColor, m_throwStartText.m_text.c_str(),TextAlign::Center);
	}

	//// 操作ガイド
	//KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_KeyGuide, GameUIConsts::KeyGuideTextPos, &kBlackColor, "[←/→] 方向転換", TextAlign::Left);

	////ステージ終了演出
	//if (m_isStageFinishTextDraw)
	//{
	//	std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

	//	//レンダー作成
	//	Math::Vector2 renderBase = Math::Vector2(1280.0f, 720.0f);
	//	tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

	//	//黒塗り
	//	KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

	//	//ターゲット設定
	//	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

	//	// 背景黒塗り
	//	Math::Color color = Math::Color(0, 0, 0, GameUIConsts::WindowAlpha);
	//	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, GameUIConsts::WindowSize.x, GameUIConsts::WindowSize.y, &color, true);

	//	// ここにクリアテキスト描画
	//	color = m_isStageClear ? kGreenColor : kRedColor;
	//	std::string text = m_isStageClear ? "STAGE  CLEAR!!" : "STAGE  FAILED...";
	//	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_StageFinish, Math::Vector2::Zero, &color, text.c_str(), TextAlign::Center);

	//	//ターゲット戻す
	//	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

	//	//tmpTexをサイズ変えて描画
	//	float drawSizeY = renderBase.y * m_windowSize_stageFinish;
	//	KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, 0, 0, renderBase.x, drawSizeY);
	//}

	// 投球リザルトテキスト
	if (m_isThrowResultTextActive)
	{
		std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

		//レンダー作成
		Math::Vector2 renderBase = Math::Vector2(1280.0f, 720.0f);
		tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

		//透明塗り
		KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

		//ターゲット設定
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

		// テキスト描画
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Game_ThrowResult, GameUIConsts::ThrowResultTestPos, &m_throwRecordColor, m_throwRecordText.c_str(), TextAlign::Center);

		//ターゲット戻す
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

		//tmpTexをサイズ変えて描画
		Math::Vector2 drawSize = renderBase * m_throwResultTextScale;
		KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, 0, 0, drawSize.x, drawSize.y);
	}

	// 中間リザルトウィンドウ
	if (m_isMiddleResultActive)
	{
		std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

		//レンダー作成
		Math::Vector2 renderBase = Math::Vector2(1280.0f, 720.0f);
		tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

		//透明塗り
		KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

		//ターゲット設定
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

		// ここに描画
		// ウィンドウ本体
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_middleResultWindowTexK, 0, 0);

		//ターゲット戻す
		KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

		//tmpTexを位置を変えて描画
		Math::Vector2 drawPos = Math::Vector2(GameUIConsts::MiddleResultPosX, m_middleResultPosY);
		KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, drawPos.x, drawPos.y, renderBase.x, renderBase.y);
	}
}

void GameUIObjects::SpawnThrowStartText(int frameNo, int throwNo, bool* endCheck)
{
	// 既にあったらリターン
	if (m_throwStartText.m_isActive)return;

	// 初期化
	m_throwStartText.m_activeTime = 0.0f;
	m_throwStartText.m_posX = GameUIConsts::ThrowStartTextStartX;
	m_throwStartText.m_text = "";

	// 引数からテキストを生成
	std::string text = "";
	// まだ仮
	switch (throwNo)
	{
	case 0:
		text = "First Throw";
		break;
	case 1:
		text = "Second Throw";
		break;
	case 2:
		text = "Bonus Throw";
		break;
	default:
		text = "Error";
		break;
	}
	m_throwStartText.m_text = text;

	// 終了確認フラグ設定
	m_throwStartText.m_endChecker = endCheck;

	// アクティブ化
	m_throwStartText.m_isActive = true;
}

void GameUIObjects::SpawnThrowResultText(int fallenPins, FrameMark mark)
{
	// もし出現済みならリターン
	if (m_isThrowResultTextActive)return;

	// 値初期化
	m_throwResultTextScale = 0.0f;
	m_throwResultTextActiveTime = 0.0f;
	
	// 受け取ったリザルトをもとにテキスト決定
	m_throwRecordText = "";
	m_throwRecordColor = kWhiteColor;
	switch (mark)
	{
	case FrameMark::Strike:
		m_throwRecordText = "Strike!!";
		m_throwRecordColor = Math::Color(1.0f, 0.85f, 0.0f, 1.0f);
		break;
	case FrameMark::Spare:
		m_throwRecordText = "Spare!";
		m_throwRecordColor = Math::Color(0.3f, 0.75f, 1.0f, 1.0f);
		break;
	case FrameMark::None:
		// マーク無しの場合は0本かどうかでさらに分岐
		if (fallenPins == 0)
		{
			m_throwRecordText = "Miss...";
			m_throwRecordColor = Math::Color(0.6f, 0.6f, 0.6f, 1.0f);
		}
		else
		{
			m_throwRecordText = std::to_string(fallenPins) + " Pins";
			m_throwRecordColor = kWhiteColor;
		}
		break;
	}

	// アクティブ化
	m_isThrowResultTextActive = true;
}

void GameUIObjects::SpawnMiddleResult()
{
	// もし出現済みならリターン
	if (m_isMiddleResultActive)return;

	// 値、フラグリセット
	m_middleResultPosY = GameUIConsts::MiddleResultStartY;
	m_isMiddleResultUp = true;
	m_isMiddleResultUpEnd = false;

	// 有効化
	m_isMiddleResultActive = true;
}

void GameUIObjects::SpawnStageFinishText(bool isClear)
{
	m_isStageClear = isClear;
	m_isStageFinishTextDraw = true;
	m_windowSize_stageFinish = 0.0f;
}

void GameUIObjects::UpdateThrowStartText(float dt)
{
	//未来を見る
	float currentActiveTime = m_throwStartText.m_activeTime;

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
	m_throwStartText.m_posX += moveDist;

	//時間更新
	m_throwStartText.m_activeTime += dt;

	//終了
	if (m_throwStartText.m_activeTime >= GameUIConsts::ThrowStartTextActiveSec)
	{
		*m_throwStartText.m_endChecker = true;
		m_throwStartText.m_isActive = false;
	}
}

void GameUIObjects::UpdateThrowResult(float dt)
{
	// 拡大
	if (m_throwResultTextScale < GameUIConsts::ThrowResultTextMaxScale)
	{
		m_throwResultTextScale += GameUIConsts::ThrowResultScaleExpandSpeed * dt;

		// 補正
		if (m_throwResultTextScale >= GameUIConsts::ThrowResultTextMaxScale)
		{
			m_throwResultTextScale = GameUIConsts::ThrowResultTextMaxScale;
		}
	}

	// 時間更新
	m_throwResultTextActiveTime += dt;

	// 終了
	if (m_throwResultTextActiveTime > GameUIConsts::ThrowResultActiveEnd)
	{
		m_isThrowResultTextActive = false;
	}
}

void GameUIObjects::UpdateMiddleResult(float dt)
{
	// リザルト閉じチェック
	if (m_isMiddleResultUpEnd && m_isMiddleResultUp)
	{
		if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_SPACE)))
		{
			m_isMiddleResultUp = false;
		}
	}

	// 位置更新
	if (m_isMiddleResultUp)
	{
		// 上
		// スキップ用
		if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_SPACE)))
		{
			m_middleResultPosY = GameUIConsts::MiddleResultEndY;
			m_isMiddleResultUpEnd = true;
		}

		// 通常
		if (m_middleResultPosY < GameUIConsts::MiddleResultEndY)
		{
			m_middleResultPosY += GameUIConsts::MiddleResultMoveSpeed * dt;

			// 補正
			if (m_middleResultPosY >= GameUIConsts::MiddleResultEndY)
			{
				m_middleResultPosY = GameUIConsts::MiddleResultEndY;
				m_isMiddleResultUpEnd = true;
			}
		}
	}
	else
	{
		// 下
		// 通常
		if (m_middleResultPosY > GameUIConsts::MiddleResultStartY)
		{
			m_middleResultPosY -= GameUIConsts::MiddleResultMoveSpeed * dt;

			// 終了
			if (m_middleResultPosY <= GameUIConsts::MiddleResultStartY)
			{
				m_isMiddleResultActive = false;
			}
		}
	}
}

void GameUIObjects::Init()
{
	m_pinTex = std::make_shared<KdTexture>();
	m_pinTex->Load("Asset/Textures/UI/SceneUI/PinIcon.png");

	m_middleResultWindowTexK = std::make_shared<KdTexture>();
	m_middleResultWindowTexK->Load("Asset/Textures/UI/SceneUI/Game/MiddleResultUI_K.png");
}
