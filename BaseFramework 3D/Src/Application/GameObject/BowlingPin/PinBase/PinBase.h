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

	// 使用・不使用切り替え
	void ActivateBody();
	void DeactivateBody();

	// 使用状態チェック
	bool GetIsActive()const { return m_isActive; }

	// 倒れているかをチェック
	bool GetIsFallen()const { return m_isFallen; }

	// リセット
	void Reset();

	// 召喚
	void Spawn(Math::Vector3 pos, Math::Quaternion rot, int index);

	// プールに返す（削除）
	void Despawn();

	// 位置と回転のセッター
	void SetPos(const Math::Vector3 pos);
	void SetRot(const Math::Quaternion rot);

	// ピン番号のゲッター
	int GetPinIndex()const { return m_pinIndex; }

	// ピンが当てられたか
	bool GetIsHit()const { return m_isHit; }

	// 当たられたとき
	void OnHit(JPH::Vec3 vel);

protected:

	struct PinBaseConsts
	{
		static constexpr float OnHitVelocityMulti = 1.0f;
	};

	virtual void Init()override;

	// 倒れたかを確認
	bool CheckIsFallen();

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	std::shared_ptr<KdModelData> m_model;

	//座標
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Quaternion m_rot = Math::Quaternion::Identity;

	// 使用状態か
	bool m_isActive = false;
	bool m_isBodyActive = false;

	// 当たられたか
	bool m_isHit = false;

	// 吹っ飛び待ちか
	bool m_isHitPending = false;
	JPH::Vec3 m_pendingVelocity = JPH::Vec3::sZero();

	// 倒れているか
	bool m_isFallen = false;

	// ピンの管理番号
	int m_pinIndex = -1;
};