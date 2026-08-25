#pragma once
#include "../../Physics/PhysicsLayer.h"

//初期化内容s
struct PhysicsInitData {
	
	//基本
	Math::Vector3 pos = Math::Vector3::Zero;	//座標
	Math::Quaternion rot = Math::Quaternion::Identity;	//回転
	JPH::EMotionType motionType = JPH::EMotionType::Dynamic;	//動的か静的か
	JPH::EMotionQuality motionQuality = JPH::EMotionQuality::Discrete;	//モーションクオリティ
	JPH::ObjectLayer layer = Layers::NUM_LAYERS;	//レイヤー
	bool isStatic = false;					//静的かどうか
	bool isSensor = false;					//センサーのみかどうか（当たり判定があるか）

	//物理
	float mass = 1.0f;							//質量
	float friction = 0.5f;						//摩擦力
	float restitution = 0.5f;					//反発力

	//減衰
	float linearDamping = 0.1f;					//線形抵抗
	float angularDamping = 0.1f;				//角度抵抗

	//ユーザーデータ
	uint64_t userData = 0;						//ユーザーデータ
};

class PhysicsComponent {
public:

	PhysicsComponent() : m_bodyID(), m_isStatic(false) {}
	~PhysicsComponent() { Release(); }

	//新Init
	bool Init(const std::string& path, PhysicsInitData initData);
	void Init(float radius, PhysicsInitData initData);
	void Init(const Math::Vector3& a_halfExtents, const PhysicsInitData& a_initData);

	//ステータスの設定s
	void SetFriction(float friction);		//摩擦力
	void SetRestitution(float restitution);		//反発力
	void SetDamping(float linearDamp, float angularDamp);		//抵抗
	void SetPosition(const JPH::RVec3& pos);	//座標

	// 力を加える（転がす）
	void AddImpulse(const JPH::Vec3& inForce);

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
	float GetMass()const;
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