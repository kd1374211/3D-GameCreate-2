#include "PhysicsComponent.h"
#include "../../Physics/PhysicsManager.h"
#include "../../../Framework/Direct3D/KdGLTFLoader.h"

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

bool PhysicsComponent::Init(const std::string& a_path)
{
	// 1. 既存のローダーでモデルを読み込む
	std::shared_ptr<KdGLTFModel> spModel = KdLoadGLTFModel(a_path);
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

	if (allVertices.empty() || allIndices.empty()) {
		return false;
	}

	// 3. Jolt への登録処理（作成済みの InitializeAsMesh を呼ぶ）
	Init(allVertices, allIndices);
	return true;
}

void PhysicsComponent::Init(const std::vector<Math::Vector3>& inVertices, const std::vector<unsigned int>& inIndices)
{
	m_isStatic = true;
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();

	// 1. 頂点リストの作成（x,y,z の分解ループが不要になり超シンプル！）
	JPH::VertexList jphVertices;
	jphVertices.reserve(inVertices.size());
	for (const auto& v : inVertices) {
		jphVertices.push_back(JPH::Float3(v.x, v.y, v.z));
	}

	// 2. インデックスリストの作成
	JPH::IndexedTriangleList jphTriangles;
	jphTriangles.reserve(inIndices.size() / 3);
	for (size_t i = 0; i < inIndices.size(); i += 3) {
		jphTriangles.push_back(JPH::IndexedTriangle(inIndices[i], inIndices[i + 1], inIndices[i + 2]));
	}

	// 3. メッシュ形状（Shape）の構築
	JPH::MeshShapeSettings meshSettings(jphVertices, jphTriangles);
	JPH::Shape::ShapeResult shapeResult = meshSettings.Create();
	JPH::Ref<JPH::Shape> meshShape = shapeResult.Get();

	// 4. 地形（Static）用の設定オブジェクトを作成
	JPH::BodyCreationSettings creationSettings(
		meshShape,
		JPH::RVec3::sZero(),     // 位置（原点 0,0,0）
		JPH::Quat::sIdentity(),  // 回転（回転なし）
		JPH::EMotionType::Static,// 動かないオブジェクト（静的）
		Layers::NON_MOVING      // 地形用レイヤー（衝突判定用）
	);

	// 5. Joltワールドに登録してBodyIDを保存
	// (※地形は動かないため、DontActivateを指定してスリープ状態のまま追加します)
	m_bodyID = bodyInterface.CreateAndAddBody(creationSettings, JPH::EActivation::DontActivate);
}

void PhysicsComponent::AddForce(const Math::Vector3 & inForce)
{
	auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
	JPH::BodyID bodyID = m_bodyID;

	if (!bodyID.IsInvalid() && !m_isStatic) {
		bodyInterface.AddForce(bodyID, JPH::Vec3(inForce.x, inForce.y, inForce.z));
	}
}

void PhysicsComponent::Sync(Math::Vector3& outPos, Math::Quaternion& outRot)
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
		outRot = Math::Quaternion(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
	}
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
