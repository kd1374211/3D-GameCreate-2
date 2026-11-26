#pragma once

struct DecorationPathData
{
	std::string m_name = "";
	std::string m_islandType = "";
	std::string m_path = "";
};

struct IslandPathData
{
	std::string m_name = "";
	std::string m_path = "";
};

class StageEditor
{
public:

	StageEditor() { Init(); }
	~StageEditor() {}

	void UpdateStageEditor(int& currentStageNo, int& currentLaneNo, std::shared_ptr<PointTargetCamera> camera);

private:

	void Init();

	// エディター内各処理																																					// 1.   ステージ変更・セーブロード等
	void UpdateFrameSelection(int& currentStageNo, int& currentLaneNo, StageOverallData& stageData, std::shared_ptr<PointTargetCamera> camera);								// 1-1. 編集ステージ切り替え
	void UpdateSaveAndLoad(int currentStageNo, int currentLaneNo, StageOverallData& stageData);																				// 1-2. セーブロード・コピー等
	void UpdateStageEdit(int currentStageNo, int currentLaneNo, StageOverallData& stageData);																				// 2.   ステージ内の編集
	void UpdateSkyAssetSettings(std::string& skyAssetPathStr, bool& isError, StageOverallData& stageData);																	// 2-1. 天球アセット
	void UpdateIslandAndDecorations(StageOverallData& stageData, int currentLaneNumber);																					// 2-2. 島アセットとデコレーション関連
	void UpdateFrameObjectsSetting(int currentLaneNo, std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageOverallData& stageData);			// 3.   フレーム内オブジェクト編集
	bool UpdateLaneAssetPath(std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageLaneData& frameData);										// 3-1. レーンのアセット
	bool UpdateIslandDecorationsVisibility(int currentLaneNo, StageOverallData& stageData);																					// 3-2. 島デコレーションの表示切替
	bool UpdateGimmickSetting(StageLaneData& frameData);																													// 3-3. ギミック編集
	bool UpdatePinsSetting(StageLaneData& frameData);																														// 3-4. ピン配置
	bool UpdatePlayerSetting(StageLaneData& frameData);																														// 3-5. プレイヤー配置
																																											// 4.   オブジェクトのカーソル移動や選択
	void MoveSelectedObjectToCursor(StageOverallData& stageData, int currentLaneNo)const;																					// 4-1. 選択中のオブジェクトをカーソル位置に移動
	void SetDebugOutlineToSelected(const StageOverallData& stageData, int currentLaneNo)const;																				// 4-2. デバッグアウトラインを選択中のオブジェクトに表示

	// カテゴリ・インデックスの変更
	void ChangeSelectedObject(SelectedTargetCategory category = SelectedTargetCategory::None, int index = -1);
	// カテゴリ・インデックスが合っているかの確認
	bool CheckCategory(SelectedTargetCategory category)const { return m_selectedCategory == category; }
	bool CheckIndex(int index)const { return m_selectedIndex == index; }
	bool CheckCategoryAndIndex(SelectedTargetCategory category, int index)const { return CheckCategory(category) && CheckIndex(index); }

	// デコレーションパスデータ
	std::vector<DecorationPathData> m_decoPathData;
	std::string m_baseDecorationPath = "";
	// 島パスデータ
	std::vector<IslandPathData> m_islandPathData;
	std::string m_baseIslandPath = "";
	// Jsonからロード
	void LoadDecorationPathData();
	void LoadIslandPathData();

	// 現在選択中のカテゴリ・インデックス
	SelectedTargetCategory m_selectedCategory = SelectedTargetCategory::None;
	int m_selectedIndex = -1;

};