#include "PhysicsComponent.h"
#include "../../Physics/PhysicsManager.h"
#include "../../../Framework/Direct3D/KdGLTFLoader.h"

bool PhysicsComponent::Init(const std::string& path, bool isStatic)
{
	//用意
	m_isStatic = isStatic;
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	//モデル読み込み
	std::shared_ptr<KdGLTFModel> spModel = KdLoadGLTFModel(path);
	if (!spModel) {
		return false; // ロード失敗
	}

	std::vector<Math::Vector3> allVertices;
	std::vector<unsigned int>  allIndices;

	// 2. モデル内の全ノードから頂点・インデックスを抽出
	for (const auto& node : spModel->Nodes) {
		if (!node.IsMesh) continue;

		const auto& mesh = node.Mesh;
		unsigned int vertexOffset = static_cast<unsigned int>(allVertices.size());

		// 頂点座標の読み込み
		for (const auto& vertex : mesh.Vertices) {
			// ※ KdMeshVertex 内の位置座標メンバ名（Pos や Position 等）を指定
			Math::Vector3 pos = vertex.Pos;

			// 必要に応じてノードのワールド行列で座標変換
			// pos = Math::Vector3::Transform(pos, node.WorldTransform);

			allVertices.push_back(pos);
		}

		// 面（インデックス）の読み込み
		for (const auto& face : mesh.Faces) {
			// ※ KdMeshFace のインデックス保持用メンバに合わせて調整してください
			allIndices.push_back(vertexOffset + face.Idx[0]);
			allIndices.push_back(vertexOffset + face.Idx[1]);
			allIndices.push_back(vertexOffset + face.Idx[2]);
		}
	}

	//無かったらリターン
	if (allVertices.empty() || allIndices.empty()) {
		return false;
	}

	// 1. 頂点リストの作成（x,y,z の分解ループが不要になり超シンプル！）
	JPH::VertexList jphVertices;
	jphVertices.reserve(allVertices.size());
	for (const auto& v : allVertices) {
		jphVertices.push_back(JPH::Float3(v.x, v.y, v.z));
	}

	// 2. インデックスリストの作成
	JPH::IndexedTriangleList jphTriangles;
	jphTriangles.reserve(allIndices.size() / 3);
	for (size_t i = 0; i < allIndices.size(); i += 3) {
		jphTriangles.push_back(JPH::IndexedTriangle(allIndices[i], allIndices[i + 1], allIndices[i + 2]));
	}

	JPH::ShapeRefC finalShape;

	//生成する形状の種分け
	if (isStatic)
	{
		// 【静的オブジェクト】三角形メッシュ（MeshShape）として作成
		JPH::MeshShapeSettings meshSettings(jphVertices, jphTriangles);
		finalShape = meshSettings.Create().Get();
	}
	else
	{
		// 凸包（ConvexHull）用に JPH::Vec3 の vector を用意！
		std::vector<JPH::Vec3> convexVertices;
		convexVertices.reserve(allVertices.size()); // ※お手元のモデルの頂点配列を指定

		for (const auto& v : allVertices) {
			// あなたのモデルの頂点座標（x, y, z）を JPH::Vec3 に変換
			convexVertices.push_back(JPH::Vec3(v.x, v.y, v.z));
		}
		// 【動的オブジェクト】頂点群から「凸包（ConvexHull）」を自動計算して作成！
		JPH::ConvexHullShapeSettings convexSettings(convexVertices.data(), static_cast<int>(convexVertices.size()));
		finalShape = convexSettings.Create().Get();
	}

	// 3. Body を作成して登録
	JPH::EMotionType motionType = isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;
	JPH::ObjectLayer layer = isStatic ? Layers::NON_MOVING : Layers::MOVING;

	JPH::BodyCreationSettings creationSettings(
		finalShape,
		JPH::Vec3::sZero(),
		JPH::Quat::sIdentity(),
		motionType,
		layer
	);

	// 3. Joltワールドに登録してIDを保存
	m_bodyID = bodyInterface.CreateAndAddBody(creationSettings, JPH::EActivation::Activate);

	return true;
}

void PhysicsComponent::Init(const Math::Vector3& inPos, float inRadius, bool inIsStatic)
{
	m_isStatic = inIsStatic;
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 1. コライダー（球体）の作成
	JPH::SphereShapeSettings sphereSettings(inRadius);

	JPH::Shape::ShapeResult shapeResult = sphereSettings.Create();
	JPH::Ref<JPH::Shape> ballShape = shapeResult.Get();

	// 2. 設定オブジェクトの構築
	JPH::Vec3 initPos(inPos.x, inPos.y, inPos.z);
	JPH::EMotionType motionType = inIsStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic;

	// ※レイヤーの定義（MOVING等）はお使いの設定に合わせてください
	JPH::ObjectLayer layer = inIsStatic ? Layers::NON_MOVING : Layers::MOVING;

	JPH::BodyCreationSettings creationSettings(
		ballShape,
		initPos,
		JPH::Quat::sIdentity(),
		motionType,
		layer
	);

	creationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	creationSettings.mMassPropertiesOverride.mMass = 1.0f; // ★ ここで質量を指定（1.0kg）

	// 3. Joltワールドに登録してIDを保存
	m_bodyID = bodyInterface.CreateAndAddBody(creationSettings, JPH::EActivation::Activate);
}

void PhysicsComponent::SetFriction(float friction)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid()) {
		bodyInterface.SetFriction(bodyID, friction);
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

void PhysicsComponent::AddForce(const Math::Vector3 & inForce)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid() && !m_isStatic) {
		bodyInterface.AddForce(bodyID, JPH::Vec3(inForce.x, inForce.y, inForce.z));
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

		// 呼び出し元の変数（プレイヤーの座標）に格納
		outPos = Math::Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
		// outRot = Math::Quaternion(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
	}
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
