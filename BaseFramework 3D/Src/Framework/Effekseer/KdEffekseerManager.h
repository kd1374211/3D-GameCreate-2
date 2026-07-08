#pragma once

class KdEffekseerObject;

class KdEffekseerManager
{
public:

	void Create(int w, int h);

	void Update();
	void Draw();

	struct PlayEfkInfo
	{
		std::string FileName = "";
		Math::Vector3 Pos = Math::Vector3::Zero;
		Math::Vector3 Size = Math::Vector3::One;
		Math::Vector3 Rotate = Math::Vector3::One;
		float Speed = 1.0f;
		bool IsLoop = false;
	};

	// Effekseerエフェクト再生
	std::weak_ptr<KdEffekseerObject> Play(const std::string& effName, const DirectX::SimpleMath::Vector3& pos, const float size = 1, const float speed = 1, const bool isLoop = false);

	void StopAllEffect();
	void StopEffect(const std::string& name);
	void StopEffect(const Effekseer::Handle& handle);

	void OnPauseEfkUpdate()
	{
		m_isPause = true;
	}

	void Release();
	void Reset();

	void SetPos(const int handle, const Math::Vector3& pos);
	void SetRotation(const int handle, const Math::Vector3& axis, const float angle);
	void SetWorldMatrix(const int handle, const Math::Matrix& mWorld);

	void SetScale(const int handle, const Math::Vector3& scale);
	void SetScale(const int handle, const float scale);

	void SetSpeed(const int handle, const float speed = 1.0f);

	void SetPause(const int handle, const bool isPause);

	// 再生中かどうか
	const bool IsPlaying(const int handle) const;

	// カメラセット
	void SetCamera(const std::shared_ptr<KdCamera>& camera)
	{
		m_wpCamera = camera;
	}

private:

	const Effekseer::Vector3D GetEfkVec3D(const Math::Vector3& vec) const
	{
		return Effekseer::Vector3D(vec.x, vec.y, vec.z);
	}

	// Effekseerエフェクトの再生
	std::weak_ptr<KdEffekseerObject> Play(const PlayEfkInfo& info);
	std::weak_ptr<KdEffekseerObject> Play(const std::shared_ptr<KdEffekseerObject>& spObject);

	void UpdateEffekseerEffect();
	void UpdateEkfCameraMatrix();

	// エフェクトのレンダラー
	EffekseerRendererDX11::RendererRef m_efkRenderer = nullptr;

	// エフェクトのマネージャー
	Effekseer::ManagerRef m_efkManager = nullptr;

	// エフェクトリスト
	std::unordered_map<std::string, std::shared_ptr<KdEffekseerObject>> m_effectMap{};
	std::list<std::shared_ptr<KdEffekseerObject>>						m_nowEffectPlayList{};

	// カメラ
	std::weak_ptr<KdCamera> m_wpCamera;

	bool m_isPause = false;

public:

	static KdEffekseerManager& GetInstance()
	{
		static KdEffekseerManager instance;

		return instance;
	}

private:

	KdEffekseerManager() {}
	~KdEffekseerManager() { Release(); }
};

class KdEffekseerObject
{
public:

	// 再生中か
	bool IsPlaying();

	//エフェクトを止める
	void StopEffect()
	{
		SetLoop(false);
		KdEffekseerManager::GetInstance().StopEffect(GetHandle());
	}

	// 再生登録したマネージャー設定
	void SetParentManager(Effekseer::ManagerRef& parentManager)
	{
		m_parentManager = parentManager;
	}

	// 再生するエフェクト設定
	void SetEffect(Effekseer::EffectRef& effect) { m_effect = effect; }

	// ハンドル設定
	void SetHandle(Effekseer::Handle& handle) { m_handle = handle; }

	// 座標設定
	void SetPos(const Math::Vector3& pos);

	// サイズ設定
	void SetScale(const Math::Vector3 scale = { 1.0f, 1.0f, 1.0f });
	void SetScale(const float scale = 1.0f);

	// 速度設定
	void SetSpeed(const float speed = 1.0f);

	// ワールド行列設定
	void SetWorldMatrix(const Math::Matrix& mWorld);

	// ループ設定
	void SetLoop(const bool isLoop = false) { m_info.IsLoop = isLoop; }

	// 再生するエフェクトの各種情報設定
	void SetPlayEfkInfo(const KdEffekseerManager::PlayEfkInfo& info) { m_info = info; }

	// エフェクト取得
	Effekseer::EffectRef& WorkEffect() { return m_effect; }
	const Effekseer::EffectRef& GetEffect() const { return m_effect; }

	// ハンドル取得
	const Effekseer::Handle& GetHandle() const { return m_handle; }

	// 座標取得
	const Math::Vector3 GetPos() { return m_info.Pos; }

	// サイズ取得
	const Math::Vector3 GetSize() const { return m_info.Size; }

	// 速度取得
	const float GetSpeed() const { return m_info.Speed; }

	// ループするかどうか
	const bool IsLoop() const { return m_info.IsLoop; }

	// エフェクトの各種情報取得
	KdEffekseerManager::PlayEfkInfo& WorkPlayEfkInfo()				{ return m_info; }
	const KdEffekseerManager::PlayEfkInfo& GetPlayEfkInfo() const	{ return m_info; }

private:

	Effekseer::ManagerRef				m_parentManager = nullptr;
	Effekseer::EffectRef				m_effect = nullptr;
	Effekseer::Handle					m_handle = -1;

	KdEffekseerManager::PlayEfkInfo		m_info = {};
};

#define EffekseerPath "Asset/Data/Effect/"