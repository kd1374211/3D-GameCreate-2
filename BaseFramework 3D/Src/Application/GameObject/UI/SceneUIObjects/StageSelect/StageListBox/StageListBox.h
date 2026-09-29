#pragma once

class StageListBox :public KdGameObject
{
public:

	StageListBox() { Init(); }
	~StageListBox()override {}

	void Update()override;
	void DrawSprite()override;

	// 描画位置設定
	void SetPos(Math::Vector2 pos) { m_drawPos = pos; }

	// このフレームでカーソルがあるかの確認
	bool GetIsCursor()const { return m_isCursorThisFrame; }

	// 番号確認
	int GetStageNo()const { return m_stageNo; }

	// ステージ情報設定
	void SetStageListName(std::string name) { m_stageName = name; }
	void LoadStageThumbPath(std::string path);
	void SetStageNo(int no) { m_stageNo = no; }

private:

	// このクラスの定数
	struct StageListBoxConsts
	{
		// サムネイル画像サイズ
		static constexpr Math::Vector2 ListBoxTexSize = Math::Vector2(200.0f, 200.0f);
		static constexpr Math::Vector2 ThumbTexSize = Math::Vector2(190.0f, 190.0f);

		// ステージ名表記用のボックスの描画情報
		static constexpr Math::Vector2 StageNamePosOfs = Math::Vector2(0, -80.0f);
		static constexpr Math::Vector2 StageNameBoxSize = Math::Vector2(100.0f, 20.0f);
		static constexpr Math::Color StageNameBoxColor = Math::Color(0.0f, 0.0f, 0.0f, 0.8f);

		// 判定サイズ
		static constexpr Math::Vector2 BoxHitSizeHalf = Math::Vector2(100.0f, 100.0f);

		// 拡大倍率
		static constexpr float OnCursorSizeMultiMin = 1.0f;
		static constexpr float OnCursorSizeMultiMax = 1.05f;

		// 拡大・縮小速度
		static constexpr float SizeChangeSpeed = 0.5f;
	};

	void Init()override;

	//ステージリストフレーム
	std::shared_ptr<KdTexture> m_stageListFrameTex = nullptr;

	// ステージサムネイル画像
	std::shared_ptr<KdTexture> m_stageThumbTex = nullptr;

	// このフレームでカーソルがあるか
	bool m_isCursorThisFrame = false;

	// 拡大倍率
	float m_sizeMulti = StageListBoxConsts::OnCursorSizeMultiMin;

	// 描画位置
	Math::Vector2 m_drawPos = Math::Vector2::Zero;

	// ステージ名
	std::string m_stageName = "";

	// ステージ番号
	int m_stageNo = 0;
};