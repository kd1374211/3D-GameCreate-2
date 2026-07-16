#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "PhysicsLayer.h"

class PhysicsManager {
public:

	// 物理エンジンの初期化
	void Init();

	// 毎フレームの更新処理
	void Update(float deltaTime);

	// 後片付け
	void Release();

	// 他のクラスから物理ワールドを操作するためのゲッター
	JPH::PhysicsSystem& GetSystem() { return *m_physicsSystem; }
	JPH::BodyInterface& GetBodyInterface() { return m_physicsSystem->GetBodyInterface(); }

private:

	PhysicsManager() {}
	~PhysicsManager() {}

	// コピー禁止
	PhysicsManager(const PhysicsManager&) = delete;
	PhysicsManager& operator=(const PhysicsManager&) = delete;

	// Joltの動作に必要なメンバー変数群
	JPH::TempAllocatorImpl* m_tempAllocator = nullptr;
	JPH::JobSystemThreadPool* m_jobSystem = nullptr;
	JPH::PhysicsSystem* m_physicsSystem = nullptr;

	//レイヤー
	BPLayerInterfaceImpl m_BPLayerInterface;
	ObjectLayerPairFilterImpl m_objectLayerPairFilter;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;

public:

	// シングルトンパターン、もしくはゲームエンジンクラス内で保持する設計にします
	static PhysicsManager& Instance() {
		static PhysicsManager instance;
		return instance;
	}

};

#define PHYSICSMGR PhysicsManager::Instance()