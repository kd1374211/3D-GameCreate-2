#include "PinBase.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"

void PinBase::Update()
{
	// 非活性状態ならリターン
	if (!m_isActive) return;

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
}

void PinBase::PostUpdate()
{
	// 非活性状態ならリターン
	if (!m_isActive) return;

	// 物理コンポーネントおよび BodyID の生存確認
	if (!m_cPhysics || m_cPhysics->GetBodyID().IsInvalid()) return;

	//同期
	m_cPhysics->Sync(m_pos, m_rot);

	// Matrix更新
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotMat = Math::Matrix::CreateFromQuaternion(m_rot);
	m_mWorld = rotMat * trans; // ワールド行列を更新
}

void PinBase::DrawLit()
{
	// 非活性状態ならリターン
	if (!m_isActive) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void PinBase::GenerateDepthMapFromLight()
{
	// 非活性状態ならリターン
	if (!m_isActive) return;

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

void PinBase::Activate()
{
	m_isActive = true;
	m_cPhysics->ActivateBody();
}

void PinBase::Deactivate()
{
	m_isActive = false;
	m_cPhysics->DeactivateBody();
}

void PinBase::Reset()
{
	// 物理コンポーネントのリセット
	if (m_cPhysics)
	{
		m_cPhysics->SetLinearVelocity(JPH::Vec3::sZero());
		m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());
	}
	// 状態のリセット
	m_isRagdoll = false;
	m_isHitPending = false;
	m_pendingVelocity = JPH::Vec3::sZero();
	m_isFallen = false;
}

void PinBase::Spawn(Math::Vector3 pos, Math::Quaternion rot, int index)
{
	// もし活性化状態ならリターン
	if (m_isActive)return;

	// 1.位置と回転の設定
	SetPos(pos);
	SetRot(rot);

	// 2.リセット
	Reset();

	// 3.使用状態を活性化にする
	Activate();

	// 4.ピンに管理番号を付与
	m_pinIndex = index;
}

void PinBase::Despawn()
{
	// もし非活性状態ならリターン
	if (!m_isActive)return;

	// 1.念のためリセット
	Reset();

	// 2.使用状態を非活性にする
	Deactivate();

	// 3.ピンの管理番号を無効値に
	m_pinIndex = -1;
}

void PinBase::SetPos(const Math::Vector3 pos)
{
	m_cPhysics->SetPosition(JPH::Vec3(pos.x, pos.y, pos.z));
}

void PinBase::SetRot(const Math::Quaternion rot)
{
	m_cPhysics->SetRotation(JPH::Quat(rot.x, rot.y, rot.z, rot.w));
}

void PinBase::Init()
{
	//ピン数追加
	STAGEMGR.OnPinSpawn();

	// 最初は非活性
	Deactivate();
}
