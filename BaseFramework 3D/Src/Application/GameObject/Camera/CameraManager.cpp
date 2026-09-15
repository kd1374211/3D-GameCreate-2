#include "CameraManager.h"
#include "CameraBase.h"

void CameraManager::Init()
{}

void CameraManager::SetGameCamera(std::shared_ptr<CameraBase> camera)
{
	// 既にあるなら破壊
	DeleteGameCamera();

	// セット
	m_wpGameCamera = camera;
}

void CameraManager::SetDebugCamera(std::shared_ptr<CameraBase> camera)
{
	// 既にあるなら破壊
	DeleteDebugCamera();

	// セット
	m_wpDebugCamera = camera;
}

void CameraManager::SetDefaultCamera(CameraType type)
{
	if (type == CameraType::Game)
	{
		if (!m_wpGameCamera.expired())
		{
			m_wpGameCamera.lock()->SetIsDefault(true);
		}

		if (!m_wpDebugCamera.expired())
		{
			m_wpDebugCamera.lock()->SetIsDefault(false);
		}
	}
	else
	{
		if (!m_wpGameCamera.expired())
		{
			m_wpGameCamera.lock()->SetIsDefault(false);
		}

		if (!m_wpDebugCamera.expired())
		{
			m_wpDebugCamera.lock()->SetIsDefault(true);
		}
	}

	// アクティブカメラを更新
	m_activeCamera = type;
}

void CameraManager::DeleteGameCamera()
{
	if (!m_wpGameCamera.expired())
	{
		m_wpGameCamera.lock()->SetExpire();
	}
}

void CameraManager::DeleteDebugCamera()
{
	if (!m_wpDebugCamera.expired())
	{
		m_wpDebugCamera.lock()->SetExpire();
	}
}

std::weak_ptr<CameraBase> CameraManager::GetActiveCamera()
{
	if (m_activeCamera == CameraType::Game)
	{
		return GetGameCamera();
	}
	else if (m_activeCamera == CameraType::Debug)
	{
		return GetDebugCamera();
	}

	// エラー
	return GetGameCamera();
}
