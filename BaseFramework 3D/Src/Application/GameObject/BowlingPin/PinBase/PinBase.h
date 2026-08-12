#pragma once
#include "../../../Physics/PhysicsManager.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class PinBase :public KdGameObject
{
public:

	PinBase() {}
	~PinBase() {}

	void Update()override;
	void PostUpdate()override;

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	//プレイヤーに当たられたとき
	void OnHitByPlayer(JPH::Vec3 playerVelocity);

protected:

	virtual void Init()override;

	bool       m_isHitPending = false;    // ヒット通知の保留フラグ
	JPH::Vec3  m_pendingVelocity = JPH::Vec3::sZero(); // プレイヤー速度の保持

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	bool m_isRagdoll = false;	//ラグドール化しているかどうか
	std::shared_ptr<KdModelWork> m_model;

	//接触後から消滅までのタイマー
	float m_destroyTimer = 10.0f;

	//座標
	Math::Vector3 m_pos = Math::Vector3::Zero;

};