#pragma once

class PhysicsComponent {
public:

	PhysicsComponent() : m_bodyID(), m_isStatic(false) {}
	~PhysicsComponent() { Release(); }

	//モデル統一化
	bool Init(const std::string& path, bool isStatic);

	// 物理オブジェクトとして初期化する（球体）
	void Init(const Math::Vector3& inPos, float inRadius, bool inIsStatic);
	
	//ステータスの設定s
	void SetFriction(float friction);		//摩擦力
	void SetDamping(float linearDamp, float angularDamp);		//抵抗

	// 力を加える（転がす）
	void AddForce(const Math::Vector3& inForce);

	//速度を変更する
	void SetSpeed(const float& newSpeed);

	//方向を変更する
	void SetDirection(const Math::Vector3& newDir);

	//速度・方向を変更する
	void SetLinearVelocity(const JPH::Vec3& newVec);
	void SetAngularVelocity(const JPH::Vec3& newVec);

	// Joltの物理座標を、ゲーム側の座標変数に同期させる
	void Sync(Math::Vector3& outPos);

	//ゲッター達
	JPH::Vec3 GetPos()const;
	JPH::Vec3 GetDirection()const;
	const JPH::BodyID& GetBodyID()const { return m_bodyID; }

private:

	//破棄
	void Release();

	//ID
	JPH::BodyID m_bodyID;

	//静的かどうか
	bool        m_isStatic;
};