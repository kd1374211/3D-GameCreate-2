#include "GameContactListener.h"
#include "PhysicsLayer.h"

//オブジェクトのインクルード
#include "../GameObject/BowlingPin/PinBase/PinBase.h"
#include "../GameObject/Chara/BowlingBall/BowlingBall.h"

void GameContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	// 1. 衝突した2つのレイヤーを取得
	JPH::ObjectLayer layer1 = inBody1.GetObjectLayer();
	JPH::ObjectLayer layer2 = inBody2.GetObjectLayer();

	// ボディーと SubShapeID を取得
	const JPH::Body* body1 = &inBody1;
	const JPH::Body* body2 = &inBody2;
	JPH::SubShapeID subShapeID1 = inManifold.mSubShapeID1;
	JPH::SubShapeID subShapeID2 = inManifold.mSubShapeID2;

	// 衝突法線（初期値は Body1 -> Body2）
	JPH::Vec3 normal = inManifold.mWorldSpaceNormal;

	// 番号が小さいレイヤーを基準にする
	if (layer1 > layer2)
	{
		std::swap(layer1, layer2);
		std::swap(body1, body2);
		std::swap(subShapeID1, subShapeID2); // ★ SubShapeID も入れ替えと連動させる

		// 法線反転
		normal = -normal;
	}

	// 固定された body1 / body2 から事前キャスト
	auto* gameObj1 = reinterpret_cast<KdGameObject*>(body1->GetUserData());
	auto* gameObj2 = reinterpret_cast<KdGameObject*>(body2->GetUserData());

	// =================================================================
	// Terrain & BowlingBall 判定部分（地形確定後に実行）
	// =================================================================
	if (layer1 == Layers::TERRAIN && layer2 == Layers::BOWLINGBALL)
	{
		// 地形（body1）と確定したため、swap対応済みの subShapeID1 から安全にマテリアルを取得
		const JPH::PhysicsMaterial* hitMaterial = GetMaterialRecursively(body1->GetShape(), subShapeID1);

		if (hitMaterial)
		{
			// カスタムマテリアルにキャストして物理係数（反発・摩擦）を上書き設定
			if (auto* terrainMat = dynamic_cast<const TerrainPhysicsMaterial*>(hitMaterial))
			{
				ioSettings.mCombinedRestitution = terrainMat->GetRestitution();
				ioSettings.mCombinedFriction = terrainMat->GetFriction();
			}
		}
	}

	// Finish & Player
	if (layer1 == Layers::FINISHAREA && layer2 == Layers::BOWLINGBALL)
	{
		if (auto ball = dynamic_cast<BowlingBall*>(gameObj2))
		{
			ball->HitFinishArea();
		}
	}

	// Player & Pin
	if (layer1 == Layers::BOWLINGBALL && layer2 == Layers::BOWLINGPIN)
	{
		// UserData から ピンオブジェクトのポインタを復元
		if (auto* pin = dynamic_cast<PinBase*>(gameObj2))
		{
			// プレイヤーの現在速度を取得してピンに通知！
			JPH::Vec3 playerVel = body1->GetLinearVelocity();
			pin->OnHitByPlayer(playerVel);
		}

		// UserData から プレイヤーオブジェクトのポインタを復元
		if (auto* ball = dynamic_cast<BowlingBall*>(gameObj1))
		{
			// ピンに当たったことを通知
			ball->OnHitPin();
		}
	}

	// Pin & Pin
	if (layer1 == Layers::BOWLINGPIN && layer2 == Layers::BOWLINGPIN)
	{
		// UserData から ピンオブジェクトのポインタを復元
		auto* pin = dynamic_cast<PinBase*>(gameObj1);
		auto* pin2 = dynamic_cast<PinBase*>(gameObj2);

		// どちらもあることを確認
		if (pin && pin2)
		{
			// ピン１が既に当てられているならピン２の吹っ飛びを呼ぶ
			if (pin->GetIsHit())
			{
				// プレイヤーの現在速度を取得してピンに通知！
				JPH::Vec3 pinVel = body1->GetLinearVelocity();
				pin2->OnHitByPin(pinVel);
			}

			// ピン２が既に当てられているならピン１の吹っ飛びを呼ぶ
			if (pin2->GetIsHit())
			{
				// プレイヤーの現在速度を取得してピンに通知！
				JPH::Vec3 pinVel = body2->GetLinearVelocity();
				pin->OnHitByPin(pinVel);
			}
		}
	}
}

void GameContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	// 1. 衝突した2つのレイヤーを取得
	JPH::ObjectLayer layer1 = inBody1.GetObjectLayer();
	JPH::ObjectLayer layer2 = inBody2.GetObjectLayer();

	// ボディーと SubShapeID を取得
	const JPH::Body* body1 = &inBody1;
	const JPH::Body* body2 = &inBody2;
	JPH::SubShapeID subShapeID1 = inManifold.mSubShapeID1;
	JPH::SubShapeID subShapeID2 = inManifold.mSubShapeID2;

	// 衝突法線（初期値は Body1 -> Body2）
	JPH::Vec3 normal = inManifold.mWorldSpaceNormal;

	// 番号が小さいレイヤーを基準にする
	if (layer1 > layer2)
	{
		std::swap(layer1, layer2);
		std::swap(body1, body2);
		std::swap(subShapeID1, subShapeID2); // ★ SubShapeID も入れ替えと連動させる

		// 法線反転
		normal = -normal;
	}

	// 固定された body1 / body2 から事前キャスト
	auto* gameObj1 = reinterpret_cast<KdGameObject*>(body1->GetUserData());
	auto* gameObj2 = reinterpret_cast<KdGameObject*>(body2->GetUserData());

	// =================================================================
	// Terrain & BowlingBall 判定部分（地形確定後に実行）
	// =================================================================
	if (layer1 == Layers::TERRAIN && layer2 == Layers::BOWLINGBALL)
	{
		// 地形（body1）と確定したため、swap対応済みの subShapeID1 から安全にマテリアルを取得
		const JPH::PhysicsMaterial* hitMaterial = GetMaterialRecursively(body1->GetShape(), subShapeID1);

		if (hitMaterial)
		{
			// カスタムマテリアルにキャストして物理係数（反発・摩擦）を上書き設定
			if (auto* terrainMat = dynamic_cast<const TerrainPhysicsMaterial*>(hitMaterial))
			{
				ioSettings.mCombinedRestitution = terrainMat->GetRestitution();
				ioSettings.mCombinedFriction = terrainMat->GetFriction();
			}
		}
	}

	// Finish & Player
	if (layer1 == Layers::FINISHAREA && layer2 == Layers::BOWLINGBALL)
	{
		if (auto ball = dynamic_cast<BowlingBall*>(gameObj2))
		{
			ball->HitFinishArea();
		}
	}

	// Player & Pin
	if (layer1 == Layers::BOWLINGBALL && layer2 == Layers::BOWLINGPIN)
	{
		// UserData から ピンオブジェクトのポインタを復元
		if (auto* pin = dynamic_cast<PinBase*>(gameObj2))
		{
			// プレイヤーの現在速度を取得してピンに通知！
			JPH::Vec3 playerVel = body1->GetLinearVelocity();
			pin->OnHitByPlayer(playerVel);
		}

		// UserData から プレイヤーオブジェクトのポインタを復元
		if (auto* ball = dynamic_cast<BowlingBall*>(gameObj1))
		{
			// ピンに当たったことを通知
			ball->OnHitPin();
		}
	}

	// Pin & Pin
	if (layer1 == Layers::BOWLINGPIN && layer2 == Layers::BOWLINGPIN)
	{
		// UserData から ピンオブジェクトのポインタを復元
		auto* pin = dynamic_cast<PinBase*>(gameObj1);
		auto* pin2 = dynamic_cast<PinBase*>(gameObj2);

		// どちらもあることを確認
		if (pin && pin2)
		{
			// ピン１が既に当てられているならピン２の吹っ飛びを呼ぶ
			if (pin->GetIsHit())
			{
				// プレイヤーの現在速度を取得してピンに通知！
				JPH::Vec3 pinVel = body1->GetLinearVelocity();
				pin2->OnHitByPin(pinVel);
			}

			// ピン２が既に当てられているならピン１の吹っ飛びを呼ぶ
			if (pin2->GetIsHit())
			{
				// プレイヤーの現在速度を取得してピンに通知！
				JPH::Vec3 pinVel = body2->GetLinearVelocity();
				pin->OnHitByPin(pinVel);
			}
		}
	}
}
