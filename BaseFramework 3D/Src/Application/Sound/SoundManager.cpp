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
	}
}

void SoundManager::Stop(SoundType type)
{
	// StoredSoundInstancesから該当するタイプの音を停止
	for (const auto& it : m_storedSoundInstances)
	{
		if (it.first == type)
		{
			if (auto soundInstance = it.second.lock())
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
	for (const auto& item : rootJson)
	{
		SoundData data;

		// .value("キー名", デフォルト値) を使うことで、キーが存在しなくても安全に取得可能
		data.m_path = item.value("path", "Error");
		data.m_isLoop = item.value("isLoop", false);
		data.m_defaultVolume = item.value("defaultVolume", 0.1f);

		// 音データをベクターに追加
		m_soundData.push_back(data);
	}
}
