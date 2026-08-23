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
	std::string m_starTexts[StageManagerConsts::StarCountMax];		//星条件テキスト
};

//リザルト
struct GameResult
{
	float m_stageTimer;	//残り時間
	bool m_isCleared;	//クリアフラグ
	int m_fallenPinCnt; //倒したピン数
	int m_totalPinCnt; //全体のピン数
};

class StageManager
{
public:

	//初期化
	void Init();

	//地形生成
	void BuildStage(StageBuildMode mode = StageBuildMode::Full);

	//リセット
	void ResetStage();

	// ゲームクリア時に呼び出して結果を保存
	void SetGameResult(GameResult result)
	{
		m_lastGameResult = result;
		m_lastStarCount = CalculateCurrentStageStarCount(m_lastGameResult.m_fallenPinCnt);
	}

	// ステージデータの保存・読み込み
	bool SaveStage(const std::string& filePath);
	bool LoadStage(int stageNo);
	bool LoadStage(const std::string& filePath);

	// ゲッター(操作用)
	std::string& GetTerrainPath() { return m_terrainPath; }
	std::vector<StageObjectData>& GetStageObjects() { return m_stageObjects; }

	//ゲッター（カメラ用）
	std::weak_ptr<KdGameObject>& GetTerrain() { return m_wpTerrain; }

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
	const StageInfo* GetStageInfo() const;

	// 指定したステージのピン撃破数から獲得星数（1〜3）を計算して返す
	int CalculateStarCount(int stageNo, int pinFallen) const;
	// 現在の選択ステージに対する星数計算（引数を減らしたい場合）
	int CalculateCurrentStageStarCount(int pinFallen) const;

	// リザルト画面で取得用
	GameResult GetLastGameResult() const { return m_lastGameResult; }
	int GetLastGameStarCount()const { return m_lastStarCount; }

	//ステージ数ゲッター（仮置き）
	int GetMaxStageNo()const { return static_cast<int>(m_stageTable.size()); }
	int GetMinStageNo()const { return 1; }

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

	// UTF-8 の std::string を Shift-JIS (ANSI) の std::string に変換する関数
	std::string Utf8ToMultiByte(const std::string& utf8Str);

	//オブジェクトのリスト管理
	std::weak_ptr<KdGameObject> m_wpSkySphere;
	std::weak_ptr<KdGameObject> m_wpTerrain;
	std::vector<std::weak_ptr<KdGameObject>> m_wpStageObject;

	//地形データ管理
	std::string m_terrainPath; //地形データのパス
	std::string m_skySpherePath; //天球のパス

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
	GameResult m_lastGameResult = {};
	int   m_lastStarCount = 0;

public:

	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

};

#define STAGEMGR StageManager::Instance()