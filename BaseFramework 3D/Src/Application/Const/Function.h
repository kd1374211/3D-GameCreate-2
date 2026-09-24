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