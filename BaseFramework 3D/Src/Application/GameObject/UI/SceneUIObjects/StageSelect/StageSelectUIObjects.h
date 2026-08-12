#pragma once

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
	//最小ステージ数（定数）
	static const int MINSTAGENO = 1;
};