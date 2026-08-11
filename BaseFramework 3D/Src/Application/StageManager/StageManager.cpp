#include "StageManager.h"
#include "../Scene/SceneManager.h"
#include "../GameObject/Chara/CharaManager.h"
#include "../GameObject/Camera/CameraBase.h"

void StageManager::BuildStage()
{
	//リセット
	ResetStage();

	//地形生成
	std::shared_ptr<Ground> ground = std::make_shared<Ground>(m_terrainPath, Math::Vector3::Zero, Math::Quaternion::Identity);
	// ゲームシーンの管理リストに追加
	SCENEMGR.AddObject(ground);

	//後で消せるようにリスト持ち
	m_wpTerrain = ground;

	// 読み込んだデータをもとにオブジェクト生成
	for (const auto& objData : m_stageObjects)
	{
		std::shared_ptr<KdGameObject> obj;
		bool isValidType = false;

		if (objData.type == "NormalPin")
		{
			// ピンの生成処理（例）
			obj = std::make_shared<NormalPin>(objData.position, objData.rotation);
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

void StageManager::ResetStage()
{
	//地形削除
	if (!m_wpTerrain.expired())
	{
		m_wpTerrain.lock()->SetExpire();
	}

	//オブジェクトリセット
	for (auto& obj : m_wpStageObject)
	{
		if (obj.expired())continue;

		//削除
		obj.lock()->SetExpire();
	}
	m_wpStageObject.clear();
}

bool StageManager::SaveStage(const std::string& filePath)
{
	nlohmann::json rootJson;
	rootJson["stage_name"] = "CustomStage";

	// 1. 地形情報
	rootJson["terrain"] = {
	{ "model_path", m_terrainPath }
	};

	// 2. 配置オブジェクト一覧
	nlohmann::json objList = nlohmann::json::array();
	for (const auto& obj : m_stageObjects)
	{
		nlohmann::json jObj;
		jObj["type"] = obj.type;
		jObj["position"] = { obj.position.x, obj.position.y, obj.position.z };
		jObj["rotation"] = { obj.rotation.x, obj.rotation.y, obj.rotation.z, obj.rotation.w };
		jObj["scale"] = { obj.scale.x,    obj.scale.y,    obj.scale.z };

		objList.push_back(jObj);
	}
	rootJson["objects"] = objList;

	std::ofstream outFile(filePath);
	if (!outFile.is_open()) return false;

	outFile << rootJson.dump(4);
	return true;
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
		m_terrainPath = rootJson["terrain"].value("model_path", "Asset/Data/Model/Stage01.gltf");
	}

	// 2. 配置オブジェクトの読み込み
	if (rootJson.contains("objects") && rootJson["objects"].is_array())
	{
		for (const auto& jObj : rootJson["objects"])
		{
			StageObjectData data;
			data.type = jObj.value("type", "Pin");

			if (jObj.contains("position")) {
				data.position = { jObj["position"][0], jObj["position"][1], jObj["position"][2] };
			}
			if (jObj.contains("rotation")) {
				data.rotation = { jObj["rotation"][0], jObj["rotation"][1], jObj["rotation"][2], jObj["rotation"][3] };
			}
			if (jObj.contains("scale")) {
				data.scale = { jObj["scale"][0], jObj["scale"][1], jObj["scale"][2] };
			}

			m_stageObjects.push_back(data);
		}
	}

	BuildStage();
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
}

void StageManager::Release()
{}
