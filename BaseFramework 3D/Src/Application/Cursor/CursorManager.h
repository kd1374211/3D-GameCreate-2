#pragma once

class CursorManager
{
public:

	// マウスの座標更新
	void Update();

	// 位置の取得
	POINT GetFixedCursorPos()const { return m_cursorPos; }
	Math::Vector2 GetFixedCursorPosVec2()const { return Math::Vector2(m_cursorPos.x, m_cursorPos.y); }

	// 位置の設定
	void SetCursorPosToCenter();
	void SetCursorToTargetPos(POINT pos);

private:

	// クラス内Const
	struct CursorManagerConsts
	{
		static constexpr POINT CenterCursorPos = POINT(640.0f, 360.0f);
	};

	// 初期化s
	CursorManager() {}
	~CursorManager() {}

	// 位置の修正
	POINT FixCursorPos(POINT cursorPos);
	
	// マウス座標
	POINT m_cursorPos = {};

public:

	static CursorManager& Instance()
	{
		static CursorManager instance;
		return instance;
	}

};

#define CURSOR CursorManager::Instance()