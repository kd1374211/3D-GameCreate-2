#pragma once

struct DeviceAndKeyConsts
{
	// メイン登録デバイス名
	static inline const std::string DeviceRegistName = "MainDevice";

	// キー登録名
	static inline const std::unordered_map<int, std::string> KeyRegistNames = 
	{
		{VK_SPACE,"SpaceKey"}
	};
};

// キー登録名取得関数
inline std::string GetKeyRegistName(int key)
{
	// 見つかったら
	auto name = DeviceAndKeyConsts::KeyRegistNames.find(key);
	if (name != DeviceAndKeyConsts::KeyRegistNames.end())
	{
		return name->second;
	}

	// 見つからなかったら
	return "Error";
}