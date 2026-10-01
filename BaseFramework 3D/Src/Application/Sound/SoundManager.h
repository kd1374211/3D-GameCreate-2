#pragma once

struct SoundData
{
	std::string m_path = "Error";
	bool m_isLoop = false;
	float m_defaultVolume = 0.1f;
};

enum SoundType
{
	SE_BowlingBallHit,
	SE_BowlingBallRoll,
	SE_BowlingBallThrow,
	SE_BowlingBallFall,
	SE_BowlingBallFinish,
	SE_CursorMove,
	SE_CursorSelect,
	SE_PowerBarMove,
	SE_PowerBarSelect,
	SoundType_Max
};

class SoundManager
{
public:

	struct SoundManagerConsts
	{
		// デフォルト値(これが引数の場合は音データのデフォルト音量を使う)
		static constexpr float DefValue = -1.0f;

		// 音データjsonのパス
		static constexpr const char* SoundDataPath = "Assets/Data/Sound/SoundData.json";
	};

	void Init();
	void Update();
	
	// 音再生
	void Play(SoundType type, float volume = SoundManagerConsts::DefValue);

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