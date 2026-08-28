#pragma once
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class BowlingBall :public KdGameObject
{
public:

	BowlingBall();
	~BowlingBall()override {}

	void Init(const Math::Vector3& a_startPos, float a_radius);
	void Update()override;
	void PostUpdate()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	// 投げ
	void Throw(const JPH::Vec3& startPos, const JPH::Vec3& direction, float power);

	// リセット
	void Reset();

private:

	// このクラス内の定数
	struct BowlingBallConsts
	{
		// 停止確認関連
		static constexpr float StopCheckBorder = 0.01f;
		static constexpr float RollEndTime = 1.5f;
	};

	// 停止確認
	void CheckIsStop();

	//モデル
	std::shared_ptr<KdModelWork> m_model;

	//位置
	Math::Vector3 m_pos;
	//回転
	Math::Quaternion m_rot;

	//半径
	float m_radius = 0.0f;

	// 連続停止時間
	float m_stopTimer = 0.0f;

	// 転がり中か
	bool m_isRolling = false;

	// 落下チェック
	bool m_isFall = false;

	// 物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;
};