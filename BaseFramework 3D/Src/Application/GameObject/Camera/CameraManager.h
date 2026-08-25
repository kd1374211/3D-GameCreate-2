#pragma once

class CameraBase;

enum class CameraType
{
	Game,
	Debug
};

class CameraManager
{
public:

	void Init();

	// カメラセット
	void SetGameCamera(std::shared_ptr<CameraBase> camera);
	void SetDebugCamera(std::shared_ptr<CameraBase> camera);
	
	// 描画用のカメラセット
	void SetDefaultCamera(CameraType type);

	// カメラ破壊
	void DeleteGameCamera();
	void DeleteDebugCamera();

	// カメラ取得
	std::weak_ptr<CameraBase> GetGameCamera() { return m_wpGameCamera; }
	std::weak_ptr<CameraBase> GetDebugCamera() { return m_wpDebugCamera; }

private:

	CameraManager() {}
	~CameraManager() {}

	//カメラ（ゲーム内）
	std::weak_ptr<CameraBase> m_wpGameCamera;

	//カメラ（デバッグ）
	std::weak_ptr<CameraBase> m_wpDebugCamera;

public:

	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}

};

#define CAMERAMGR CameraManager::Instance()