#pragma once

class BowlingBall;

class CharaHandler
{
public:

	CharaHandler() {}
	~CharaHandler() {}

	// 初期化
	void Init();

	// 次の投球を開始（リセット＆位置・方向設定）
	void StartNextThrow(const Math::Vector3& pos, const Math::Vector3& rot);

	//プレイヤー取得
	std::shared_ptr <BowlingBall> GetPlayerBall() { return m_playerBall; }

	// ボール投球確認
	void CheckBallThrow();

	// ボール投球後か
	bool IsBallThrown()const { return m_isBallThrown; }

	// ボールが動いているかの確認
	bool IsBallRolling()const;

	// ボールの投球可能フラグをオン
	void StartThrow();

	// ボールの終了時処理
	void EndThrow();

private:

	// プレイヤー召喚
	void InitPlayerBall();

	// リセット
	void ResetPlayerBall();
	// リスポーン
	void RespawnPlayerBall(const Math::Vector3& pos, const Math::Vector3& rot);

	// ボール保持
	std::shared_ptr<BowlingBall> m_playerBall;

	// ボール投球後フラグ
	bool m_isBallThrown = false;
};
