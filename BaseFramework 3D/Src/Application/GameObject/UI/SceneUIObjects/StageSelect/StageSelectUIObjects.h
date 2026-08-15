#pragma once

// StageSelect内の仮定義
struct UILayoutConfig
{
	// リスト領域
	static constexpr Math::Vector2 ListStartPos = { 192.0f, 216.0f }; // screenW * 0.15f 等を基準にしたドット値
	static constexpr float         LineSpacing = 40.0f;

	// 詳細ウィンドウ領域
	static constexpr Math::Vector2 DetailWindowPos = { 768.0f, 216.0f };
	static constexpr Math::Vector2 ThumbnailSize = { 320.0f, 180.0f };
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
	
	//メモ
	//ステージ選択をここで
	//ステージ登録とシーン以降はシーン側で

	//仮画像
	std::shared_ptr<KdTexture> m_stageSelectTex = nullptr;

	//現在の選択ステージ番号
	int m_selectStageNo = 0;
	//最大ステージ数
	int m_maxStageNo = 0;
	//最小ステージ数
	int m_minStageNo = 0;
};