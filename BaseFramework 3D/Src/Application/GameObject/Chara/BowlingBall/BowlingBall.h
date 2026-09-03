#pragma once
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class BowlingBall :public KdGameObject
{
public:

	BowlingBall();
	~BowlingBall()override {}

	void Init(float a_radius);
	void Update()override;
	void PostUpdate()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	// 投げ
	void Throw(const Math::Vector3& startPos, const Math::Vector3& direction, float power);

	// リセット
	void Reset();

	// 停止・落下確認用
	bool IsRolling() const { return m_isRolling; }
	bool IsFall() const { return m_isFall; }

	// 位置・回転セット
	void Respawn(const Math::Vector3& pos, const Math::Quaternion& rot);

private:

	// このクラス内の定数
	struct BowlingBallConsts
	{
		// 停止確認関連
		static constexpr float StopCheckBorder = 0.01f;
		static constexpr float RollEndTime = 1.5f;
		
		// 発射速度
		static constexpr float ThrowPowerMulti = 7.0f;

		// 重さ
		static constexpr float BallMass = 7.0f;
	};

	// 活性化・非活性化
	void ActivateBody();
	void DeactivateBody();

	// 停止確認
	void CheckIsStop();

	//モデル
	std::shared_ptr<KdModelWork> m_model;

	//位置
	Math::Vector3 m_pos;
	//回転
	Math::Quaternion m_rot;

	// 連続停止時間
	float m_stopTimer = 0.0f;

	// 転がり中か
	bool m_isRolling = false;

	// 落下チェック
	bool m_isFall = false;

	// 物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	// 物理が有効か
	bool m_isActive = false;
};