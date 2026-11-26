#include "TPSCamera.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 0.6f, -3.5f);
	
	// 見下ろし
	m_mLocalRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(30.0f));
}

void TPSCamera::PostUpdate()
{
	// 有効でないならリターン
	if (!m_isDefault)return;

	// ゲームデルタ取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	Math::Matrix _targetMat = Math::Matrix::CreateTranslation(m_currentPivotPos);
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		// 目標となる中心点（ボールの現在座標）
		Math::Vector3 goalPos = _spTarget->GetPos();

		// 現在のピボット位置から目標位置への差分ベクトル
		Math::Vector3 _distToTarget = goalPos - m_currentPivotPos;

		// 補間係数
		float lerp = 1.0f - std::exp(-TPSCameraConsts::LerpSpeed * gameDt);

		// ピボット位置を更新
		m_currentPivotPos += _distToTarget * lerp;
	
		// ピボット距離の補正
		Math::Vector3 diff = goalPos - m_currentPivotPos;
		float distance = diff.Length();

		// 最大を超えたら最大に補正する
		if (distance > TPSCameraConsts::MaxDistance)
		{
			diff.Normalize();
			m_currentPivotPos = goalPos - (diff * TPSCameraConsts::MaxDistance);
		}
		// 最小を下回ったら一致させる
		else if (distance < TPSCameraConsts::MinDistance)
		{
			m_currentPivotPos = goalPos;
		}

		// 更新されたピボット位置を行列化
		_targetMat = Math::Matrix::CreateTranslation(m_currentPivotPos);
	}

	// カメラの回転（ロックされていなければマウス操作で角度を更新）
	if (!m_isCamLocked)
	{
		UpdateRotateYOnlyByMouse();
	}

	m_mRotation = GetRotationMatrix();

	// ピボット(_targetMat)を中心に、カメラ自身の向き(m_mRotation)と距離(m_mLocalPos)を適用
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

			//同じ方向ならカメラを補正する
			if (dot > 0.0f)
			{
				dir.Normalize();
				hitVec3Pos += dir * 0.4f;
				SetPos(hitVec3Pos);
			}
		}
	}
}

void TPSCamera::SetPivotToCurrentTarget()
{
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_currentPivotPos = _spTarget->GetPos();
	}
}
