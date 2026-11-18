#pragma once
#include "PhysicsWorld.h"

class PhysicsManager
{
public:

	void Init();
	void Update(float deltaTime);
	void Release();

	// 既存コードとの完全互換アクセサ（すべてメインワールドを返す）
	JPH::PhysicsSystem& GetSystem() { return m_mainWorld.GetSystem(); }
	JPH::BodyInterface& GetBodyInterface() { return m_mainWorld.GetBodyInterface(); }
	PhysicsWorld& GetMainWorld() { return m_mainWorld; }

	// 裏シミュレーション用の独立したワールドを生成して返す
	std::unique_ptr<PhysicsWorld> CreateSimulationWorld()
	{
		auto simWorld = std::make_unique<PhysicsWorld>();
		simWorld->Init(true); // isSimulationOnly = true で初期化
		return simWorld;
	}

	// 裏ワールドで Update を回す際に渡す共有 JobSystem
	JPH::JobSystem* GetJobSystem() { return m_jobSystem; }

private:
	PhysicsManager() = default;
	~PhysicsManager() = default;

	JPH::JobSystemThreadPool* m_jobSystem = nullptr;
	PhysicsWorld              m_mainWorld; // 画面に映るメインの物理空間

public:

	static PhysicsManager& Instance()
	{
		static PhysicsManager instance;
		return instance;
	}
};

#define PHYSICSMGR PhysicsManager::Instance()