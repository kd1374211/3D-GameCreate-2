#pragma once

class CameraBase;

enum class CameraType
{
	Game_Normal,
	Game_LookingDown,
	Debug,
	Number
};

class CameraManager
{
public:

	void Init();

	// カメラセット
	void SetCamera(CameraType type, std::shared_ptr<CameraBase> camera);
	
	// 描画用のカメラセット
	void SetDefaultCamera(CameraType type);
	
	// カメラ取得
	std::weak_ptr<CameraBase> GetCamera(CameraType type)const 
	{ 
		// 有効確認
		if (CheckValidIndex(type))return m_wpCamera[(size_t)type];
		// 無効なら通常カメラを返す
		else return GetCamera(CameraType::Game_Normal);
	}
	std::weak_ptr<CameraBase> GetActiveCamera()const { return GetCamera(m_activeCamera); }

private:

	CameraManager() {}
	~CameraManager() {}

	// カメラ破壊
	void DeleteCamera(CameraType type);

	// インデックス有効確認
	const bool CheckValidIndex(CameraType type)const { return type != CameraType::Number; }

	// 各カメラ
	std::weak_ptr<CameraBase> m_wpCamera[(size_t)CameraType::Number];

	// 現在アクティブなカメラ
	CameraType m_activeCamera = CameraType::Game_Normal;

public:

	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}

};

#define CAMERAMGR CameraManager::Instance()