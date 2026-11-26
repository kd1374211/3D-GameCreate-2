#pragma once

inline Math::Quaternion ConvertEulerVec3ToQuat(Math::Vector3 vec3)
{
	// Radian変換
	float radX = DirectX::XMConvertToRadians(vec3.x);
	float radY = DirectX::XMConvertToRadians(vec3.y);
	float radZ = DirectX::XMConvertToRadians(vec3.z);

	// Quat変換
	Math::Quaternion Quat = Math::Quaternion::CreateFromYawPitchRoll(radY, radX, radZ);
	Quat.Normalize();

	// リターン
	return Quat;
}

// UTF-8 の std::string を Shift-JIS (ANSI) の std::string に変換する関数
inline std::string Utf8ToMultiByte(const std::string& utf8Str)
{
	if (utf8Str.empty()) return "";

	// 1. UTF-8 -> ワイド文字列 (UTF-16) へ変換
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	std::wstring wstr(wideLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], wideLen);

	// 2. ワイド文字列 (UTF-16) -> Shift-JIS (CP932) へ変換
	int ansiLen = WideCharToMultiByte(932, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string ansiStr(ansiLen, 0);
	WideCharToMultiByte(932, 0, wstr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

	// 末尾のヌル文字を除外して返す
	if (!ansiStr.empty() && ansiStr.back() == '\0') {
		ansiStr.pop_back();
	}

	return ansiStr;
}

