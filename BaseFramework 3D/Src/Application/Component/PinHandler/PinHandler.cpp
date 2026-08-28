#include "PinHandler.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/BowlingPin/NormalPin/NormalPin.h"

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
