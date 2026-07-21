#pragma once

class PhysicsComponent {
public:

	PhysicsComponent() : m_bodyID(), m_isStatic(false) {}
	~PhysicsComponent() { Release(); }

	// 物理オブジェクトとして初期化する（球体）
	void Init(const Math::Vector3& inPos, float inRadius, bool inIsStatic);
	void Init(const std::vector<Math::Vector3>& inVertices, const std::vector<unsigned int>& inIndices);

	// 物理オブジェクトとして初期化する（モデル）
	bool Init(const std::string& a_path);

	// 力を加える（転がす）
	void AddForce(const Math::Vector3& inForce);

	// Joltの物理座標を、ゲーム側の座標変数に同期させる
	void Sync(Math::Vector3& outPos, Math::Quaternion& outRot);

private:

	//破棄
	void Release();

	//ID
	JPH::BodyID m_bodyID;

	//静的かどうか
	bool        m_isStatic;
};