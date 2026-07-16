#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

// --- ① レイヤーの定義 ---
namespace Layers
{
	// オブジェクトレイヤー（細かな分類）
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;

	// ブロードフェーズレイヤー（大雑把な分類）
	static constexpr JPH::BroadPhaseLayer BP_NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer BP_MOVING(1);
	static constexpr JPH::uint BP_NUM_LAYERS = 2;
};

// --- ② オブジェクトレイヤー同士の衝突判定フィルター ---
// 「プレイヤー同士は当たるか？」「床と床は当たるか？」などを判定します。
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // 動かないものは、動くものとだけ衝突する（床同士は計算しない）
		case Layers::MOVING:
			return true; // 動くものは、何とでも衝突する
		default:
			return false;
		}
	}
};

// --- ③ オブジェクトレイヤーとブロードフェーズの仲介役 ---
// 「このオブジェクトレイヤーは、どっちの大雑把グループに属するか」をJoltに教えます。
class BPLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// マッピングの登録
		mObjectToBroadPhase[Layers::NON_MOVING] = Layers::BP_NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = Layers::BP_MOVING;
	}

	virtual JPH::uint GetNumBroadPhaseLayers() const override
	{
		return Layers::BP_NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)Layers::BP_NON_MOVING:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)Layers::BP_MOVING:		return "MOVING";
		default:                                                return "INVALID";
		}
	}
#endif

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// --- ④ ブロードフェーズレイヤー同士の衝突判定フィルター ---
// 高速化のため、大雑把グループ同士でそもそも衝突する可能性があるかを判定します。
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == Layers::BP_MOVING; // 動かないグループは、動くグループとだけ衝突する
		case Layers::MOVING:
			return true; // 動くグループは、何とでも衝突する
		default:
			return false;
		}
	}
};