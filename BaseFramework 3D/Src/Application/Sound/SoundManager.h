#pragma once

struct SoundData
{
	std::string m_path = "Error";
	bool m_isLoop = false;
	float m_defaultVolume = 0.1f;
};

enum SoundType
{
	BGM_Title,
	BGM_Select,
	BGM_Ingame,
	BGM_Result,
	SE_Cursor,
	SE_Decide,
	SE_Throw,
	SE_Rolling,
	SE_HitPin,
	SE_WallHitAndLanding,
	SoundType_Max
};

class SoundManager
{
public:

	struct SoundManagerConsts
	{
		// デフォルト値(音量倍率)
		static constexpr float DefValue = 1.0f;

		// 音データjsonのパス
		static constexpr const char* SoundDataPath = "Asset/Data/Sound/SoundData.json";
	};

	void Init();
	void Update();

	// 音再生＆インスタンス保持
	std::weak_ptr<KdSoundInstance> PlayAndGetInst(SoundType type, float volume = SoundManagerConsts::DefValue);
	
	// 音再生
	void Play(SoundType type, float volumeMulti = SoundManagerConsts::DefValue);

	// 音停止（タイプを指定して該当する音を全て停止）
	void Stop(SoundType type);

	// 全ての音停止
	void StopAll();

private:

	SoundManager() {};
	~SoundManager() {};

	// 音データ取得
	const SoundData& GetSoundData(SoundType type) { return m_soundData[(size_t)type]; }

	// 音データ読み込み
	void LoadSounds();

	// 音データ保持
	std::vector<SoundData> m_soundData;

	// 音インスタンス保持
	std::map<SoundType, std::weak_ptr<KdSoundInstance>> m_storedSoundInstances;

public:

	static SoundManager& Instance()
	{
		static SoundManager instance;
		return instance;
	}

};

#define SOUNDMGR SoundManager::Instance()