#pragma once

enum class RollingState
{
	NotRolling, // 投球前
	Rolling,    // 投球中
	Stopped,    // 投球終了(停止)
	Fallen		// 投球終了(落下)
};

class Player;
class BowlingBall;

class CharaHandler
{
public:

	CharaHandler() {}
	~CharaHandler() {}

	// 初期化
	void Init();

	// 次の投球を開始（リセット＆位置・方向設定）
	void StartNextThrow(const Math::Vector3& pos, const Math::Quaternion& rot);

	//プレイヤーの位置をリセット
	//void SetPlayerPos(const Math::Vector3& pos);
	void SetPlayerPosAndRot(const Math::Vector3& pos, const Math::Quaternion& rot);

	//プレイヤー取得
	std::shared_ptr<Player> GetPlayer() { return m_player; }
	std::shared_ptr <BowlingBall> GetBall() { return m_bowlingBall; }

	// ボールの投球状態フラグを確認する
	void CheckRollingState();

	// ボールの投球状態フラグを取得する
	RollingState GetRollingState()const { return m_rollingState; }

private:

	// プレイヤー召喚
	void InitPlayerAndBall();

	// リセット
	void ResetPlayerAndBall();

	// プレイヤー保持
	std::shared_ptr<Player> m_player;
	// ボール保持
	std::shared_ptr<BowlingBall> m_bowlingBall;

	// 投球状態
	RollingState m_rollingState = RollingState::NotRolling;
};
