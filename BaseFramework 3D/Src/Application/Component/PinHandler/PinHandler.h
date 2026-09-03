#pragma once
#include "../../Const/PinTypes.h"

class PinBase;
struct LanePinData;

class PinHandler
{
public:

	PinHandler() {}
	~PinHandler() {}

	// ピンプールに必要な量だけ追加
	void CreatePinPool(PinType type, size_t amount);

	// 現在アクティブなピンを全て非活性化
	void DespawnAllPins();

	// このフレームのピンを召喚
	void SpawnPinsForThisFrame(const std::vector<LanePinData>& pinsToSpawn);

	// 倒れたピン数を取得
	int GetFallenPinCount();

	// 残ったピンを再配置＆倒れたピンをリストから除外
	void CheckAndResetRemainingPins(const std::vector<LanePinData>& pinsToSpawn);

	// 全部のピンが倒れたかを確認
	bool CheckIsAllPinsFallen()const;

private:

	// 特定の位置に特定のピンを召喚
	void SpawnPin(PinType type, Math::Vector3 startPos, int pinIndex);

	// ピンプールから未使用のピンを取得
	std::shared_ptr<PinBase> GetUnusedPin(PinType type);

	// ピンプール
	std::array<std::vector<std::shared_ptr<PinBase>>, static_cast<size_t>(PinType::Number)> m_pinPool;

	// 現在アクティブなピンのリスト
	std::vector<std::weak_ptr<PinBase>> m_activePins;

};