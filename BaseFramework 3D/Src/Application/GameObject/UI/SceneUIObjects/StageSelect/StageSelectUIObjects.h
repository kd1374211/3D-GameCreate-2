#pragma once

// StageSelect内の仮定義
struct StageSelectUIConsts
{
	// リスト配置
	static constexpr int ListIndexX = 2;
	static constexpr Math::Vector2 ListPosBase = { -530.0f,210.0f };
	static constexpr float ListPosDiff = 220.0f;

	// 詳細ウィンドウ領域
	static constexpr Math::Vector2 DetailWindowPos = { 310.0f, 50.0f };
	static constexpr Math::Vector2 DetailWindowSize = { 620.0f, 540.0f };
	static constexpr Math::Vector2 ThumbnailPos = { 320.0f, 165.0f };
	static constexpr Math::Vector2 ThumbnailSize = { 480.0f, 270.0f };

	// 星
	static constexpr float StarListPosY = -120.0f;
	static constexpr float StarListBasePosX = 270.0f;
	static constexpr float StarPosDiffX = 40.0f;

	// ピン数
	static constexpr Math::Vector2 PinIconPos = { 240.0f, -170.0f };
	static constexpr Math::Vector2 PinCountTextPos = { 390.0f, -170.0f };

	// テキスト
	static constexpr Math::Vector2 StageNamePos = { 310.0f, -30.0f };
	static constexpr Math::Vector2 ClearedTextPos = { 310.0f,-90.0f };

	// ── 操作ガイド ──
	static constexpr Math::Vector2 KeyHelpPos = { 0.0f, -320.0f }; // 画面最下部・中央
};

class StageListBox;

class StageSelectUIObject :public KdGameObject
{
public:

	StageSelectUIObject() { Init(); }
	~StageSelectUIObject()override {}

	void Update()override;
	void DrawSprite()override;

	//選択チェック
	int GetSelectedStageNo()const { return m_selectStageNo; }
	// カーソルがあるか
	bool GetIsSelect()const { return m_isCursorOnAnyStage; }

private:

	void Init()override;
	
	//サムネイル画像変更
	void ChangeThumbTex();

	//ステージウィンドウフレーム
	std::shared_ptr<KdTexture> m_stageInfoFrameTex = nullptr;

	//ステージサムネイル
	std::shared_ptr<KdTexture> m_stageThumbTex = nullptr;

	// ピンアイコン
	std::shared_ptr<KdTexture> m_pinTex = nullptr;

	//現在の選択ステージ番号
	int m_selectStageNo = 0;

	// ステージリスト分のフレーム用意
	std::vector<std::shared_ptr<StageListBox>> m_spList;

	// このフレームでいずれかのステージにカーソルがあるか
	bool m_isCursorOnAnyStage = false;
};