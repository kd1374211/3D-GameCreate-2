#include "EffectManager.h"
#include "EffectsInclude.h"
#include "../../Scene/SceneManager.h"

void EffectManager::Init()
{
	// データの読み込み
	LoadData();

	// 仮置きテクスチャ生成
	m_sqTex = std::make_shared<KdTexture>();
	m_sqTex->Load("Asset/Textures/Effect/Confetti/RandomColoredSquare.png");
}

void EffectManager::PreUpdate()
{
	// 消去チェック
	CleanActiveList();

	// DEBUG
	KdDebugGUI::Instance().AddLog("EffectListSize : %d\n", m_wpEffectList.size());
}

void EffectManager::SpawnEffect(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 scaleMulti, EffectType type)
{
	std::shared_ptr<EffectBase> effectObj;
	switch (type)
	{
	case EffectType::HitImpact:
		effectObj = std::make_shared<PinHit>();
		break;
	case EffectType::LandingAndWallHit:
		effectObj = std::make_shared<Landing>();
		break;
	case EffectType::Confetti:
		// これは別枠
		SpawnConfettiSpawner();
		return;
	}

	// 変換成功したならパラメータを設定して召喚
	if (effectObj)
	{
		effectObj->SetPos(pos);
		effectObj->SetRot(rot);
		effectObj->SetScaleMulti(scaleMulti);
		SCENEMGR.AddObject(effectObj);

		// リストに追加
		m_wpEffectList.push_back(effectObj);
	}
}

void EffectManager::ClearActiveEffect()
{
	// 念のため先にアクティブなエフェクトを更新
	CleanActiveList();

	// 順番に見て残っていたらexpireセット
	for (auto& effect : m_wpEffectList)
	{
		if (const auto& spEffect = effect.lock())
		{
			spEffect->SetExpire();
		}
	}

	// もしスポナーがアクティブならexpire
	if (const auto& cfSpawner = m_cfSpawner.lock())
	{
		cfSpawner->SetExpire();
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
		KdSquarePolygon::PivotType pivot = KdSquarePolygon::PivotType::Center_Middle;
		int effectID = -1;
		std::string texPath = "";

		int splitX = 1;
		int splitY = 1;

		// .value("キー名", デフォルト値) を使うことで、キーが存在しなくても安全に取得可能
		effectID = item.value("effectID", -1);
		texPath = item.value("texPath", "Error");
		pivot = item.value("pivotType", KdSquarePolygon::PivotType::Center_Middle);
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

		// ピボット変更
		data.m_polygon->SetPivot(pivot);

		// 画像を縦横分割
		data.m_polygon->SetSplit(splitX, splitY);

		// 有効なIDならリストに追加
		if (effectID != -1 && effectID < (size_t)EffectType::Number)
		{
			m_effectData[effectID] = data;
		}
	}
}

void EffectManager::SpawnConfettiSpawner()
{
	// 無いことを確認して新しく召喚
	if (!m_cfSpawner.expired())return;

	std::shared_ptr<ConfettiSpawner> spawner = std::make_shared<ConfettiSpawner>();
	m_cfSpawner = spawner;
	SCENEMGR.AddObject(spawner);
}

void EffectManager::CleanActiveList()
{
	// オブジェクトリストを確認
	auto it = m_wpEffectList.begin();
	while (it != m_wpEffectList.end())
	{
		std::shared_ptr<EffectBase> spEffect = it->lock();
		if (!spEffect)
		{
			// 無効なオブジェクトをリストから削除
			it = m_wpEffectList.erase(it);
		}
		else
		{
			++it; // 次の要素へイテレータを進める
		}
	}
}
