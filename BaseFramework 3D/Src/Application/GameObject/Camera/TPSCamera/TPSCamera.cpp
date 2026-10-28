#include "TPSCamera.h"
#include "../../../Physics/PhysicsManager.h"

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 0.4f, -2.3f);
	
	// 見下ろし
	m_mLocalRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(30.0f));
}

void TPSCamera::PostUpdate()
{
	// 有効でないならリターン
	if (!m_isDefault)return;

	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}
	
	// カメラの回転
	if (!m_isCamLocked)
	{
		// ロック状態にないならカメラを回転させる
		UpdateRotateYOnlyByMouse();
	}
	
	m_mRotation = GetRotationMatrix();
	m_mWorld = m_mLocalPos * m_mRotation * _targetMat;

	m_mWorld = m_mLocalPos * m_mLocalRot * m_mRotation * _targetMat;

	// レイ用に現在の情報を取得
	Math::Vector3 currentPos = GetPos();
	// レイの発射方向を設定
	Math::Vector3 dir = Math::Vector3::Down;
	if (_spTarget)
	{
		Math::Vector3 _targetPos = _spTarget->GetPos();
		_targetPos.y += 0.1f;
		dir = _targetPos - GetPos();
	}

	// レイを作成
	JPH::RRayCast rayCast;
	rayCast.mOrigin = JPH::RVec3(currentPos.x, currentPos.y, currentPos.z);
	rayCast.mDirection = JPH::RVec3(dir.x, dir.y, dir.z);

	// レイを飛ばす(登録したボディーフィルタを使用)
	bool hasHit = false;
	JPH::RayCastResult hit;
	hasHit = PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastRay(rayCast, hit, {}, {}, *m_hitObjList);

	if (hasHit)
	{
		// 3. 衝突ボディ（Body）を取得
		JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), hit.mBodyID);
		if (lock.Succeeded()) {
			const JPH::Body& body = lock.GetBody();

			// 衝突点の3D座標を取得
			JPH::RVec3 hitPos = rayCast.GetPointOnRay(hit.mFraction);
			Math::Vector3 hitVec3Pos = Math::Vector3(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());

			// ワールド空間での法線（World Space Normal）を取得
			JPH::Vec3 worldNormal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, hitPos);

			// 法線がカメラと同方向かの確認
			JPH::Vec3 rayDir = rayCast.mDirection.Normalized();
			JPH::Vec3 surfaceNormal = worldNormal.Normalized();

			float dot = rayDir.Dot(surfaceNormal);

			// 同じ方向ならカメラを補正する
			if (dot > 0.0f)
			{
				dir.Normalize();
				hitVec3Pos += dir * 0.4f;
				SetPos(hitVec3Pos);
			}
		}
	}

	// ↓めり込み防止の為の座標補正計算↓
	// ①当たり判定(レイ判定)用の情報作成
	//KdCollider::RayInfo rayInfo;
	//// レイの発射位置を設定
	//rayInfo.m_pos = GetPos();

	//// レイの発射方向を設定
	//rayInfo.m_dir = Math::Vector3::Down;
	//// レイの長さを設定
	//rayInfo.m_range = 1000.f;
	//if (_spTarget)
	//{
	//	Math::Vector3 _targetPos = _spTarget->GetPos();
	//	_targetPos.y += 0.1f;
	//	rayInfo.m_dir = _targetPos - GetPos();
	//	rayInfo.m_range = rayInfo.m_dir.Length();
	//	rayInfo.m_dir.Normalize();
	//}

	//// 当たり判定をしたいタイプを設定
	//rayInfo.m_type = KdCollider::TypeGround;

	//// ②HIT判定対象オブジェクトに総当たり
	//for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	//{
	//	std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
	//	if (spGameObj)
	//	{
	//		std::list<KdCollider::CollisionResult> retRayList;
	//		spGameObj->Intersects(rayInfo, &retRayList);

	//		// ③ 結果を使って座標を補完する
	//		// レイに当たったリストから一番近いオブジェクトを検出
	//		float maxOverLap = 0;
	//		Math::Vector3 hitPos = {};
	//		bool hit = false;
	//		for (auto& ret : retRayList)
	//		{
	//			// レイを遮断しオーバーした長さが
	//			// 一番長いものを探す
	//			if (maxOverLap < ret.m_overlapDistance)
	//			{
	//				maxOverLap = ret.m_overlapDistance;
	//				hitPos = ret.m_hitPos;
	//				hit = true;
	//			}
	//		}
	//		if (hit)
	//		{
	//			// 何かしらの障害物に当たっている
	//			Math::Vector3 _hitPos = hitPos;
	//			_hitPos += rayInfo.m_dir * 0.4f;
	//			SetPos(_hitPos);
	//		}
	//	}
	//}
}
