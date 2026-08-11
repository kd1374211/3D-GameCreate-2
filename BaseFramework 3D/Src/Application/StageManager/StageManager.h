#pragma once
#include "StageObjectInclude.h"

// 配置オブジェクト情報（ピン・障害物・ギミックなど）
struct StageObjectData
{
	std::string type = "NormalPin";                            // "Pin", "Obstacle", "Bumper" など
	Math::Vector3 position = { 0.0f, 0.0f, 0.0f };
	Math::Quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	Math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
};

enum class StageMode
{
	Edit,   // エディットモード（物理停止・編集可能）
	Play    // プレイモード（物理動作・編集不可）
};

class StageManager
{
public:

	//地形生成
	void BuildStage();

	//リセット
	void ResetStage();

	// ステージデータの保存・読み込み
	bool SaveStage(const std::string& filePath);
	bool LoadStage(const std::string& filePath);

	// ゲッター(操作用)
	std::string& GetTerrainPath() { return m_terrainPath; }
	std::vector<StageObjectData>& GetStageObjects() { return m_stageObjects; }

	void AddStageObject(const StageObjectData& objectData) { m_stageObjects.push_back(objectData); }
	void RemoveStageObject(size_t index)
	{
		if (index < m_stageObjects.size())
		{
			m_stageObjects.erase(m_stageObjects.begin() + index);
		}
	}

	// モード切り替え関連
	void SetMode(StageMode mode);
	StageMode GetMode() const { return m_mode; }
	bool IsEditMode() const { return m_mode == StageMode::Edit; }

	// 選択インデックスの管理
	void SetSelectedIndex(int index) { m_selectedIndex = index; }
	int GetSelectedIndex() const { return m_selectedIndex; }

	//選択時アウトライン追加
	void DrawSelectedObjectOutline();

	// ImGuiやBuildStageから呼ばれるターゲット設定関数
	void ApplyCameraTarget();

private:

	//いつもの
	StageManager() { Init(); }
	~StageManager() { Release(); }

	void Init();
	void Release();

	//オブジェクトのリスト管理
	std::weak_ptr<KdGameObject> m_wpTerrain;
	std::vector<std::weak_ptr<KdGameObject>> m_wpStageObject;

	//地形データ管理
	std::string m_terrainPath; //地形データのパス

	//オブジェクトデータ管理
	std::vector<StageObjectData> m_stageObjects;

	StageMode m_mode = StageMode::Play; // 初期状態はエディットモード
	int m_selectedIndex = -1; // 選択中のオブジェクトインデックス（-1は未選択）

	//デバッグ用
	std::unique_ptr<KdDebugWireFrame> m_debugWireFrame;

public:

	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

};

#define STAGEMGR StageManager::Instance()