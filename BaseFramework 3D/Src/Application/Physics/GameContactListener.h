#pragma once

class GameContactListener : public JPH::ContactListener
{
public:

	// 2つの物体が接触した瞬間に Jolt から自動で呼ばれる
	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
	
	// 2つの物体が接触し続けている間に Jolt から自動で呼ばれる
	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

private:

	const JPH::PhysicsMaterial* GetMaterialRecursively(const JPH::Shape* inShape, const JPH::SubShapeID& inSubShapeID)
	{
		if (!inShape || inSubShapeID.IsEmpty()) return nullptr; // ★ ガード追加

		// 1. ScaledShape や RotatedTranslatedShape のアンラップ
		JPH::SubShapeID remainderID;
		const JPH::Shape* leafShape = inShape->GetLeafShape(inSubShapeID, remainderID);
		if (!leafShape) return nullptr;

		// 2. CompoundShape（複合形状）の場合、子 Shape をたどる
		if (leafShape->GetType() == JPH::EShapeType::Compound)
		{
			const auto* compoundShape = static_cast<const JPH::CompoundShape*>(leafShape);

			// SubShapeID から子 Shape のインデックスを取得
			JPH::SubShapeID subRemainder;
			uint32_t subShapeIndex = compoundShape->GetSubShapeIndexFromID(remainderID, subRemainder);

			if (subShapeIndex < compoundShape->GetNumSubShapes())
			{
				const JPH::Shape* childShape = compoundShape->GetSubShape(subShapeIndex).mShape;
				return GetMaterialRecursively(childShape, subRemainder); // 再帰で子形状へ
			}
			return nullptr;
		}

		// 3. MeshShape または通常の Shape からマテリアルを取得
		return leafShape->GetMaterial(remainderID);
	}

};