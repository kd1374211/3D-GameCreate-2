#include "EffectManager.h"
#include "EffectsInclude.h"
#include "../../Scene/SceneManager.h"

void EffectManager::Init()
{
	// データの読み込み
	LoadData();
}

void EffectManager::SpawnEffect(Math::Vector3 pos, EffectType type)
{
	std::shared_ptr<EffectBase> effectObj;
	switch (type)
	{
	case EffectType::HitImpact:
		effectObj = std::make_shared<PinHit>();
		effectObj->SetPos(pos);
		SCENEMGR.AddObject(effectObj);
		break;
	}
}

void EffectManager::LoadData()
{
	std::ifstream file("Asset/Data/Effect/EffectData.json");
	if (!file.is_open())
	{
		return ; // ファイルが開けない場合
	}

	nlohmann::json rootJson;
	try
	{
		file >> rootJson;
	}
	catch (...)
	{
		file.close();
		return ; // JSONの構文エラー等
	}
	file.close();

	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson["Effects"])
	{
		EffectData data;
		int effectID = -1;
		std::string texPath = "";

		int splitX = 1;
		int splitY = 1;

		// .value("キー名", デフォルト値) を使うことで、キーが存在しなくても安全に取得可能
		effectID = item.value("effectID", -1);
		texPath = item.value("texPath", "Error");
		splitX = item.value("splitX", 1);
		splitY = item.value("splitY", 1);
		data.m_scale = item.value("scale", 1.0f);
		data.m_animSpeed = item.value("animSpeed", 1.0f);
		data.m_isLoop = item.value("isLoop", false);
		
		// animMaxは X * Y
		data.m_animMax = splitX * splitY;

		// 画像をロード
		data.m_polygon = std::make_shared<KdSquarePolygon>();
		data.m_polygon->SetMaterial(texPath);

		// 画像を縦横分割
		data.m_polygon->SetSplit(splitX, splitY);

		// 有効なIDならリストに追加
		if (effectID != -1 && effectID < (size_t)EffectType::Number)
		{
			m_effectData[effectID] = data;
		}
	}
}
