#include "CameraManager.h"
#include "CameraBase.h"

void CameraManager::Init()
{
	m_activeCamera = CameraType::Game_Normal;
}

void CameraManager::SetCamera(CameraType type, std::shared_ptr<CameraBase> camera)
{
	// 有効確認
	if (!CheckValidIndex(type))return;

	// 既にあるなら破壊
	DeleteCamera(type);

	// セット
	m_wpCamera[(size_t)type] = camera;
}

void CameraManager::SetDefaultCamera(CameraType type)
{
	// 有効確認
	if (!CheckValidIndex(type))return;

	// 指定されたカメラをtrue・他をfalseに
	for (size_t i = 0; i < (size_t)CameraType::Number; i++)
	{
		if (const auto& spCamera = m_wpCamera[i].lock())
		{
			spCamera->SetIsDefault(i == (size_t)type);
		}
	}

	// アクティブカメラを更新
	m_activeCamera = type;
}

void CameraManager::DeleteCamera(CameraType type)
{
	// 有効確認
	if (!CheckValidIndex(type))return;

	// 存在確認
	if (const auto& spCamera = m_wpCamera[(size_t)type].lock())
	{
		// 破壊
		spCamera->SetExpire();
	}
}
