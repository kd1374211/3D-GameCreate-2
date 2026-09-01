#include "PinHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../StageManager/StageManager.h"
#include "../../GameObject/BowlingPin/NormalPin/NormalPin.h"
#include "../../Const/PinTypes.h"

void PinHandler::CreatePinPool(PinType type, size_t amount)
{
	// ピンのプールへの追加とシーンへの追加
	for (size_t i = 0; i < amount; i++)
	{
		std::shared_ptr<PinBase> pin;

		switch (type)
		{
		case PinType::NormalPin:
			pin = std::make_shared<NormalPin>(Math::Vector3::Zero, Math::Quaternion::Identity);
			break;
		default:	// 無効なタイプ
			return;
		}
		m_pinPool[static_cast<size_t>(type)].push_back(pin);
		SCENEMGR.AddObject(pin);
	}
}

void PinHandler::DespawnAllPins()
{
	// 全てのピンを見て非活性化させる
	//for (const auto& pin : m_pinPool[type])
	//{
	//	pin->Despawn();
	//}

	// 全てのアクティブなピンを見てデスポーン
	for (const auto& wpPin : m_activePins)
	{
		auto pin = wpPin.lock();
		if (pin)
		{
			pin->Despawn();
		}
	}

	// 現在アクティブなピンのリストもクリア
	m_activePins.clear();
}

void PinHandler::SpawnPinsForThisFrame(const std::vector<StageObjectData>& pinsToSpawn)
{
	DespawnAllPins(); // まず全てのピンを非活性化

	// 今回のフレームで必要なピンを召喚
	int index = 0;	// 召喚するピンの管理番号
	for (const auto& pinData : pinsToSpawn)
	{
		SpawnPin(ConvertStringToPinType(pinData.m_type), pinData.m_position, index);

		// 管理番号++
		index++;
	}
}

int PinHandler::GetFallenPinCount()
{
	// 倒れたピンの数をカウントする
	int fallenPinCnt = 0;

	// アクティブなピンのリストを走査
	for (const auto& wpPin : m_activePins)
	{
		auto pin = wpPin.lock();

		// あることを確認
		if (pin)
		{
			// もし倒れていたら
			if (pin->GetIsActive() && pin->GetIsFallen())
			{
				// 数える 
				fallenPinCnt++;
			}
		}
	}

	return fallenPinCnt;
}

void PinHandler::CheckAndResetRemainingPins(const std::vector<StageObjectData>& pinsToSpawn)
{
	// ピンリストを回って倒れていないピンの番号を取得
	std::vector<int> remainingPinIndex;

	for (const auto& wpPin : m_activePins)
	{
		auto pin = wpPin.lock();

		if (pin)
		{
			if (!pin->GetIsFallen() && pin->GetIsActive())
			{
				remainingPinIndex.push_back(pin->GetPinIndex());
			}
		}
	}

	// 一度ピンを全消去
	DespawnAllPins();

	// 残ったピン番号からデータを取得する
	for (int index : remainingPinIndex)
	{
		const auto& pinData = pinsToSpawn[index];

		// 取得したピンを生成
		SpawnPin(ConvertStringToPinType(pinData.m_type), pinData.m_position, index);
	}
}

void PinHandler::SpawnPin(PinType type, Math::Vector3 startPos, int pinIndex)
{
	std::shared_ptr<PinBase> pin = GetUnusedPin(type);
	if (pin)
	{
		// 見つかったピンを召喚
		pin->Spawn(startPos, Math::Quaternion::Identity, pinIndex);

		// 召喚したピンをアクティブなピンのリストに追加
		m_activePins.push_back(pin);
	}
	else
	{
		// 未使用のピンが見つからなかった場合の処理（必要に応じてログ出力など）
		//CreatePinPool(type, 1);
	}
}

std::shared_ptr<PinBase> PinHandler::GetUnusedPin(PinType type)
{
	size_t index = static_cast<size_t>(type);

	for (const auto& pin : m_pinPool[index])
	{
		if (pin && !pin->GetIsActive())
		{
			return pin; // 見つかったら即座に返して終了
		}
	}

	return nullptr; // 見つからなければ nullptr
}
