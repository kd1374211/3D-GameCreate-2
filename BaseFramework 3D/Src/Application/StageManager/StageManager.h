#pragma once
#include "StageObjectInclude.h"
#include "../Const/BowlingSystemConst.h"
#include "../Const/PinTypes.h"

// 配置オブジェクト情報（座標・回転・スケール）
struct LaneObjectData
{
	Math::Vector3 m_position = { 0.0f, 0.0f, 0.0f };
	Math::Quaternion m_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	Math::Vector3 m_scale = { 1.0f, 1.0f, 1.0f };
};

// ピン以外のステージオブジェクト
struct LaneGimmickData
{
	std::string m_type = "Goal";		// "Goal" など
	LaneObjectData m_data;				// 配置情報
};

// ピンは↑と追加でIndexも持つ
struct LanePinData
{
	int m_index = -1;						// ピンのインデックス
	PinType m_type = PinType::Error;		// ピンの種類
	LaneObjectData m_data;					// ピンの配置情報
};

// ステージのレーンごとの情報
struct StageLaneData
{
	int m_frameNumber;		// レーン番号
	std::string m_terrainPath;		// 地形パス
	std::vector<LaneGimmickData> m_laneGimmickData;	// ピン以外のオブジェクト
	std::vector<LanePinData> m_lanePinData;		// ピン一覧
};

// ステージ全体の情報
struct StageOverallData
{
	std::string m_skyPath;			// 天球パス
	std::vector<StageLaneData> m_stageLaneData;	// 各レーンの情報
};

enum class StageMode
{
	Edit,   // エディットモード（物理停止・編集可能）
	Play    // プレイモード（物理動作・編集不可）
};

// ステージ生成をどこまでするか
enum class StageBuildMode
{
	Full,       // 本編用（地形＋天球＋プレイヤー＋ピン＋ギミック等）
	Background  // 背景用（地形＋天球のみ）
};

struct StageManagerConsts
{
	static constexpr int StarCountMax = 3;		//最大星数
};

struct StageInfo
{
	int m_stageNo = 0;				//ステージ番号
	std::string m_stageListName;	//リスト上でのステージ名
	std::string m_stageName;		//詳細上でのステージ名（本当の名前）
	std::string m_stageThumbPath;	//サムネイル画像パス
	float m_timeLimit;				//制限時間
	float m_fallOutLine;			//落下死境界
	int m_totalPinCount;			//ピン数
	std::string m_starTexts[StageManagerConsts::StarCountMax];		//星条件テキスト
	int m_starPinNeed[StageManagerConsts::StarCountMax];			//星必要ピン数
};

class PinHandler;
enum class PinType;

class StageManager
{
public:

	//初期化
	void Init();

	//地形生成
	//void BuildStage(StageBuildMode mode = StageBuildMode::Full);
	void BuildStage(int laneNumber = BowlingSystemConsts::StartFrame, StageBuildMode mode = StageBuildMode::Full);

	//リセット
	void ResetStage();

	// ステージデータの保存・読み込み
	bool SaveStage(const std::string& filePath);
	bool LoadStage(int stageNo);
	bool LoadStage(const std::string& filePath);

	// ゲッター(操作用)
	//std::string& GetTerrainPath() { return ; }
	//std::vector<LaneObjectData>& GetStageObjects() { return m_stageGimmicks; }

	//ゲッター（カメラ用）
	//std::weak_ptr<KdGameObject>& GetTerrain() { return m_wpTerrain; }

	//void AddStageObject(const LaneObjectData& objectData) { m_stageGimmicks.push_back(objectData); }
	//void RemoveStageObject(size_t index)
	//{
	//	if (index < m_stageGimmicks.size())
	//	{
	//		m_stageGimmicks.erase(m_stageGimmicks.begin() + index);
	//	}
	//}

	// モード切り替え関連
	void SetMode(StageMode mode);
	StageMode GetMode() const { return m_mode; }
	bool IsEditMode() const { return m_mode == StageMode::Edit; }

	// 選択インデックスの管理
	void SetSelectedIndex(int index) { m_selectedIndex = index; }
	int GetSelectedIndex() const { return m_selectedIndex; }

	//選択時アウトライン追加
	void DrawSelectedObjectOutline();

	// ステージ番号からマスタ情報を取得（存在しない場合は nullptr）
	const StageInfo* GetStageInfo(int stageNo) const;
	const StageInfo* GetStageInfo() const;

	// 指定したステージのピン撃破数から獲得星数（1〜3）を計算して返す
	int CalculateStarCount(int stageNo, int pinFallen, bool isClear) const;
	// 現在の選択ステージに対する星数計算（引数を減らしたい場合）
	int CalculateCurrentStageStarCount(int pinFallen, bool isClear) const;

	// リザルト画面で取得用
	//GameResult GetLastGameResult() const { return m_lastGameResult; }
	//int GetLastGameStarCount()const { return m_lastStarCount; }

	//ステージ数ゲッター（仮置き）
	int GetMaxStageNo()const { return static_cast<int>(m_stageTable.size()); }
	int GetMinStageNo()const { return 1; }

	// ピンハンドラー登録
	void RegistPinHandler(std::shared_ptr<PinHandler> handler) { m_wpPinHandler = handler; }

	// ハンドラーにプールを作成
	void CreatePinPool();

private:

	// このクラス内の定数
	struct StageManagerConsts
	{
		static constexpr int LaneIndexOffset = -1;		// 引数のレーン番号とvectorの位置差
	};

	//いつもの
	StageManager() {}
	~StageManager() { Release(); }

	//マスターデータ読み込み
	bool LoadStageMasterData();

	void Release();

	//ステージパス生成
	std::string GetStagePath(int stageNo)
	{
		char path[64];

		snprintf(path, sizeof(path), "Asset/Data/StageData/Stage%02d.json", stageNo);
		
		return std::string(path);
	}

	// Jsonの要素確認をして変換するヘルパー(Vector3)
	Math::Vector3 ParseVector3(const nlohmann::json& j, const std::string& key, const Math::Vector3& defaultValue)
	{
		if (j.contains(key) && j[key].is_array() && j[key].size() >= 3)
		{
			return { j[key][0].get<float>(), j[key][1].get<float>(), j[key][2].get<float>() };
		}
		return defaultValue;
	}

	// Jsonの要素確認をして変換するヘルパー(Quarternion)
	Math::Quaternion ParseQuaternion(const nlohmann::json& j, const std::string& key, const Math::Quaternion& defaultValue)
	{
		if (j.contains(key) && j[key].is_array() && j[key].size() >= 4)
		{
			return { j[key][0].get<float>(), j[key][1].get<float>(), j[key][2].get<float>(), j[key][3].get<float>() };
		}
		return defaultValue;
	}

	// UTF-8 の std::string を Shift-JIS (ANSI) の std::string に変換する関数
	std::string Utf8ToMultiByte(const std::string& utf8Str);

	//オブジェクトのリスト管理
	std::weak_ptr<KdGameObject> m_wpSkySphere;
	std::weak_ptr<KdGameObject> m_wpTerrain;
	std::vector<std::weak_ptr<KdGameObject>> m_wpStageGimmicks;

	// ↑の置き換え後
	StageOverallData m_stageOverallData;

	StageMode m_mode = StageMode::Play; // 初期状態はエディットモード
	int m_selectedIndex = -1; // 選択中のオブジェクトインデックス（-1は未選択）

	//デバッグ用
	std::unique_ptr<KdDebugWireFrame> m_debugWireFrame;

	// stageNo をキーにしたマスタデータ保持用マップ
	std::unordered_map<int, StageInfo> m_stageTable;

	// リザルト用データ
	//GameResult m_lastGameResult = {};
	//int   m_lastStarCount = 0;

	// ピンハンドラーweak
	std::weak_ptr<PinHandler> m_wpPinHandler;

public:

	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

};

#define STAGEMGR StageManager::Instance()