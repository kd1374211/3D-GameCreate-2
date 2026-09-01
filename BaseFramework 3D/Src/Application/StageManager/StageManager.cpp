#include "StageManager.h"
#include "../Scene/SceneManager.h"
#include "../GameObject/Chara/CharaManager.h"
#include "../GameObject/Camera/CameraBase.h"
#include "../Component/PinHandler/PinHandler.h"
#include "../Const/PinTypes.h"

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
	for (auto& obj : m_wpStageObject)
	{
		if (obj.expired())continue;

		//削除
		obj.lock()->SetExpire();
	}
	m_wpStageObject.clear();

	//ピン数リセット
	ResetPinCount();
}

bool StageManager::SaveStage(const std::string& filePath)
{
	nlohmann::json rootJson;

	// 1. 地形情報
	rootJson["terrain"] = {
		{ "model_path", m_terrainPath }
	};

	// 2. 天球情報
	rootJson["sky"] = {
		{ "model_path", m_skySpherePath }
	};

	// 3. 一般配置オブジェクト一覧
	nlohmann::json objList = nlohmann::json::array();
	for (const auto& obj : m_stageGimmicks)
	{
		nlohmann::json jObj;
		jObj["type"] = obj.m_type;
		jObj["position"] = { obj.m_position.x, obj.m_position.y, obj.m_position.z };
		jObj["rotation"] = { obj.m_rotation.x, obj.m_rotation.y, obj.m_rotation.z, obj.m_rotation.w };
		jObj["scale"] = { obj.m_scale.x,    obj.m_scale.y,    obj.m_scale.z };

		objList.push_back(jObj);
	}
	rootJson["objects"] = objList;

	// 4. ピン配置情報一覧
	nlohmann::json pinList = nlohmann::json::array();
	for (const auto& pin : m_stagePins)
	{
		nlohmann::json jPin;
		jPin["type"] = pin.m_type;
		jPin["position"] = { pin.m_position.x, pin.m_position.y, pin.m_position.z };
		jPin["rotation"] = { pin.m_rotation.x, pin.m_rotation.y, pin.m_rotation.z, pin.m_rotation.w };
		jPin["scale"] = { pin.m_scale.x,    pin.m_scale.y,    pin.m_scale.z };

		pinList.push_back(jPin);
	}
	rootJson["pins"] = pinList;

	// ファイルへの書き出し
	std::ofstream outFile(filePath);
	if (!outFile.is_open()) return false;

	// インデント4
	outFile << rootJson.dump(4);
	outFile.close();

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
	m_stageGimmicks.clear();
	m_stagePins.clear(); // ピン配置データもリセット

	// 1. 地形情報の読み込み
	if (rootJson.contains("terrain"))
	{
		m_terrainPath = rootJson["terrain"].value("model_path", "Asset/Models/Terrain/Stage01/Stage01.gltf");
	}

	// 2. 天球情報の読み込み
	if (rootJson.contains("sky"))
	{
		m_skySpherePath = rootJson["sky"].value("model_path", "Asset/Models/Sky/SkySphere/SkySphere.gltf");
	}

	// 3. 一般配置オブジェクトの読み込み（FinishAreaなど）
	if (rootJson.contains("objects") && rootJson["objects"].is_array())
	{
		for (const auto& jObj : rootJson["objects"])
		{
			StageObjectData data;
			data.m_type = jObj.value("type", "Error");

			if (jObj.contains("position")) {
				data.m_position = { jObj["position"][0], jObj["position"][1], jObj["position"][2] };
			}
			if (jObj.contains("rotation")) {
				data.m_rotation = { jObj["rotation"][0], jObj["rotation"][1], jObj["rotation"][2], jObj["rotation"][3] };
			}
			if (jObj.contains("scale")) {
				data.m_scale = { jObj["scale"][0], jObj["scale"][1], jObj["scale"][2] };
			}

			m_stageGimmicks.push_back(data);
		}
	}

	// 4. ピン配置情報の読み込み（新規追加！）
	if (rootJson.contains("pins") && rootJson["pins"].is_array())
	{
		for (const auto& jPin : rootJson["pins"])
		{
			StageObjectData pinData;

			pinData.m_type = jPin.value("type", "Error");

			if (jPin.contains("position")) {
				pinData.m_position = { jPin["position"][0], jPin["position"][1], jPin["position"][2] };
			}
			if (jPin.contains("rotation")) {
				pinData.m_rotation = { jPin["rotation"][0], jPin["rotation"][1], jPin["rotation"][2], jPin["rotation"][3] };
			}
			if (jPin.contains("scale")) {
				pinData.m_scale = { jPin["scale"][0], jPin["scale"][1], jPin["scale"][2] };
			}

			m_stagePins.push_back(pinData);
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
	KdDebugGUI::Instance().AddLog("obj: %d\n", m_wpStageObject.size());

	size_t targetIdx = static_cast<size_t>(m_selectedIndex);
	if (targetIdx < m_wpStageObject.size() && !m_wpStageObject[targetIdx].expired())
	{
		auto spObj = m_wpStageObject[targetIdx].lock();

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

	if (!LoadStageSaveData())
	{
		//ロード失敗時の処理があるなら書く
	}
}

void StageManager::BuildStage(StageBuildMode mode)
{
	//リセット
	ResetStage();

	//地形生成
	std::shared_ptr<Ground> ground = std::make_shared<Ground>(m_terrainPath, Math::Vector3::Zero, Math::Quaternion::Identity);
	// ゲームシーンの管理リストに追加
	SCENEMGR.AddObject(ground);
	//後で消せるようにリスト持ち
	m_wpTerrain = ground;

	//天球生成
	std::shared_ptr<SkySphere> sky = std::make_shared<SkySphere>(m_skySpherePath);
	// ゲームシーンの管理リストに追加
	SCENEMGR.AddObject(sky);
	//後で消せるようにリスト持ち
	m_wpSkySphere = sky;

	//背景モードはこれ以降をロードしない
	if (mode == StageBuildMode::Background)return;

	// 読み込んだデータをもとにオブジェクト生成
	for (const auto& objData : m_stageGimmicks)
	{
		std::shared_ptr<KdGameObject> obj;

		//タイプ名有効フラグ
		bool isValidType = false;

		if (objData.m_type == "NormalPin")
		{
			// ピンの生成
			obj = std::make_shared<NormalPin>(objData.m_position, objData.m_rotation);

			isValidType = true;
		}
		else if (objData.m_type == "Goal")
		{
			// ピンの生成
			obj = std::make_shared<FinishArea>(objData.m_position, objData.m_rotation, objData.m_scale);

			isValidType = true;
		}

		// タイプ名チェック
		if (isValidType)
		{
			// ゲームシーンの管理リストに追加
			SCENEMGR.AddObject(obj);

			//後で消せるようにリスト持ち
			m_wpStageObject.push_back(obj);
		}
	}

	// ★ リビルド完了後、新しい実体のポインタをカメラに再設定する
	//ApplyCameraTarget();
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

const StageSaveData* StageManager::GetUserSave(int stageNo) const
{
	auto it = m_stageSave.find(stageNo);
	if (it != m_stageSave.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const StageSaveData* StageManager::GetUserSave() const
{
	//現在のステージ番号を見る
	return GetUserSave(SCENEMGR.GetStageNo());
}

StageSaveData* StageManager::WorkUserSave()
{
	auto it = m_stageSave.find(SCENEMGR.GetStageNo());
	if (it != m_stageSave.end())
	{
		return &(it->second);
	}
	return nullptr;
}

int StageManager::CalculateStarCount(int stageNo, int pinFallen, bool isClear) const
{
	// マスタデータが存在しない、または無効なステージ番号の場合は最小の★1を返す
	auto it = m_stageTable.find(stageNo);
	if (it == m_stageTable.end())
	{
		return 1;
	}

	const auto& info = it->second;

	// ここで評価の計算
	int starCount = 0;

	// クリアしていないなら0確定
	if (isClear)
	{
		for (int i = 0; i < StageManagerConsts::StarCountMax; i++)
		{
			if (pinFallen >= info.m_starPinNeed[i])
			{
				starCount++;
			}
			else break;
		}
	}

	return starCount; // 計算した星数
}

int StageManager::CalculateCurrentStageStarCount(int pinFallen, bool isClear) const
{
	// 現在選択されているステージ番号（m_currentStageNo）を使って計算
	return CalculateStarCount(SCENEMGR.GetStageNo(), pinFallen, isClear);
}

bool StageManager::SaveUserData()
{
	nlohmann::json rootJson = nlohmann::json::array();
	
	// 3. 配置オブジェクト一覧
	for (const auto& obj : m_stageSave)
	{
		nlohmann::json jObj;
		jObj["stageNo"] = obj.second.m_stageNo;
		jObj["isClear"] = obj.second.m_isClear;
		jObj["bestPinsFallen"] = obj.second.m_bestPinFallen;

		rootJson.push_back(jObj);
	}

	std::ofstream outFile("Asset/Data/StageData/StageSaveData.json");
	if (!outFile.is_open()) return false;

	outFile << rootJson.dump(4);
	outFile.close();
	return true;
}

void StageManager::CreatePinPool()
{
	// ハンドラーがないならリターン
	if (m_wpPinHandler.expired())return;

	// ピン種ごとの「最大必要数」を記録するマップ
	std::unordered_map<PinType, size_t> maxRequiredCounts;

	// 各レーンを見てピンごとに最大値を計算
	for (const auto& data : m_stageLaneDatas)
	{
		// このレーン内でのピン種ごとの個数を一時カウント
		std::unordered_map<PinType, size_t> currentLaneCounts;

		// このレーンにあるピンを見てデータを追加
		for (const auto& pins : data.m_stagePinData)
		{
			PinType type = ConvertStringToPinType(pins.m_type);

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

bool StageManager::LoadStageSaveData()
{
	// パスはクラス内部に直書きで保持
	const std::string masterJsonPath = "Asset/Data/StageData/StageSaveData.json";

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
	m_stageSave.clear();

	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson)
	{
		StageSaveData data;

		// .value("キー名", デフォルト値) を使うことで、キーが存在しなくても安全に取得可能
		data.m_stageNo = item.value("stageNo", 0);
		data.m_isClear = item.value("isClear", false);
		data.m_bestPinFallen = item.value("bestPinsFallen", 0);
		
		// stageNo をキーとしてマップに格納
		if (data.m_stageNo > 0)
		{
			m_stageSave[data.m_stageNo] = data;
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
