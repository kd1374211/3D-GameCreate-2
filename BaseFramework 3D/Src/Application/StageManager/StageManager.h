#pragma once
#include "StageObjectInclude.h"

// 配置オブジェクト情報（ピン・障害物・ギミックなど）
struct StageObjectData
{
	std::string m_type = "NormalPin";                            // "Pin", "Obstacle", "Bumper" など
	Math::Vector3 m_position = { 0.0f, 0.0f, 0.0f };
	Math::Quaternion m_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	Math::Vector3 m_scale = { 1.0f, 1.0f, 1.0f };
};

enum class StageMode
{
	Edit,   // エディットモード（物理停止・編集可能）
	Play    // プレイモード（物理動作・編集不可）
};

struct StageInfo
{
	int m_stageNo = 0;
	float m_2StarTime = 0.0f; // ★2の目標時間
	float m_3StarTime = 0.0f; // ★3の目標時間
};

class StageManager
{
public:

	//初期化
	void Init();

	//地形生成
	void BuildStage();

	//リセット
	void ResetStage();

	// ゲームクリア時に呼び出して結果を保存
	void SetGameResult(float clearTime)
	{
		m_lastClearTime = clearTime;
		m_lastStarCount = CalculateCurrentStageStarCount(clearTime);
	}

	// ステージデータの保存・読み込み
	bool SaveStage(const std::string& filePath);
	bool LoadStage(int stageNo);
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

	//ピン追加時
	void OnPinSpawn()
	{
		m_totalPinCount++;
		m_remainingPinCount++;
	}

	//ピンが倒れる時
	void OnPinFallen()
	{
		m_remainingPinCount--;
	}

	//全部のピンが倒れたかの確認
	bool IsAllPinsFallen() const
	{
		//ピン総数 >= 1かつピン残数 == 0
		return (m_totalPinCount > 0 && m_remainingPinCount == 0);
	}

	//ピン数ゲッター(DEBUG)
	int GetTotalPinCount()const { return m_totalPinCount; }
	int GetRemainingPinCount()const { return m_remainingPinCount; }

	// ステージ番号からマスタ情報を取得（存在しない場合は nullptr）
	const StageInfo* GetStageInfo(int stageNo) const;

	// 指定したステージのクリアタイムから獲得星数（1〜3）を計算して返す
	int CalculateStarCount(int stageNo, float clearTime) const;
	// 現在の選択ステージに対する星数計算（引数を減らしたい場合）
	int CalculateCurrentStageStarCount(float clearTime) const;

	// リザルト画面で取得用
	float GetLastClearTime() const { return m_lastClearTime; }
	int   GetLastStarCount() const { return m_lastStarCount; }

	//ステージ数ゲッター（仮置き）
	int GetMaxStageNo()const { return static_cast<int>(m_stageTable.size()); }

private:

	//いつもの
	StageManager() {}
	~StageManager() { Release(); }

	//マスターデータ読み込み
	bool LoadStageMasterData();

	void Release();

	//ピン数リセット
	void ResetPinCount()
	{
		m_totalPinCount = 0;
		m_remainingPinCount = 0;
	}

	//ステージパス生成
	std::string GetStagePath(int stageNo)
	{
		char path[64];

		snprintf(path, sizeof(path), "Asset/Data/StageData/Stage%02d.json", stageNo);
		
		return std::string(path);
	}

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

	//ピン数（クリア目標）
	int m_totalPinCount = 0;
	int m_remainingPinCount = 0;

	// stageNo をキーにしたマスタデータ保持用マップ
	std::unordered_map<int, StageInfo> m_stageTable;

	// リザルト用データ
	float m_lastClearTime = 0.0f;
	int   m_lastStarCount = 0;

public:

	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

};

#define STAGEMGR StageManager::Instance()