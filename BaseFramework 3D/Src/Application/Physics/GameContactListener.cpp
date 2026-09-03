#include "GameContactListener.h"
#include "PhysicsLayer.h"

//オブジェクトのインクルード
#include "../GameObject/BowlingPin/PinBase/PinBase.h"
#include "../GameObject/Chara/Player/Player.h"

void GameContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	// 衝突した2つのレイヤーを取得
	JPH::ObjectLayer layer1 = inBody1.GetObjectLayer();
	JPH::ObjectLayer layer2 = inBody2.GetObjectLayer();

	// ボディーを取得
	const JPH::Body* body1 = &inBody1;
	const JPH::Body* body2 = &inBody2;

	// 衝突法線（初期値は Body1 -> Body2）
	JPH::Vec3 normal = inManifold.mWorldSpaceNormal;

	//番号が小さいレイヤーを基準にする
	if (layer1 > layer2)
	{
		std::swap(layer1, layer2);
		std::swap(body1, body2);

		//法線反転
		normal = -normal;
	}

	// 2. 固定された body1 / body2 から事前キャスト
	auto* gameObj1 = reinterpret_cast<KdGameObject*>(body1->GetUserData());
	auto* gameObj2 = reinterpret_cast<KdGameObject*>(body2->GetUserData());

	//// Terrain & Player
	//if (layer1 == Layers::TERRAIN && layer2 == Layers::BOWLINGBALL)
	//{
	//	// ★ Y成分の絶対値が一定以下（＝水平に近い＝壁）の場合のみ通知
	//	if (std::abs(normal.GetY()) < 0.3f)
	//	{
	//		if (auto player = dynamic_cast<Player*>(gameObj2))
	//		{
	//			player->OnHitWall(normal);
	//		}
	//	}
	//}

	//// Finish & Player
	//if (layer1 == Layers::FINISHAREA && layer2 == Layers::BOWLINGBALL)
	//{
	//	if (auto player = dynamic_cast<Player*>(gameObj2))
	//	{
	//		player->Finish();
	//	}
	//}

	// Player & Pin
	if (layer1 == Layers::BOWLINGBALL && layer2 == Layers::PIN_STATIC)
	{
		// UserData から ピンオブジェクトのポインタを復元
		if (auto* pin = dynamic_cast<PinBase*>(gameObj2))
		{
			// プレイヤーの現在速度を取得してピンに通知！
			JPH::Vec3 playerVel = body1->GetLinearVelocity();
			pin->OnHitByPlayer(playerVel);
		}
	}
}

void GameContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	// 衝突した2つのレイヤーを取得
	JPH::ObjectLayer layer1 = inBody1.GetObjectLayer();
	JPH::ObjectLayer layer2 = inBody2.GetObjectLayer();

	// ボディーを取得
	const JPH::Body* body1 = &inBody1;
	const JPH::Body* body2 = &inBody2;

	// 衝突法線（プレイヤーから見て押し返される向き）
	JPH::Vec3 normal = inManifold.mWorldSpaceNormal;

	//番号が小さいレイヤーを基準にする
	if (layer1 > layer2)
	{
		std::swap(layer1, layer2);
		std::swap(body1, body2);

		//法線反転
		normal = -normal;
	}

	// 2. 固定された body1 / body2 から事前キャスト
	auto* gameObj1 = reinterpret_cast<KdGameObject*>(body1->GetUserData());
	auto* gameObj2 = reinterpret_cast<KdGameObject*>(body2->GetUserData());

	//// Terrain & Player
	//if (layer1 == Layers::TERRAIN && layer2 == Layers::BOWLINGBALL)
	//{
	//	// ★ Y成分の絶対値が一定以下（＝水平に近い＝壁）の場合のみ通知
	//	// std::abs(normal.GetY()) < 0.5f で約45度以上の急傾斜・壁を判別
	//	if (std::abs(normal.GetY()) < 0.5f)
	//	{
	//		if (auto player = dynamic_cast<Player*>(gameObj2))
	//		{
	//			player->OnHitWall(normal);
	//		}
	//	}
	//}

	//// Finish & Player
	//if (layer1 == Layers::FINISHAREA && layer2 == Layers::BOWLINGBALL)
	//{
	//	if (auto player = dynamic_cast<Player*>(gameObj2))
	//	{
	//		player->Finish();
	//	}
	//}
}
