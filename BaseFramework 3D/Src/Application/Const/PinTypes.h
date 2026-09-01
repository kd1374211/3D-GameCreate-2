#pragma once

enum class PinType
{
	NormalPin,
	Number,
	Error = 999
};

// 共通のインライン関数（またはヘッダー専用関数）として定義
inline PinType ConvertStringToPinType(const std::string& typeStr)
{
	static const std::unordered_map<std::string, PinType> typeMap = {
		{ "Normal",    PinType::NormalPin },
	};

	auto it = typeMap.find(typeStr);
	if (it != typeMap.end())
	{
		return it->second;
	}

	return PinType::Error;
}