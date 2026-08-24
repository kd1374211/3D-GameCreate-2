#include "PinBase.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"

void PinBase::Update()
{
	if (STAGEMGR.IsEditMode())
	{
		// エディット中は物理ボディの位置を動かさず、
		// STAGEMGR からもらった Position / Rotation をそのまま維持する
		return;
	}

	// デルタタイム(ゲーム)
	float gameDt = SCENEMGR.GetDeltaGameTime();

	if (!m_cPhysics || m_cPhysics->GetBodyID().IsInvalid()) return;

	// -------------------------------------------------------------
	// A. 保留されていたヒット処理の遅延実行（安全なタイミング）
	// -------------------------------------------------------------
	if (m_isHitPending)
	{
		m_isHitPending = false;
		m_isRagdoll = true;
		m_destroyTimer = 10.0f; // 消去タイマー開始

		JPH::BodyID bodyID = m_cPhysics->GetBodyID();
		JPH::BodyInterface& bodyInterface = PHYSICSMGR.GetSystem().GetBodyInterface();

		// 1. レイヤーを吹き飛び後（PIN_MOVING）に変更
		bodyInterface.SetObjectLayer(bodyID, Layers::PIN_MOVING);

		// ---------------------------------------------------------
		// ★ 2. 質量の変更と慣性モーメント（回転）の解放
		// （Update 内は物理ステップ外のため Safe に BodyLockWrite が使用可能）
		// ---------------------------------------------------------
		{
			JPH::BodyLockWrite lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), bodyID);
			if (lock.Succeeded())
			{
				JPH::Body& body = lock.GetBody();

				float normalMass = 1.0f;

				// 質量を1.0f（通常値）に変更
				body.GetMotionProperties()->SetInverseMass(1.0f / normalMass);

				// 慣性モーメント（回転のしやすさ）を再計算して回転を解放
				JPH::MassProperties massProps;
				if (body.GetShape())
				{
					massProps = body.GetShape()->GetMassProperties();
				}
				massProps.mMass = normalMass;

				JPH::Mat44 invInertiaMat = massProps.mInertia.Inversed();
				JPH::Vec3 invInertiaDiagonal(invInertiaMat(0, 0), invInertiaMat(1, 1), invInertiaMat(2, 2));

				body.GetMotionProperties()->SetInverseInertia(
					invInertiaDiagonal,
					JPH::Quat::sIdentity()
				);
			}
		}

		// 3. 吹き飛び後の跳ね返り係数（Restitution）をセット（例: 0.4f）
		m_cPhysics->SetRestitution(0.4f);

		// 4. プレイヤーの速度をもとにピンを弾き飛ばす
		JPH::Vec3 blowDir = m_pendingVelocity;
		blowDir.SetY(std::max(blowDir.GetY(), 2.0f)); // 上向きに跳ね上げる

		bodyInterface.AddImpulse(bodyID, blowDir * 2.0f);
	}

	// -------------------------------------------------------------
	// B. 状態に応じた毎フレーム処理
	// -------------------------------------------------------------
	if (!m_isRagdoll)
	{
		// 未接触時：倒れないように回転速度をゼロクリア
		m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());
	}
	else
	{
		// 吹き飛び後：消去タイマー減算
		m_destroyTimer -= gameDt;
		if (m_destroyTimer <= 0.0f)
		{
			m_isExpired = true;
		}
	}
}

void PinBase::PostUpdate()
{
	// 物理コンポーネントおよび BodyID の生存確認
	if (!m_cPhysics || m_cPhysics->GetBodyID().IsInvalid()) return;

	//同期
	m_cPhysics->Sync(m_pos);

	// 1. Jolt Physics から現在の最新の位置（Position）と回転（Rotation）を取得
	JPH::BodyInterface& bodyInterface = PHYSICSMGR.GetSystem().GetBodyInterface();

	//変換
	JPH::Quat rotation = bodyInterface.GetRotation(m_cPhysics->GetBodyID());
	Math::Quaternion rot(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());

	// Matrix更新
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromQuaternion(rot);
	m_mWorld = rotMat * trans; // ワールド行列を更新
}

void PinBase::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void PinBase::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void PinBase::OnHitByPlayer(JPH::Vec3 playerVelocity)
{
	if (m_isRagdoll || m_isHitPending) return;

	// OnContactAdded 内では直接 BodyInterface をいじらず、フラグと速度だけメモして抜ける
	m_isHitPending = true;
	m_pendingVelocity = playerVelocity;

	//ピンが倒れたという情報を送る
	STAGEMGR.OnPinFallen();
}

void PinBase::Init()
{
	//ピン数追加
	STAGEMGR.OnPinSpawn();
}
