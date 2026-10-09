#include "StageListBox.h"
#include "../../../../../Cursor/CursorManager.h"
#include "../../../../../Const/WindowConsts.h"
#include "../../../../../main.h"

void StageListBox::Update()
{
	// dt取得
	float dt = Application::Instance().GetDeltaTime();

	// フラグリセット
	m_isCursorThisFrame = false;

	// カーソル位置を確認
	Math::Vector2 cursorPos = CURSOR.GetFixedCursorPosVec2();

	// マウス位置が自分の判定内にあるならフラグをtrueに
	if (fabs(m_drawPos.x - cursorPos.x) <= (StageListBoxConsts::BoxHitSizeHalf.x * m_sizeMulti) &&
		fabs(m_drawPos.y - cursorPos.y) <= (StageListBoxConsts::BoxHitSizeHalf.y * m_sizeMulti))
	{
		m_isCursorThisFrame = true;
	}

	// カーソルが乗っているなら拡大
	if (m_isCursorThisFrame)
	{
		m_sizeMulti = std::min(m_sizeMulti + StageListBoxConsts::SizeChangeSpeed * dt, StageListBoxConsts::OnCursorSizeMultiMax);
	}
	// 載っていないなら縮小
	else
	{
		m_sizeMulti = std::max(m_sizeMulti - StageListBoxConsts::SizeChangeSpeed * dt, StageListBoxConsts::OnCursorSizeMultiMin);
	}
}

void StageListBox::DrawSprite()
{
	//ウィンドウ作る
	std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();

	//レンダー作成
	Math::Vector2 renderBase = WindowSizeConsts::WindowSize;
	tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

	//透明塗り
	KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

	//ターゲット設定
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

	// 先にサムネイル貼り
	Math::Vector2 drawSize;
	if (m_stageThumbTex)
	{
		drawSize = StageListBoxConsts::ThumbTexSize;
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageThumbTex, 0, 0, drawSize.x, drawSize.y);
	}

	// 上からフレームを重ねる
	drawSize = StageListBoxConsts::ListBoxTexSize;
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageListFrameTex, 0, 0, drawSize.x, drawSize.y);

	// さらに上からボックスを召喚
	Math::Vector2 drawPos = StageListBoxConsts::StageNamePosOfs;
	drawSize = StageListBoxConsts::StageNameBoxSize;
	KdShaderManager::Instance().m_spriteShader.DrawBox(drawPos.x, drawPos.y, drawSize.x, drawSize.y, &StageListBoxConsts::StageNameBoxColor, true);

	// ステージ名表示
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::StageSelect_StageListName, drawPos, &kWhiteColor, m_stageName.c_str(), TextAlign::Center);

	//ターゲット戻す
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, KdDirect3D::Instance().WorkBackBuffer()->WorkRTViewAddress(), KdDirect3D::Instance().WorkZBuffer()->WorkDSView());

	//tmpTexをサイズ変えて描画
	drawSize = renderBase * m_sizeMulti;
	KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, m_drawPos.x, m_drawPos.y, drawSize.x, drawSize.y);
}

void StageListBox::LoadStageThumbPath(std::string path)
{
	// サムネイルロード
	m_stageThumbTex = KdAssets::Instance().m_textures.GetData(path);
}

void StageListBox::Init()
{
	// 画像ロード
	m_stageListFrameTex = std::make_shared<KdTexture>();
	m_stageListFrameTex->Load("Asset/Textures/UI/SceneUI/StageSelect/StageFrame.png");

	// サムネイルはパスがまだないので準備のみ
	m_stageThumbTex = std::make_shared<KdTexture>();
}
