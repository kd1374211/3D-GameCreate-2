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
	// 最初は非活性
	Deactivate();
}
