#include "StageManager.h"
#include "../Scene/SceneManager.h"
#include "../Component/CharaHandler/CharaHandler.h"
#include "../GameObject/Camera/CameraBase.h"
#include "../Component/PinHandler/PinHandler.h"

void StageManager::ResetStage()
{
	//地形削除
	if (!m_wpTerrain.expired())
	{
		m_wpTerrain.lock()->SetExpire();
	}

	//天球削除
	if (!m_wpSkySphere.expired())
	{
		m_wpSkySphere.lock()->SetExpire();
	}

	//オブジェクトリセット
	for (auto& obj : m_wpStageGimmicks)
	{
		if (obj.expired())continue;

		//削除
		obj.lock()->SetExpire();
	}
	m_wpStageGimmicks.clear();
}

bool StageManager::SaveStage(const std::string& filePath)
{
	nlohmann::json rootJson;

	// 1. 天球パス
	rootJson["skyPath"] = m_stageOverallData.m_skyPath;

	// 2. フレーム（レーン）配列
	nlohmann::json framesArray = nlohmann::json::array();

	for (const auto& lane : m_stageOverallData.m_stageLaneData)
	{
		nlohmann::json frameJson;
		frameJson["frameNumber"] = lane.m_frameNumber;
		frameJson["terrainPath"] = lane.m_terrainPath;

		// プレイヤー (player)
		frameJson["player"]["position"] = { lane.m_playerData.m_pos.x, lane.m_playerData.m_pos.y, lane.m_playerData.m_pos.z };
		frameJson["player"]["rotation"] = { lane.m_playerData.m_rot.x, lane.m_playerData.m_rot.y, lane.m_playerData.m_rot.z, lane.m_playerData.m_rot.w };

		// ギミック一覧 (gimmicks)
		nlohmann::json gimmickArray = nlohmann::json::array();
		for (const auto& gimmick : lane.m_laneGimmickData)
		{
			nlohmann::json gJson;
			gJson["type"] = gimmick.m_type;
			gJson["position"] = { gimmick.m_data.m_position.x, gimmick.m_data.m_position.y, gimmick.m_data.m_position.z };
			gJson["rotation"] = { gimmick.m_data.m_rotation.x, gimmick.m_data.m_rotation.y, gimmick.m_data.m_rotation.z, gimmick.m_data.m_rotation.w };
			gJson["scale"] = { gimmick.m_data.m_scale.x, gimmick.m_data.m_scale.y, gimmick.m_data.m_scale.z };
			gimmickArray.push_back(gJson);
		}
		frameJson["gimmicks"] = gimmickArray;

		// ピン一覧 (pins)
		nlohmann::json pinArray = nlohmann::json::array();
		for (const auto& pin : lane.m_lanePinData)
		{
			nlohmann::json pinJson;
			pinJson["index"] = pin.m_index;
			pinJson["type"] = ConvertPinTypeToString(pin.m_type); // 例: "Normal" 等の文字列変換
			pinJson["position"] = { pin.m_data.m_position.x, pin.m_data.m_position.y, pin.m_data.m_position.z };
			pinJson["rotation"] = { pin.m_data.m_rotation.x, pin.m_data.m_rotation.y, pin.m_data.m_rotation.z, pin.m_data.m_rotation.w };
			pinJson["scale"] = { pin.m_data.m_scale.x, pin.m_data.m_scale.y, pin.m_data.m_scale.z };
			pinArray.push_back(pinJson);
		}
		frameJson["pins"] = pinArray;

		framesArray.push_back(frameJson);
	}

	rootJson["frames"] = framesArray;

	// ファイルへ書き出し
	std::ofstream file(filePath);
	if (!file.is_open()) return false;

	file << rootJson.dump(4);
	return true;
}

bool StageManager::LoadStage(int stageNo)
{
	return LoadStage(GetStagePath(stageNo));
}

bool StageManager::LoadStage(const std::string& filePath)
{
	std::ifstream inFile(filePath);
	if (!inFile.is_open()) return false;

	nlohmann::json rootJson;
	inFile >> rootJson;

	// リセット
	m_stageOverallData.m_skyPath = "";
	m_stageOverallData.m_stageLaneData.clear();

	// 1.ステージ全体の天球を読み込む
	if (rootJson.contains("skyPath"))
	{
		m_stageOverallData.m_skyPath = rootJson.value("skyPath", "Asset/Models/Sky/SkySphere/SkySphere.gltf");
	}

	// 2.各フレームのデータを見る
	if (rootJson.contains("frames"))
	{
		StageLaneData laneData;
		LaneGimmickData gimmickData;
		LanePinData pinData;
		PlayerPlacementData playerData;
		for (const auto& frame : rootJson["frames"])
		{
			// 情報クリア
			laneData = {};
			playerData = {};
			
			// 2-1.レーン番号を取得
			laneData.m_frameNumber = frame.value("frameNumber", -1);

			// 2-2.地形パスを取得
			laneData.m_terrainPath = frame.value("terrainPath", "Asset/Models/Terrain/Stage01/Stage01.gltf");

			// 2-3.プレイヤーを取得
			if (frame.contains("player"))
			{
				const auto& player = frame["player"];

				// 各情報を取得
				playerData.m_pos = ParseVector3(player, "position", {0.0f, 0.0f, 0.0f});
				playerData.m_rot = ParseQuaternion(player, "rotation", { 0.0f, 0.0f, 0.0f,1.0f });

				// レーンデータに設定
				laneData.m_playerData = playerData;
			}
				
			// 2-4.ギミックを取得
			if (frame.contains("gimmicks"))
			{
				for (const auto& gimmick : frame["gimmicks"])
				{
					// 情報クリア
					gimmickData = {};

					// 各情報を取得
					gimmickData.m_type = gimmick.value("type", "Goal");
					gimmickData.m_data.m_position = ParseVector3(gimmick, "position", { 0.0f, 0.0f, 0.0f });
					gimmickData.m_data.m_rotation = ParseQuaternion(gimmick, "rotation", { 0.0f, 0.0f, 0.0f, 1.0f });
					gimmickData.m_data.m_scale = ParseVector3(gimmick, "scale", { 1.0f, 1.0f, 1.0f });

					// レーンデータに追加
					laneData.m_laneGimmickData.push_back(gimmickData);
				}
			}

			// 2-5.ピンを取得
			if (frame.contains("pins"))
			{
				for (const auto& pin : frame["pins"])
				{
					// 情報クリア
					pinData = {};

					// 各情報を取得
					pinData.m_index = pin.value("index", -1);
					pinData.m_type = ConvertStringToPinType(pin.value("type", "error"));
					pinData.m_data.m_position = ParseVector3(pin, "position", { 0.0f, 0.0f, 0.0f });
					pinData.m_data.m_rotation = ParseQuaternion(pin, "rotation", { 0.0f, 0.0f, 0.0f, 1.0f });
					pinData.m_data.m_scale = ParseVector3(pin, "scale", { 1.0f, 1.0f, 1.0f });

					// レーンデータに追加
					laneData.m_lanePinData.push_back(pinData);
				}
			}

			// 2-6.全体データに追加
			m_stageOverallData.m_stageLaneData.push_back(laneData);
		}
	}

	return true;
}

void StageManager::SetMode(StageMode mode)
{
	m_mode = mode;

	BuildStage();
}

void StageManager::DrawSelectedObjectOutline()
{
	// エディットモード中かつ有効なインデックスが選択されている場合のみ
	if (!IsEditMode() || m_selectedIndex < 0) return;

	//DEBUG
	KdDebugGUI::Instance().AddLog("obj: %d\n", m_wpStageGimmicks.size());

	size_t targetIdx = static_cast<size_t>(m_selectedIndex);
	if (targetIdx < m_wpStageGimmicks.size() && !m_wpStageGimmicks[targetIdx].expired())
	{
		auto spObj = m_wpStageGimmicks[targetIdx].lock();

		// オブジェクトの現在位置を取得
		Math::Vector3 pos = spObj->GetPos();

		// オブジェクト位置にワイヤーフレームを描画
		m_debugWireFrame->AddDebugSphere(pos, 1.5f, kBlueColor);
		m_debugWireFrame->Draw();
	}
}

void StageManager::Init()
{
	m_debugWireFrame = std::make_unique<KdDebugWireFrame>();

	if (!LoadStageMasterData())
	{
		//ロード失敗時の処理があるなら書く
	}
}

void StageManager::BuildStage(int laneNumber, StageBuildMode mode)
{
	// 一応レーン番号確認
	if (laneNumber > BowlingSystemConsts::FrameCount)return;
	if (laneNumber < BowlingSystemConsts::StartFrame)return;

	// レーンデータ
	const StageLaneData& laneData = m_stageOverallData.m_stageLaneData[laneNumber];

	//リセット
	ResetStage();

	// 天球生成
	std::shared_ptr<SkySphere> sky = std::make_shared<SkySphere>(m_stageOverallData.m_skyPath);
	// ゲームシーンの管理リストに追加
	SCENEMGR.AddObject(sky);
	// 後で消せるようにリスト持ち
	m_wpSkySphere = sky;

	// 地形生成
	std::shared_ptr<Ground> ground = std::make_shared<Ground>(laneData.m_terrainPath, Math::Vector3::Zero, Math::Quaternion::Identity);
	// ゲームシーンの管理リストに追加
	SCENEMGR.AddObject(ground);
	// 後で消せるようにリスト持ち
	m_wpTerrain = ground;

	// ギミック生成
	for (const auto& gimmickData : laneData.m_laneGimmickData)
	{
		// ギミック生成
		std::shared_ptr<KdGameObject> gimmickObj;
		// 有効タイプ確認
		bool isValidType = false;

		// ゴール地点
		if (gimmickData.m_type == "Goal")
		{
			gimmickObj = std::make_shared<FinishArea>(gimmickData.m_data.m_position, gimmickData.m_data.m_rotation, gimmickData.m_data.m_scale);
			isValidType = true;
		}

		// 有効ならゲームシーンに追加
		if (isValidType)
		{
			// ゲームシーンの管理リストに追加
			SCENEMGR.AddObject(gimmickObj);
			// 後で消せるようにリスト持ち
			m_wpStageGimmicks.push_back(gimmickObj);
		}
	}

	// ピン生成（ハンドラー）
	if (!m_wpPinHandler.expired())
	{
		m_wpPinHandler.lock()->SpawnPinsForThisFrame(laneData.m_lanePinData);
	}

	// プレイヤー配置(未実装)
	if (!m_wpCharaHandler.expired())
	{
		m_wpCharaHandler.lock()->StartNextThrow(laneData.m_playerData.m_pos, laneData.m_playerData.m_rot);
	}
}

void StageManager::RespawnStage(int laneNumber)
{
	// 一応レーン番号確認
	if (laneNumber > BowlingSystemConsts::FrameCount)return;
	if (laneNumber < BowlingSystemConsts::StartFrame)return;

	// レーンデータ
	const StageLaneData& laneData = m_stageOverallData.m_stageLaneData[laneNumber];

	// ピンの再配置
	if (auto spPinHandler = m_wpPinHandler.lock())
	{
		spPinHandler->CheckAndResetRemainingPins(laneData.m_lanePinData);
	}

	// プレイヤー再配置(未実装)
	if (!m_wpCharaHandler.expired())
	{
		m_wpCharaHandler.lock()->StartNextThrow(laneData.m_playerData.m_pos, laneData.m_playerData.m_rot);
	}
}

const StageInfo* StageManager::GetStageInfo(int stageNo) const
{
	auto it = m_stageTable.find(stageNo);
	if (it != m_stageTable.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const StageInfo* StageManager::GetStageInfo() const
{
	//現在のステージ番号を見る
	return GetStageInfo(SCENEMGR.GetStageNo());
}

int StageManager::CalculateStarCount(int stageNo, int pinFallen, bool isClear) const
{
	//// マスタデータが存在しない、または無効なステージ番号の場合は最小の★1を返す
	//auto it = m_stageTable.find(stageNo);
	//if (it == m_stageTable.end())
	//{
	//	return 1;
	//}

	//const auto& info = it->second;

	//// ここで評価の計算
	//int starCount = 0;

	//// クリアしていないなら0確定
	//if (isClear)
	//{
	//	for (int i = 0; i < StageManagerConsts::StarCountMax; i++)
	//	{
	//		if (pinFallen >= info.m_starPinNeed[i])
	//		{
	//			starCount++;
	//		}
	//		else break;
	//	}
	//}

	//return starCount; // 計算した星数
	return 1;
}

int StageManager::CalculateCurrentStageStarCount(int pinFallen, bool isClear) const
{
	// 現在選択されているステージ番号（m_currentStageNo）を使って計算
	//return CalculateStarCount(SCENEMGR.GetStageNo(), pinFallen, isClear);
	return 1;
}

void StageManager::CreatePinPool()
{
	// ハンドラーがないならリターン
	if (m_wpPinHandler.expired())return;

	// ピン種ごとの「最大必要数」を記録するマップ
	std::unordered_map<PinType, size_t> maxRequiredCounts;

	// 各レーンを見てピンごとに最大値を計算
	for (const auto& data : m_stageOverallData.m_stageLaneData)
	{
		// このレーン内でのピン種ごとの個数を一時カウント
		std::unordered_map<PinType, size_t> currentLaneCounts;

		// このレーンにあるピンを見てデータを追加
		for (const auto& pins : data.m_lanePinData)
		{
			PinType type = pins.m_type;

			if (type != PinType::Error)
			{
				currentLaneCounts[type]++;
			}
		}

		// これまでの最大数と比較して多ければ更新
		for (const auto& [type, count] : currentLaneCounts)
		{
			if (count > maxRequiredCounts[type])
			{
				maxRequiredCounts[type] = count;
			}
		}
	}

	// 計算した数をベースにハンドラーの作成処理を呼ぶ
	auto pinHandler = m_wpPinHandler.lock();
	for (const auto& [type, count] : maxRequiredCounts)
	{
		pinHandler->CreatePinPool(type, count);
	}
}

bool StageManager::LoadStageMasterData()
{
	// パスはクラス内部に直書きで保持
	const std::string masterJsonPath = "Asset/Data/StageData/StageMasterData.json";

	std::ifstream file(masterJsonPath);
	if (!file.is_open())
	{
		return false; // ファイルが開けない場合
	}

	nlohmann::json rootJson;
	try
	{
		file >> rootJson;
	}
	catch (...)
	{
		file.close();
		return false; // JSONの構文エラー等
	}
	file.close();

	// 既存データをクリア
	m_stageTable.clear();

	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson)
	{
		StageInfo info;

		// .value("キー名", デフォルト値) を使うことで、キーが存在しなくても安全に取得可能
		info.m_stageNo = item.value("stageNo", 0);
		info.m_stageListName = Utf8ToMultiByte(item.value("stageListName", "エラー"));
		info.m_stageName = Utf8ToMultiByte(item.value("stageName", "エラー"));
		info.m_stageThumbPath = item.value("thumbnail", "Asset/Textures/System/WhiteNoise.png");
		info.m_timeLimit = item.value("timeLimit", 10.0f);
		info.m_fallOutLine = item.value("fallOutLine", -10.0f);
		info.m_totalPinCount = item.value("totalPinCount", 1);
		info.m_starTexts[0] = Utf8ToMultiByte(item.value("1StarText", "エラー"));
		info.m_starTexts[1] = Utf8ToMultiByte(item.value("2StarText", "エラー"));
		info.m_starTexts[2] = Utf8ToMultiByte(item.value("3StarText", "エラー"));
		info.m_starPinNeed[0] = item.value("pinFallen1Star", 0);
		info.m_starPinNeed[1] = item.value("pinFallen2Star", 0);
		info.m_starPinNeed[2] = item.value("pinFallen3Star", 0);

		// stageNo をキーとしてマップに格納
		if (info.m_stageNo > 0)
		{
			m_stageTable[info.m_stageNo] = info;
		}
	}

	return true;
}

void StageManager::Release()
{}

std::string StageManager::Utf8ToMultiByte(const std::string & utf8Str)
{
	if (utf8Str.empty()) return "";

	// 1. UTF-8 -> ワイド文字列 (UTF-16) へ変換
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	std::wstring wstr(wideLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], wideLen);

	// 2. ワイド文字列 (UTF-16) -> Shift-JIS (CP932) へ変換
	int ansiLen = WideCharToMultiByte(932, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string ansiStr(ansiLen, 0);
	WideCharToMultiByte(932, 0, wstr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

	// 末尾のヌル文字を除外して返す
	if (!ansiStr.empty() && ansiStr.back() == '\0') {
		ansiStr.pop_back();
	}

	return ansiStr;
}
