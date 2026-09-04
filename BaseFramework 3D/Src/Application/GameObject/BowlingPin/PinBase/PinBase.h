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
	void Activate();
	void Deactivate();

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

protected:

	virtual void Init()override;

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	std::shared_ptr<KdModelWork> m_model;

	//座標
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Quaternion m_rot = Math::Quaternion::Identity;

	// 使用状態か
	bool m_isActive = false;

	// 倒れているか
	bool m_isFallen = false;

	// ピンの管理番号
	int m_pinIndex = -1;
};