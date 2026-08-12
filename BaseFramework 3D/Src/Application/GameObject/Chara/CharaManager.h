#pragma once
#include "Player/Player.h"

class CharaManager
{
public:

	//初期化
	void Init() {};

	//プレイヤーセット
	void SetPlayer(std::shared_ptr<Player> player) { m_wpPlayer = player; }

	//プレイヤーの位置をリセット
	void ResetPlayerPos();

	//プレイヤー取得
	std::weak_ptr<Player> GetPlayer() { return m_wpPlayer; }

private:

	CharaManager() {}
	~CharaManager() {}

	//プレイヤー保持
	std::weak_ptr<Player> m_wpPlayer;

public:

	static CharaManager& GetInstance()
	{
		static CharaManager instance;
		return instance;
	}

};

#define CHARAMGR CharaManager::GetInstance()