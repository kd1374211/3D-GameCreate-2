#include "UserSaveManager.h"
#include "../Scene/SceneManager.h"

void UserSaveManager::Init()
{
	if (!LoadUserSaveData())
	{
		//ロード失敗時の処理があるなら書く
	}
}

void UserSaveManager::UpdateSave()
{}

const StageSaveData* UserSaveManager::GetUserSave(int stageNo) const
{
	auto it = m_stageSave.find(stageNo);
	if (it != m_stageSave.end())
	{
		return &(it->second);
	}
	return nullptr;
}

const StageSaveData* UserSaveManager::GetUserSave() const
{
	//現在のステージ番号を見る
	return GetUserSave(SCENEMGR.GetStageNo());
}

StageSaveData* UserSaveManager::WorkUserSave()
{
	auto it = m_stageSave.find(SCENEMGR.GetStageNo());
	if (it != m_stageSave.end())
	{
		return &(it->second);
	}
	return nullptr;
}

bool UserSaveManager::LoadUserSaveData()
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

bool UserSaveManager::SaveUserSaveData()
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
