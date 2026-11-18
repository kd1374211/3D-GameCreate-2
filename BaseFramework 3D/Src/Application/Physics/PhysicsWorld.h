#pragma once
#include "PhysicsLayer.h"
#include "GameContactListener.h"

class PhysicsWorld
{
public:
	PhysicsWorld() = default;
	~PhysicsWorld() { Release(); }

	// isSimulationOnly: true にすると裏シミュレーション用の軽量設定（SE・エフェクト用のメインリスナーを使わない等）にできる
	void Init(bool isSimulationOnly = false);
	void Update(float deltaTime, JPH::JobSystem* jobSystem);
	void Release();

	JPH::PhysicsSystem& GetSystem() { return *m_physicsSystem; }
	JPH::BodyInterface& GetBodyInterface() { return m_physicsSystem->GetBodyInterface(); }

	// 任意のリスナーに差し替えたい場合用（シミュレーション専用の接触検知など）
	void SetContactListener(JPH::ContactListener* listener)
	{
		if (m_physicsSystem) m_physicsSystem->SetContactListener(listener);
	}

private:
	JPH::TempAllocatorImpl* m_tempAllocator = nullptr;
	JPH::PhysicsSystem* m_physicsSystem = nullptr;

	// 既存のレイヤーインターフェース・フィルターを各ワールドごとに保持
	BPLayerInterfaceImpl              m_BPLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         m_objectLayerPairFilter;

	// メインワールド用のコンタクトリスナー
	GameContactListener               m_contactListener;
};