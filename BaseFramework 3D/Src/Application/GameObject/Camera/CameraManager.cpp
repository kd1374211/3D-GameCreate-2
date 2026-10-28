#include "CameraManager.h"
#include "CameraBase.h"

void CameraManager::Init()
{
	m_activeCamera = CameraType::Game;
}

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
	const auto& gameCam = m_wpGameCamera.lock();
	const auto& debugCam = m_wpDebugCamera.lock();

	if (type == CameraType::Game)
	{
		if (gameCam)
		{
			gameCam->SetIsDefault(true);
		}

		if (debugCam)
		{
			debugCam->SetIsDefault(false);
		}
	}
	else
	{
		if (gameCam)
		{
			gameCam->SetIsDefault(false);
		}

		if (debugCam)
		{
			debugCam->SetIsDefault(true);
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
