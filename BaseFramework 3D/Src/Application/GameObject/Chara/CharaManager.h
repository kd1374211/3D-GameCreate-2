#pragma once
#include "Player/Player.h"

class CharaManager
{
public:

	//プレイヤーセット
	void SetPlayer(std::shared_ptr<Player> player) { m_wpPlayer = player; }

	//プレイヤー取得
	std::weak_ptr<Player> GetPlayer() { return m_wpPlayer; }

private:

	CharaManager() { Init(); }
	~CharaManager() {}

	void Init() {};

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