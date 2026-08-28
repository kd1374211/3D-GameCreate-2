#pragma once

class PinBase;

enum class PinType
{
	NormalPin,
	Number
};

class PinHandler
{
public:

	PinHandler() {}
	~PinHandler() {}

	// ピンプールに必要な量だけ追加
	void CreatePinPool(PinType type, size_t amount);

	// ピンプールから未使用のピンを取得
	std::shared_ptr<PinBase> GetUnusedPin(PinType type);

private:

	// ピンプール
	std::array<std::vector<std::shared_ptr<PinBase>>, static_cast<size_t>(PinType::Number)> m_pinPool;

};