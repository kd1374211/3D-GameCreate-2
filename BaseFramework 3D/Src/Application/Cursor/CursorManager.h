#pragma once

enum class CursorTexType
{
	Normal,
	Number
};

class MouseCursor;

class CursorManager
{
public:

	// 初期化
	void Init();

	// マウスの座標更新
	void Update();
	void DrawCursor();

	// 位置の取得
	POINT GetFixedCursorPos()const { return m_cursorPos; }
	Math::Vector2 GetFixedCursorPosVec2()const { return Math::Vector2(m_cursorPos.x, m_cursorPos.y); }

	// 位置の設定
	void SetCursorPosToCenter();
	void SetCursorToTargetPos(POINT pos);

	// 表示の設定
	void SetIsShowCursor(bool flg) { m_isShowCursor = flg; }
	bool GetIsShowCursor()const { return m_isShowCursor; }

	// カーソル画像の変更
	void ChangeCursorTex(CursorTexType type) { m_currentTex = type; }

private:

	// クラス内Const
	struct CursorManagerConsts
	{
		// カーソル中央合わせ用
		static constexpr POINT CenterCursorPos = POINT(640.0f, 360.0f);
	};

	// 画像パス
	#define TEXPATH "Asset/Textures/Cursor/Cursor" + std::to_string(i + 1) + ".png"
	

	// 初期化s
	CursorManager() {}
	~CursorManager() {}

	// 画像生成
	void LoadCursorTextures();

	// 位置の修正
	POINT FixCursorPos(POINT cursorPos);
	
	// マウス座標
	POINT m_cursorPos = {};

	// 表示用カーソルオブジェクト
	std::shared_ptr<MouseCursor> m_spCursor = nullptr;

	// 現在選択中の画像
	CursorTexType m_currentTex = CursorTexType::Normal;

	// 各カーソル画像
	std::shared_ptr<KdTexture> m_cursorTex[(size_t)CursorTexType::Number] = {};

	// カーソル表示フラグ
	bool m_isShowCursor = false;

public:

	static CursorManager& Instance()
	{
		static CursorManager instance;
		return instance;
	}

};

#define CURSOR CursorManager::Instance()