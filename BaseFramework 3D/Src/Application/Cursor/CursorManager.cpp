#include "CursorManager.h"
#include "../main.h"
#include "../GameObject/UI/MouseCursor/MouseCursor.h"

void CursorManager::Init()
{
	// 画像ロード
	LoadCursorTextures();

	// カーソル召喚
	m_spCursor = std::make_shared<MouseCursor>();
}

void CursorManager::Update()
{
	// カーソル位置取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// 修正して格納
	m_cursorPos = FixCursorPos(cursorPos);

	// DEBUG
	KdDebugGUI::Instance().AddLog("CursorPos : %.2f,%.2f\n", (float)m_cursorPos.x, (float)m_cursorPos.y);
}

void CursorManager::DrawCursor()
{
	// 描画フラグオンなら描画
	if (m_isShowCursor)
	{
		m_spCursor->DrawCursor(m_cursorTex[(size_t)m_currentTex], GetFixedCursorPosVec2());
	}
}

void CursorManager::SetCursorToTargetPos(POINT pos)
{
	POINT _returnPos = pos;
	ClientToScreen(Application::Instance().GetWindowHandle(), &_returnPos);
	SetCursorPos(_returnPos.x, _returnPos.y);
	m_cursorPos = FixCursorPos(pos);
}

void CursorManager::SetCursorPosToCenter()
{
	SetCursorToTargetPos(CursorManagerConsts::CenterCursorPos);
}

void CursorManager::LoadCursorTextures()
{
	for (size_t i = 0; i < (size_t)CursorTexType::Number; i++)
	{
		// 生成
		m_cursorTex[i] = std::make_shared<KdTexture>();
		
		// 事前に定めたマクロでロード
		m_cursorTex[i]->Load(TEXPATH);
	}
}

POINT CursorManager::FixCursorPos(POINT cursorPos)
{
	// クライアント座標に変換
	ScreenToClient(Application::Instance().GetWindowHandle(), &cursorPos);

	// 中心に指定
	cursorPos.x -= 640.0f;
	cursorPos.y -= 360.0f;

	// Y反転
	cursorPos.y *= -1.0f;

	return cursorPos;
}
