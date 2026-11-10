#pragma once
#include "VariantParamData.h"
#include "PinTypes.h"
#include "BowlingSystemConst.h"

// 配置オブジェクト情報（座標・回転・スケール）
struct LaneObjectData
{
	Math::Vector3 m_position = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 m_rotation = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 m_scale = { 1.0f, 1.0f, 1.0f };
};

// ピン以外のステージオブジェクト
struct LaneGimmickData
{
	std::string m_type;
	GimmickParam m_param;				// 固有パラメータ兼タイプ保持
	LaneObjectData m_data;				// 配置情報
};

// ピンは↑と追加でIndexも持つ
struct LanePinData
{
	int m_index = -1;						// ピンのインデックス
	PinType m_type = PinType::Error;		// ピンの種類
	LaneObjectData m_data;					// ピンの配置情報
};

// プレイヤーの配置個所と向き
struct PlayerPlacementData
{
	Math::Vector3 m_position = {};
	Math::Vector3 m_rotation = {};
};

// ステージのレーンごとの情報
struct StageLaneData
{
	int m_frameNumber;		// レーン番号
	std::string m_terrainPath;		// 地形パス
	std::vector<LaneGimmickData> m_laneGimmickData;	// ピン以外のオブジェクト
	std::vector<LanePinData> m_lanePinData;		// ピン一覧
	PlayerPlacementData m_playerData;			// プレイヤーの位置
};

// 島に配置するオブジェクトのデータ
struct IslandDecorationData
{
	std::string m_modelPath;
	LaneObjectData m_srtData;
	bool m_isActive[BowlingSystemConsts::FrameCount] = { true };	// 各フレームに表示するかどうかのフラグ
};

// ステージ全体の情報
struct StageOverallData
{
	std::string m_skyPath;			// 天球パス
	std::string m_islandPath;		// 島のパス
	Math::Vector3 m_islandPos;		// 島の座標
	std::vector<StageLaneData> m_stageLaneData;	// 各レーンの情報
	std::vector<IslandDecorationData> m_decorationData;	// デコレーション
};

// ギミック識別enum
enum class GimmickType
{
	FinishArea,
	SpeedUpArea,
	RotatingTerrain,
	MovingTerrain,
	Error = 999
};