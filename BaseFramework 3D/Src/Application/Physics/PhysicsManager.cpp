#include "PhysicsManager.h"
#include <Jolt/Core/Memory.h> // ★アロケータの登録に必要なヘッダー

// --- Joltのエラーメッセージを受け取ってコンソールに出力する関数 ---
static void TraceImpl(const char* inFMT, ...)
{
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);
	std::cerr << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS
// --- アサーションが失敗したときに呼び出される関数 ---
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	std::cerr << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;
	return true; // trueを返すとデバッガでブレーク（一時停止）します
}
#endif

void PhysicsManager::Init() {
	
	// 1. Joltの基礎初期化の前に、コールバックを登録する！
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)
	
	// 2.これを new Factory の前に呼ぶ
	JPH::RegisterDefaultAllocator();

	// 3. ファクトリーの作成（アロケータ登録の「後」、型登録の「前」）
	JPH::Factory::sInstance = new JPH::Factory();

	// 4. Joltの型登録
	JPH::RegisterTypes();

	// 5. テンポラリアロケータのメモリ割り当て
	m_tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10MB

	// 6. スレッド数の自動設定
	int num_threads = std::max(1u, std::thread::hardware_concurrency() - 1);
	m_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, num_threads);

	// 7. 物理システムの作成
	m_physicsSystem = new JPH::PhysicsSystem();

	const JPH::uint max_bodies = 1024;
	const JPH::uint num_body_mutexes = 0;
	const JPH::uint max_body_pairs = 1024;
	const JPH::uint max_contact_constraints = 1024;

	// ※前述の通り、本来はここにレイヤーインターフェースを渡して初期化します
	m_physicsSystem->Init(max_bodies, num_body_mutexes, max_body_pairs, max_contact_constraints, m_BPLayerInterface, m_objectVsBroadPhaseLayerFilter, m_objectLayerPairFilter);

	// デフォルトの重力を設定
	m_physicsSystem->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));
}

void PhysicsManager::Update(float deltaTime) {
	if (!m_physicsSystem) return;

	// 物理シミュレーションを1ステップ進める
	// 1フレームあたりの衝突計算ステップ数は通常「1」で十分です
	m_physicsSystem->Update(deltaTime, 1, m_tempAllocator, m_jobSystem);
}

void PhysicsManager::Release() {
	// インスタンスの解放（初期化と逆順に行うのが鉄則です）
	delete m_physicsSystem;
	m_physicsSystem = nullptr;

	delete m_jobSystem;
	m_jobSystem = nullptr;

	delete m_tempAllocator;
	m_tempAllocator = nullptr;

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}