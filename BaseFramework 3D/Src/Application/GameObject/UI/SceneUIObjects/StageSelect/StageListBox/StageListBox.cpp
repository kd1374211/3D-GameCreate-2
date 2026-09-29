#include "StageListBox.h"
#include "../../../../../Const/WindowConsts.h"
#include "../../../../../main.h"

void StageListBox::Update()
{
	// dt取得
	float dt = Application::Instance().GetDeltaTime();

	// フラグリセット
	m_isCursorThisFrame = false;

	// カーソル位置を確認
	POINT cursorPos;
	// 現在のマウス位置を取得
	GetCursorPos(&cursorPos);
	Math::Vector2 fixedPos = GetFixedCursorPos(cursorPos);

	// D
	KdDebugGUI::Instance().AddLog("Cursor : %.2f,%.2f\n", fixedPos.x, fixedPos.y);

	// マウス位置が自分の判定内にあるならフラグをtrueに
	if (fabs(m_drawPos.x - fixedPos.x) <= (StageListBoxConsts::BoxHitSizeHalf.x * m_sizeMulti) &&
		fabs(m_drawPos.y - fixedPos.y) <= (StageListBoxConsts::BoxHitSizeHalf.y * m_sizeMulti))
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
	// 先にサムネイル貼り
	Math::Vector2 drawSize;
	if (m_stageThumbTex)
	{
		drawSize = StageListBoxConsts::ThumbTexSize * m_sizeMulti;
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageThumbTex, m_drawPos.x, m_drawPos.y, drawSize.x, drawSize.y);
	}

	// 上からフレームを重ねる
	drawSize = StageListBoxConsts::ListBoxTexSize * m_sizeMulti;
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageListFrameTex, m_drawPos.x, m_drawPos.y, drawSize.x, drawSize.y);
}

void StageListBox::LoadStageThumbPath(std::string path)
{
	// サムネイルロード
	m_stageThumbTex->Load(path);
}

void StageListBox::Init()
{
	// 画像ロード
	m_stageListFrameTex = std::make_shared<KdTexture>();
	m_stageListFrameTex->Load("Asset/Textures/UI/SceneUI/StageSelect/StageFrame.png");

	// サムネイルはパスがまだないので準備のみ
	m_stageThumbTex = std::make_shared<KdTexture>();
}
