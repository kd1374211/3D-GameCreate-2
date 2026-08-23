#pragma once

// StageSelect内の仮定義
struct UILayoutConfig
{
	// リスト領域
	static constexpr Math::Vector2 ListStartPos = { -500.0f, 270.0f }; // screenW * 0.15f 等を基準にしたドット値
	static constexpr Math::Vector2 FrameSize = { 200.0f,60.0f };
	static constexpr float         LineSpacing = 80.0f;

	// 詳細ウィンドウ領域
	static constexpr Math::Vector2 DetailWindowPos = { 310.0f, 50.0f };
	static constexpr Math::Vector2 DetailWindowSize = { 620.0f, 540.0f };
	static constexpr Math::Vector2 ThumbnailPos = { 320.0f, 165.0f };
	static constexpr Math::Vector2 ThumbnailSize = { 480.0f, 270.0f };

	// テキスト
	static constexpr Math::Vector2 StageNamePos = { 310.0f, -40.0f };
	
	// ── 操作ガイド ──
	static constexpr Math::Vector2 KeyHelpPos = { 0.0f, -320.0f }; // 画面最下部・中央
};

class StageSelectUIObject :public KdGameObject
{
public:

	StageSelectUIObject() { Init(); }
	~StageSelectUIObject()override {}

	void Update()override;
	void DrawSprite()override;

	//選択チェック
	int GetSelectedStageNo()const { return m_selectStageNo; }

private:

	void Init()override;
	
	//サムネイル画像変更
	void ChangeThumbTex();

	//ステージリストフレーム
	std::shared_ptr<KdTexture> m_stageListFrameTex = nullptr;

	//ステージウィンドウフレーム
	std::shared_ptr<KdTexture> m_stageInfoFrameTex = nullptr;

	//ステージサムネイル
	std::shared_ptr<KdTexture> m_stageThumbTex = nullptr;

	//現在の選択ステージ番号
	int m_selectStageNo = 0;
	//最大ステージ数
	int m_maxStageNo = 0;
	//最小ステージ数
	int m_minStageNo = 0;
};