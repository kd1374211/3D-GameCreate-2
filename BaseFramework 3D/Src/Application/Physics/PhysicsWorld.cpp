#include "PhysicsWorld.h"

void PhysicsWorld::Init(bool isSimulationOnly)
{
	// 5. テンポラリアロケータのメモリ割り当て（裏ワールドなら少し控えめでも可）
	m_tempAllocator = new JPH::TempAllocatorImpl(isSimulationOnly ? 4 * 1024 * 1024 : 10 * 1024 * 1024);

	// 7. 物理システムの作成
	m_physicsSystem = new JPH::PhysicsSystem();

	const JPH::uint max_bodies = 1024;
	const JPH::uint num_body_mutexes = 0;
	const JPH::uint max_body_pairs = 1024;
	const JPH::uint max_contact_constraints = 1024;

	m_physicsSystem->Init(
		max_bodies,
		num_body_mutexes,
		max_body_pairs,
		max_contact_constraints,
		m_BPLayerInterface,
		m_objectVsBroadPhaseLayerFilter,
		m_objectLayerPairFilter
	);

	// デフォルトの重力を設定
	m_physicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

	// メインワールドの場合のみ、通常のゲーム用リスナー（SEやエフェクト等）を登録
	if (!isSimulationOnly)
	{
		m_physicsSystem->SetContactListener(&m_contactListener);
	}

	// 各種設定
	JPH::PhysicsSettings settings = m_physicsSystem->GetPhysicsSettings();
	settings.mMinVelocityForRestitution = 0.25f;
	m_physicsSystem->SetPhysicsSettings(settings);
}

void PhysicsWorld::Update(float deltaTime, JPH::JobSystem* jobSystem)
{
	if (!m_physicsSystem) return;
	m_physicsSystem->Update(deltaTime, 1, m_tempAllocator, jobSystem);
}

void PhysicsWorld::Release()
{
	delete m_physicsSystem;
	m_physicsSystem = nullptr;

	delete m_tempAllocator;
	m_tempAllocator = nullptr;
}