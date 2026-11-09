#pragma once
#include "../../Physics/PhysicsLayer.h"

//初期化内容s
struct PhysicsInitData {
	
	//基本
	Math::Vector3 pos = Math::Vector3::Zero;	//座標
	Math::Quaternion rot = Math::Quaternion::Identity;	// 回転
	Math::Vector3 scale = Math::Vector3::One;			// 拡縮
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

class TerrainPhysicsMaterial : public JPH::PhysicsMaterialSimple
{
public:
	TerrainPhysicsMaterial(const std::string& inName, JPH::ColorArg inColor, float inRestitution, float inFriction, float inRollingResitance)
		: JPH::PhysicsMaterialSimple(inName, inColor)
		, m_restitution(inRestitution)
		, m_friction(inFriction)
		, m_rollingResistance(inRollingResitance)
	{}

	float GetRestitution() const { return m_restitution; }
	float GetFriction() const { return m_friction; }
	float GetRollingResistance()const { return m_rollingResistance; }

private:
	float m_restitution = 0.0f;
	float m_friction = 0.2f;
	float m_rollingResistance = 0.0f;
};

class PhysicsComponent {
public:

	PhysicsComponent() : m_bodyID(), m_isStatic(false) {}
	~PhysicsComponent() { Release(); }

	// Init
	bool InitTerrainModel(const std::string& path, PhysicsInitData initData);				// マテリアル付きのモデルInit
	bool Init(const std::string& path, PhysicsInitData initData);							// マテリアル無しのモデルInit
	void Init(float radius, PhysicsInitData initData);										// 球体型Init
	void Init(const Math::Vector3& a_halfExtents, const PhysicsInitData& a_initData);		// 箱型Init

	// ステータスの設定
	void SetFriction(float friction);														//摩擦力
	void SetRestitution(float restitution);													//反発力
	void SetDamping(float linearDamp, float angularDamp);									//抵抗
	void SetPosition(const JPH::RVec3& pos);												//座標
	void SetRotation(const JPH::Quat& rotation);											//回転

	// 物理な処理
	void AddImpulse(const JPH::Vec3& inForce);												// 力を加える（転がす）
	//void SetSpeed(const float& newSpeed);													// 方向を維持して速度を変更する
	//void SetDirection(const Math::Vector3& newDir);										// 速度を維持して方向を変更する
	void SetLinearVelocity(const JPH::Vec3& newVec);										// 線形速度を変更する
	void SetAngularVelocity(const JPH::Vec3& newVec);										// 角速度を変更する

	// Jolt世界の処理
	void ActivateBody();																	// 活性化
	void DeactivateBody();																	// 非活性化
	// 当たり判定ごと操作したい時
	void RemoveBody()const;																	// 世界から削除
	void AddBody()const;																	// 世界に再追加
	// 同期処理
	void Sync(Math::Vector3& outPos, Math::Quaternion& outRot);								// Jolt世界の物理・回転を、ゲーム側に同期させる

	//ゲッター達
	float GetMass()const;																	// 重量
	JPH::Vec3 GetPos()const;																// 座標
	JPH::Quat GetRotation()const;															// 回転
	JPH::Vec3 GetLinearVelocity()const;														// 線形速度
	JPH::Vec3 GetAngularVelocity()const;													// 角速度
	const JPH::BodyID& GetBodyID()const { return m_bodyID; }								// 物理BodyID

private:

	//破棄
	void Release();

	//ID
	JPH::BodyID m_bodyID;

	//静的かどうか
	bool        m_isStatic;
};