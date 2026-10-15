#include "SoundManager.h"

void SoundManager::Init()
{
	LoadSounds();
}

void SoundManager::Update()
{
	// 音の再生が終了したインスタンスを削除する
	for (auto it = m_storedSoundInstances.begin(); it != m_storedSoundInstances.end(); )
	{
		if (it->second.expired())
		{
			it = m_storedSoundInstances.erase(it);
		}
		else
		{
			++it;
		}
	}
}

std::weak_ptr<KdSoundInstance> SoundManager::PlayAndGetInst(SoundType type, float volume)
{
	// デフォルト音量かの確認
	bool isDefaultVolume = (volume == SoundManagerConsts::DefValue);

	// 音データ取得
	const SoundData& data = GetSoundData(type);

	// 音量がデフォルト値なら音データのデフォルト音量を使用
	if (isDefaultVolume)
	{
		volume = data.m_defaultVolume;
	}

	// 再生
	std::shared_ptr<KdSoundInstance> soundInstance = KdAudioManager::Instance().Play(data.m_path, data.m_isLoop);

	// 音量設定
	if (soundInstance)
	{
		soundInstance->SetVolume(volume);
		m_storedSoundInstances.emplace(type, soundInstance);
	}

	// インスタンスを返す
	return soundInstance;
}

void SoundManager::Play(SoundType type, float volume)
{
	// デフォルト音量かの確認
	bool isDefaultVolume = (volume == SoundManagerConsts::DefValue);

	// 音データ取得
	const SoundData& data = GetSoundData(type);

	// 音量がデフォルト値なら音データのデフォルト音量を使用
	if (isDefaultVolume)
	{
		volume = data.m_defaultVolume;
	}

	// 再生
	std::shared_ptr<KdSoundInstance> soundInstance = KdAudioManager::Instance().Play(data.m_path, data.m_isLoop);

	// 音量設定
	if (soundInstance)
	{
		soundInstance->SetVolume(volume);
		m_storedSoundInstances.emplace(type, soundInstance);
	}
}

void SoundManager::Stop(SoundType type)
{
	// StoredSoundInstancesから該当するタイプの音を停止
	for (const auto& it : m_storedSoundInstances)
	{
		if (it.first == type)
		{
			if (const auto& soundInstance = it.second.lock())
			{
				soundInstance->Stop();
			}
		}
	}
}

void SoundManager::StopAll()
{
	// 全ての音を停止
	KdAudioManager::Instance().StopAllSound();
}

void SoundManager::LoadSounds()
{
	std::ifstream file(SoundManagerConsts::SoundDataPath);
	if (!file.is_open())
	{
		return; // ファイルが開けない場合
	}

	nlohmann::json rootJson;
	try
	{
		file >> rootJson;
	}
	catch (...)
	{
		file.close();
		return; // JSONの構文エラー等
	}
	file.close();

	// 既存データをクリア
	m_soundData.clear();

	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson["soundData"])
	{
		SoundData data;

		// 各値を取得
		data.m_path = item.value("filePath", "Error");
		data.m_defaultVolume = item.value("defaultVolume", 0.1f);
		data.m_isLoop = item.value("loop", false);

		// 音データをベクターに追加
		m_soundData.push_back(data);
	}
}
