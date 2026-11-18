#include "PhysicsManager.h"

// --- TraceImpl と AssertFailedImpl はそのまま ---
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
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
	std::cerr << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;
	return true;
}
#endif

void PhysicsManager::Init()
{
	// 1〜4. プロセス全体で1回だけ行う Jolt の基礎初期化
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

		JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	// 6. スレッドプールの作成（全ワールドで共有する）
	int num_threads = std::max(1u, std::thread::hardware_concurrency() - 1);
	m_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, num_threads);

	// メインワールドの初期化
	m_mainWorld.Init(false);
}

void PhysicsManager::Update(float deltaTime)
{
	// メインワールドを1ステップ進める
	m_mainWorld.Update(deltaTime, m_jobSystem);
}

void PhysicsManager::Release()
{
	// 先にメインワールド（PhysicsSystem / TempAllocator）を解放
	m_mainWorld.Release();

	// 次に共有の JobSystem と Factory を解放（初期化の逆順）
	delete m_jobSystem;
	m_jobSystem = nullptr;

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}