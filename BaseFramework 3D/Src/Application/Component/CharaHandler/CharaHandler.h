#pragma once

enum class RollingState
{
	NotRolling, // 投球前
	Rolling,    // 投球中
	RollEnd    // 投球終了
};

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

	//プレイヤー取得
	std::shared_ptr <BowlingBall> GetPlayerBall() { return m_playerBall; }

	// ボールの投球状態フラグを確認する
	void CheckRollingState();

	// ボールの投球状態フラグを取得する
	RollingState GetRollingState()const { return m_rollingState; }

	// ボールの投球可能フラグをオン
	void StartThrow();

private:

	// プレイヤー召喚
	void InitPlayerBall();

	// リセット
	void ResetPlayerBall();
	// リスポーン
	void RespawnPlayerBall(const Math::Vector3& pos, const Math::Quaternion& rot);

	// ボール保持
	std::shared_ptr<BowlingBall> m_playerBall;

	// 投球状態
	RollingState m_rollingState = RollingState::NotRolling;
};
