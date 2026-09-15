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

	// 吹っ飛び待機があるなら実行
	if (m_isHitPending)
	{
		// 待機力を強くして加える
		m_cPhysics->AddImpulse(m_pendingVelocity * PinBaseConsts::OnHitVelocityMulti);

		// 次呼ばれないように
		m_isHitPending = false;
	}

	// 倒れチェック
	m_isFallen = CheckIsFallen();
}

void PinBase::PostUpdate()
{
	// 非活性状態ならリターン
	if (!m_isActive) return;

	// 物理コンポーネントおよび BodyID の生存確認
	if (!m_cPhysics || m_cPhysics->GetBodyID().IsInvalid()) return;

	//同期
	m_cPhysics->Sync(m_pos, m_rot);

	// 落下チェック
	if (m_pos.y < STAGEMGR.GetStageInfo()->m_fallOutLine)
	{
		m_isFallen = true;
	}

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

void PinBase::ActivateBody()
{
	m_cPhysics->ActivateBody();
	m_isBodyActive = true;
}

void PinBase::DeactivateBody()
{
	m_cPhysics->DeactivateBody();
	m_isBodyActive = false;
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
	m_isHit = false;
	m_isHitPending = false;
	m_pendingVelocity = JPH::Vec3::sZero();
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
	m_isActive = true;

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
	m_isActive = false;
	DeactivateBody();

	// 3.ピンの管理番号を無効値に
	m_pinIndex = -1;
}

void PinBase::SetPos(const Math::Vector3 pos)
{
	// ゲーム内の変数を変更
	m_pos = pos;

	// Joltに反映
	m_cPhysics->SetPosition(JPH::Vec3(pos.x, pos.y, pos.z));
}

void PinBase::SetRot(const Math::Quaternion rot)
{
	// ゲーム内の変数を変更
	m_rot = rot;

	// Joltに反映
	m_cPhysics->SetRotation(JPH::Quat(rot.x, rot.y, rot.z, rot.w));
}

void PinBase::OnHit(JPH::Vec3 vel)
{
	// もう当たっているならリターン
	if (m_isHit)return;

	// 吹っ飛び待機
	m_isHitPending = true;
	m_isHit = true;

	m_pendingVelocity = vel;
}

void PinBase::Init()
{
	// 最初は非活性
	m_isActive = false;
	DeactivateBody();
}

bool PinBase::CheckIsFallen()
{
	// 既に倒れていると判定済みなら更新しない（必要に応じて）
	if (m_isFallen) return true;

	// 非アクティブ（画面上に存在しない等）なら判定しない
	if (!m_isActive) return false;

	// 1. ピンの現在の回転（Quaternion）からワールドでの「上方向ベクトル」を取得
	// ※お使いの Math クラス（DirectXMath等）に合わせてベクトル変換を行ってください
	Math::Vector3 pinUp = Math::Vector3::Transform(Math::Vector3(0.0f, 1.0f, 0.0f), m_rot);
	pinUp.Normalize();

	// ワールドの垂直上方向
	Math::Vector3 worldUp(0.0f, 1.0f, 0.0f);

	// 2. ピンの上方向とワールドの上方向の内積を計算
	float dot = pinUp.Dot(worldUp);

	// 3. 傾き角度が約45度以上（cos(45°) ≒ 0.707）傾いたら倒れたと判定
	const float fallenThreshold = 0.707f;

	if (dot < fallenThreshold)
	{
		m_isFallen = true;
	}

	return m_isFallen;
}
