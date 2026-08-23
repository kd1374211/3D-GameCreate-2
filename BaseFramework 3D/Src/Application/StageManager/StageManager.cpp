#include "StageManager.h"
#include "../Scene/SceneManager.h"
#include "../GameObject/Chara/CharaManager.h"
#include "../GameObject/Camera/CameraBase.h"

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
	rootJson["stage_name"] = "CustomStage";

	// 1. 地形情報
	rootJson["terrain"] = {
	{ "model_path", m_terrainPath }
	};

	// 2. 天球情報
	rootJson["sky"] = {
	{ "model_path", m_skySpherePath }
	};

	// 3. 配置オブジェクト一覧
	nlohmann::json objList = nlohmann::json::array();
	for (const auto& obj : m_stageObjects)
	{
		nlohmann::json jObj;
		jObj["type"] = obj.m_type;
		jObj["position"] = { obj.m_position.x, obj.m_position.y, obj.m_position.z };
		jObj["rotation"] = { obj.m_rotation.x, obj.m_rotation.y, obj.m_rotation.z, obj.m_rotation.w };
		jObj["scale"] = { obj.m_scale.x,    obj.m_scale.y,    obj.m_scale.z };

		objList.push_back(jObj);
	}
	rootJson["objects"] = objList;

	std::ofstream outFile(filePath);
	if (!outFile.is_open()) return false;

	outFile << rootJson.dump(4);
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

	//リセット
	m_stageObjects.clear();
	
	// 1. 地形情報の読み込み
	// Load 時
	if (rootJson.contains("terrain"))
	{
		m_terrainPath = rootJson["terrain"].value("model_path", "Asset/Models/Terrain/Stage01/TestGround.gltf");
	}

	// 2. 地形情報の読み込み
	// Load 時
	if (rootJson.contains("sky"))
	{
		m_skySpherePath = rootJson["sky"].value("model_path", "Asset/Models/Sky/SkySphere/SkySphere.gltf");
	}

	// 3. 配置オブジェクトの読み込み
	if (rootJson.contains("objects") && rootJson["objects"].is_array())
	{
		for (const auto& jObj : rootJson["objects"])
		{
			StageObjectData data;
			data.m_type = jObj.value("type", "Pin");

			if (jObj.contains("position")) {
				data.m_position = { jObj["position"][0], jObj["position"][1], jObj["position"][2] };
			}
			if (jObj.contains("rotation")) {
				data.m_rotation = { jObj["rotation"][0], jObj["rotation"][1], jObj["rotation"][2], jObj["rotation"][3] };
			}
			if (jObj.contains("scale")) {
				data.m_scale = { jObj["scale"][0], jObj["scale"][1], jObj["scale"][2] };
			}

			m_stageObjects.push_back(data);
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

void StageManager::ApplyCameraTarget()
{
	auto wpCamera = SCENEMGR.GetCamera();
	if (wpCamera.expired()) return;
	auto spCamera = wpCamera.lock();

	if (IsEditMode())
	{
		// 地形(Ground)が [0] にある場合は +1 オフセット
		size_t targetIdx = static_cast<size_t>(m_selectedIndex) + 1;

		if (m_selectedIndex >= 0 && targetIdx < m_wpStageObject.size())
		{
			if (!m_wpStageObject[targetIdx].expired())
			{
				// 新しく生成された実体のポインタをカメラにセット
				spCamera->SetTarget(m_wpStageObject[targetIdx].lock());
				return;
			}
		}
		spCamera->SetTarget(nullptr);
	}
	else
	{
		// プレイモード中：プレイヤーをセット
		auto wpPlayer = CHARAMGR.GetPlayer();
		if (!wpPlayer.expired())
		{
			spCamera->SetTarget(wpPlayer.lock());
		}
		else
		{
			spCamera->SetTarget(nullptr);
		}
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
	for (const auto& objData : m_stageObjects)
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

int StageManager::CalculateStarCount(int stageNo, int pinFallen) const
{
	// マスタデータが存在しない、または無効なステージ番号の場合は最小の★1を返す
	auto it = m_stageTable.find(stageNo);
	if (it == m_stageTable.end())
	{
		return 1;
	}

	const auto& info = it->second;

	// ここで評価の計算

	return 1; // クリアした時点で★1は確定
}

int StageManager::CalculateCurrentStageStarCount(int pinFallen) const
{
	// 現在選択されているステージ番号（m_currentStageNo）を使って計算
	return CalculateStarCount(SCENEMGR.GetStageNo(), pinFallen);
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
		info.m_starTexts[0] = Utf8ToMultiByte(item.value("1StarText", "エラー"));
		info.m_starTexts[1] = Utf8ToMultiByte(item.value("2StarText", "エラー"));
		info.m_starTexts[2] = Utf8ToMultiByte(item.value("3StarText", "エラー"));

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
