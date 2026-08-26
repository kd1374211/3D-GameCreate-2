#include "PhysicsComponent.h"
#include "../../Physics/PhysicsManager.h"
#include "../../../Framework/Direct3D/KdGLTFLoader.h"

// 1. モデル版 Init
bool PhysicsComponent::Init(const std::string& path, PhysicsInitData initData)
{
	m_isStatic = initData.isStatic;
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	std::shared_ptr<KdGLTFModel> spModel = KdLoadGLTFModel(path);
	if (!spModel) return false;

	std::vector<Math::Vector3> allVertices;
	std::vector<unsigned int>  allIndices;

	for (const auto& node : spModel->Nodes) {
		if (!node.IsMesh) continue;
		const auto& mesh = node.Mesh;
		unsigned int vertexOffset = static_cast<unsigned int>(allVertices.size());

		for (const auto& vertex : mesh.Vertices) {
			allVertices.push_back(vertex.Pos);
		}
		for (const auto& face : mesh.Faces) {
			allIndices.push_back(vertexOffset + face.Idx[0]);
			allIndices.push_back(vertexOffset + face.Idx[1]);
			allIndices.push_back(vertexOffset + face.Idx[2]);
		}
	}

	if (allVertices.empty() || allIndices.empty()) return false;

	JPH::VertexList jphVertices;
	jphVertices.reserve(allVertices.size());
	for (const auto& v : allVertices) {
		jphVertices.push_back(JPH::Float3(v.x, v.y, v.z));
	}

	JPH::IndexedTriangleList jphTriangles;
	jphTriangles.reserve(allIndices.size() / 3);
	for (size_t i = 0; i < allIndices.size(); i += 3) {
		jphTriangles.push_back(JPH::IndexedTriangle(allIndices[i], allIndices[i + 1], allIndices[i + 2]));
	}

	JPH::ShapeRefC finalShape;
	if (m_isStatic)
	{
		JPH::MeshShapeSettings meshSettings(jphVertices, jphTriangles);
		finalShape = meshSettings.Create().Get();
	}
	else
	{
		std::vector<JPH::Vec3> convexVertices;
		convexVertices.reserve(allVertices.size());
		for (const auto& v : allVertices) {
			convexVertices.push_back(JPH::Vec3(v.x, v.y, v.z));
		}
		JPH::ConvexHullShapeSettings convexSettings(convexVertices.data(), static_cast<int>(convexVertices.size()));
		finalShape = convexSettings.Create().Get();
	}

	JPH::BodyCreationSettings creationSettings(
		finalShape,
		JPH::RVec3(initData.pos.x, initData.pos.y, initData.pos.z),
		JPH::Quat(initData.rot.x, initData.rot.y, initData.rot.z, initData.rot.w),
		initData.motionType,
		initData.layer
	);

	creationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	creationSettings.mMassPropertiesOverride.mMass = initData.mass;
	creationSettings.mMotionQuality = initData.motionQuality;
	creationSettings.mUserData = initData.userData;
	creationSettings.mIsSensor = initData.isSensor;
	creationSettings.mFriction = initData.friction;
	creationSettings.mRestitution = initData.restitution;
	creationSettings.mLinearDamping = initData.linearDamping;
	creationSettings.mAngularDamping = initData.angularDamping;

	m_bodyID = bodyInterface.CreateAndAddBody(
		creationSettings,
		initData.isStatic ? JPH::EActivation::DontActivate : JPH::EActivation::Activate
	);

	return true;
}

// 2. 球体版 Init
void PhysicsComponent::Init(float radius, PhysicsInitData initData)
{
	m_isStatic = initData.isStatic;
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	JPH::SphereShapeSettings sphereSettings(radius);
	JPH::ShapeRefC ballShape = sphereSettings.Create().Get();

	JPH::BodyCreationSettings creationSettings(
		ballShape,
		JPH::RVec3(initData.pos.x, initData.pos.y, initData.pos.z),
		JPH::Quat(initData.rot.x, initData.rot.y, initData.rot.z, initData.rot.w),
		initData.motionType,
		initData.layer
	);

	creationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	creationSettings.mMassPropertiesOverride.mMass = initData.mass;
	creationSettings.mMotionQuality = initData.motionQuality;
	creationSettings.mUserData = initData.userData;
	creationSettings.mIsSensor = initData.isSensor;
	creationSettings.mFriction = initData.friction;
	creationSettings.mRestitution = initData.restitution;
	creationSettings.mLinearDamping = initData.linearDamping;
	creationSettings.mAngularDamping = initData.angularDamping;

	m_bodyID = bodyInterface.CreateAndAddBody(
		creationSettings,
		initData.isStatic ? JPH::EActivation::DontActivate : JPH::EActivation::Activate
	);
}

// 3. 箱型 Init（修正済み）
void PhysicsComponent::Init(const Math::Vector3& halfExtents, const PhysicsInitData& initData)
{
	m_isStatic = initData.isStatic;

	JPH::BoxShapeSettings boxSettings(JPH::Vec3(halfExtents.x, halfExtents.y, halfExtents.z));
	JPH::ShapeRefC shape = boxSettings.Create().Get();

	JPH::BodyCreationSettings bodySettings(
		shape,
		JPH::RVec3(initData.pos.x, initData.pos.y, initData.pos.z),
		JPH::Quat(initData.rot.x, initData.rot.y, initData.rot.z, initData.rot.w),
		initData.motionType,
		initData.layer
	);

	bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	bodySettings.mMassPropertiesOverride.mMass = initData.mass;
	bodySettings.mMotionQuality = initData.motionQuality;
	bodySettings.mFriction = initData.friction;
	bodySettings.mRestitution = initData.restitution;
	bodySettings.mLinearDamping = initData.linearDamping;
	bodySettings.mAngularDamping = initData.angularDamping;
	bodySettings.mUserData = initData.userData;
	bodySettings.mIsSensor = initData.isSensor; // ★正しく反映されるよう修正

	JPH::BodyInterface& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// ★設定済みの bodySettings をそのまま渡す
	m_bodyID = bodyInterface.CreateAndAddBody(
		bodySettings,
		initData.isStatic ? JPH::EActivation::DontActivate : JPH::EActivation::Activate
	);
}

void PhysicsComponent::SetFriction(float friction)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid()) {
		bodyInterface.SetFriction(bodyID, friction);
	}
}

void PhysicsComponent::SetRestitution(float restitution)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid()) {
		bodyInterface.SetRestitution(bodyID, restitution);
	}
}

void PhysicsComponent::SetDamping(float linearDamp, float angularDamp)
{
	//動かないものには設定しない
	if (m_isStatic)return;

	JPH::BodyID bodyID = m_bodyID;

	// 💡 m_physicsSystem は JPH::PhysicsSystem のポインタ
	JPH::BodyLockWrite lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), bodyID);

	if (lock.Succeeded()) {
		JPH::Body& body = lock.GetBody();

		// 静的(Static)ボディ以外なら MotionProperties を取得して変更可能
		if (auto* motionProps = body.GetMotionProperties()) {
			motionProps->SetLinearDamping(linearDamp);
			motionProps->SetAngularDamping(angularDamp); // ★回転の減衰を設定！
		}
	}
}

void PhysicsComponent::SetPosition(const JPH::RVec3& pos)
{
	if (m_bodyID.IsInvalid()) return;

	// Jolt Physics の BodyInterface を取得して座標を書き換える
	// (EActivation::Activate を渡すことで、休止状態の物理ボディも起こす)
	PHYSICSMGR.GetSystem().GetBodyInterface().SetPosition(
		m_bodyID,
		pos,
		JPH::EActivation::Activate
	);
}

void PhysicsComponent::AddImpulse(const JPH::Vec3& inForce)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid() && !m_isStatic) {
		bodyInterface.AddImpulse(bodyID, inForce);
	}
}

void PhysicsComponent::SetSpeed(const float& newSpeed)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 1. 現在の速度ベクトルを取得
	JPH::Vec3 currentVel = bodyInterface.GetLinearVelocity(m_bodyID);

	// 速度がほぼゼロ（停止中）の場合は向きが計算できないので処理をスキップ
	if (currentVel.LengthSq() < 0.0001f) {
		return;
	}

	// 2. 正規化（長さ1の「向きベクトル」を取得）して、新しいスピードを掛ける
	JPH::Vec3 newVel = currentVel.Normalized() * newSpeed;

	// 3. 速度を再設定
	bodyInterface.SetLinearVelocity(m_bodyID, newVel);
}

void PhysicsComponent::SetDirection(const Math::Vector3& newDir)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 1. 現在の速度ベクトルから「現在のスピード（長さ）」を取得
	JPH::Vec3 currentVel = bodyInterface.GetLinearVelocity(m_bodyID);
	float currentSpeed = currentVel.Length();

	// 2. 新しい方向ベクトルの正規化（長さを1にする）
	JPH::Vec3 joltDir(newDir.x, newDir.y, newDir.z);
	if (joltDir.LengthSq() < 0.0001f) {
		return; // 入力方向がゼロなら何もしない
	}
	JPH::Vec3 normalizedDir = joltDir.Normalized();

	// 3. 「新しい向き」 × 「元のスピード」 で新しい速度ベクトルを作る
	JPH::Vec3 newVel = normalizedDir * currentSpeed;

	// 4. 速度を再設定
	bodyInterface.SetLinearVelocity(m_bodyID, newVel);
}

void PhysicsComponent::SetLinearVelocity(const JPH::Vec3& newVel)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 設定
	bodyInterface.SetLinearVelocity(m_bodyID, newVel);
}

void PhysicsComponent::SetAngularVelocity(const JPH::Vec3& newVec)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 設定
	bodyInterface.SetAngularVelocity(m_bodyID, newVec);
}

void PhysicsComponent::Sync(Math::Vector3& outPos)
{
	if (m_isStatic) return; // 静的なものは同期不要

	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid() && bodyInterface.IsAdded(bodyID)) {
		// Joltから値を取得
		JPH::RVec3 pos = bodyInterface.GetPosition(bodyID);
		JPH::Quat rot = bodyInterface.GetRotation(bodyID);

		// 呼び出し元の変数（座標）に格納
		outPos = Math::Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
		// outRot = Math::Quaternion(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
	}
}

float PhysicsComponent::GetMass() const
{
	if (m_bodyID.IsInvalid()) return 1.0f;

	JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), m_bodyID);
	if (lock.Succeeded())
	{
		const JPH::Body& body = lock.GetBody();
		// Dynamic ボディの場合は MotionProperties から逆質量を取得
		if (const auto* motionProps = body.GetMotionProperties())
		{
			float invMass = motionProps->GetInverseMass();
			if (invMass > 0.0f) return 1.0f / invMass; // 逆数の逆数で質量を返す
		}
	}
	return 1.0f; // デフォルト値
}

JPH::Vec3 PhysicsComponent::GetPos() const
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	return bodyInterface.GetPosition(m_bodyID);
}

JPH::Vec3 PhysicsComponent::GetDirection() const
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	return bodyInterface.GetLinearVelocity(m_bodyID);
}

void PhysicsComponent::Release()
{
	if (!m_bodyID.IsInvalid()) {
		// PhysicsManagerやPhysicsSystemからBodyInterfaceを取得
		auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

		// Joltのワールドから取り除き、メモリを解放する
		bodyInterface.RemoveBody(m_bodyID);
		bodyInterface.DestroyBody(m_bodyID);

		m_bodyID = JPH::BodyID(); // IDを無効化
	}
}
