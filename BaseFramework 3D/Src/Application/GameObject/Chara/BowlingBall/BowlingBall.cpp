#include "BowlingBall.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"

BowlingBall::BowlingBall()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/PlayerBall/bowling_ball.gltf");
}

void BowlingBall::Init(float a_radius)
{
	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = Math::Vector3::Zero;
	initData.rot = Math::Quaternion::Identity;
	initData.motionType = JPH::EMotionType::Dynamic;
	initData.motionQuality = JPH::EMotionQuality::LinearCast;
	initData.isStatic = false;
	initData.layer = Layers::BOWLINGBALL;
	initData.mass = BowlingBallConsts::BallMass;
	initData.friction = 0.15f;
	initData.restitution = 0.15f;
	initData.linearDamping = 0.01f;
	initData.angularDamping = 0.02f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);	//自分自身のポインタを登録

	//物理Init
	m_cPhysics = std::make_shared<PhysicsComponent>();
	m_cPhysics->Init(a_radius, initData);
}

void BowlingBall::Update()
{
	// 転がり中じゃないならリターン
	if (!m_isRolling)return;

	// 停止チェック
	CheckIsStop();
}

void BowlingBall::PostUpdate()
{
	// 1. 物理座標の同期（Joltから最新座標を反映）
	m_cPhysics->Sync(m_pos, m_rot);

	// 落下チェック
	if (m_pos.y < STAGEMGR.GetStageInfo()->m_fallOutLine)
	{
		m_isFall = true;
		m_isRolling = false;
	}

	// 1. 回転
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);

	// 2. 位置（Translation）
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);

	// 3. ワールド行列の合成（旋回 → 移動)
	m_mWorld = rotat * trans;

	// デバッグ
	KdDebugGUI::Instance().AddLog("BallPos : %.2f,%.2f,%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	KdDebugGUI::Instance().AddLog("isRolling : %d\nisFall : %d\n", m_isRolling, m_isFall);
}

void BowlingBall::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::Throw(const Math::Vector3& startPos, const Math::Vector3& direction, float power)
{
	if (m_isRolling) return;

	m_isRolling = true;
	m_isFall = false;
	m_stopTimer = 0.0f;

	// 1.物理を一度止める
	DeactivateBody();

	// 2. プレイヤーから受け取った投球位置へ移動
	m_cPhysics->SetPosition(JPH::Vec3(startPos.x, startPos.y, startPos.z));
	m_cPhysics->SetRotation(JPH::Quat::sIdentity());

	// 3. 物理ボディをアクティブ化（描画フラグ等があればそれもON）
	ActivateBody();

	// 4. 指定された方向と強さでインパルス（初速）を与える
	JPH::Vec3 impulse = JPH::Vec3(direction.x, direction.y, direction.z).Normalized() * power * BowlingBallConsts::ThrowPowerMulti * BowlingBallConsts::BallMass;
	m_cPhysics->AddImpulse(impulse);
}

void BowlingBall::Reset()
{
	// 物理コンポーネントのリセット
	if (m_cPhysics)
	{
		m_cPhysics->SetLinearVelocity(JPH::Vec3::sZero());
		m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());
	}
	// 状態のリセット
	m_isRolling = false;
	m_isFall = false;
	m_stopTimer = 0.0f;
}

void BowlingBall::Respawn(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	// テスト
	Math::Vector3 testPos = pos + Math::Vector3(0, 0.2f, 0);

	// 状態のリセット
	Reset();

	// 物理ボディを一旦非活性化してから座標と回転を設定
	DeactivateBody();

	m_cPhysics->SetPosition(JPH::Vec3(pos.x, pos.y, pos.z));
	m_cPhysics->SetRotation(JPH::Quat(rot.x, rot.y, rot.z, rot.w));

	// 物理ボディを再度活性化
	ActivateBody();
}

void BowlingBall::ActivateBody()
{
	// 有効ならリターン
	if(m_isActive) return;

	m_cPhysics->ActivateBody();
	m_isActive = true;
}

void BowlingBall::DeactivateBody()
{
	// 無効ならリターン
	if (!m_isActive) return;

	m_cPhysics->DeactivateBody();
	m_isActive = false;
}

void BowlingBall::CheckIsStop()
{
	// ゲームデルタ
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 速度チェック
	JPH::Vec3 linearVelocity = PHYSICSMGR.GetBodyInterface().GetLinearVelocity(m_cPhysics->GetBodyID());

	// 速度の２乗が一定値以下かを確認
	if (linearVelocity.LengthSq() < BowlingBallConsts::StopCheckBorder)
	{
		// 連続停止時間増加
		m_stopTimer += gameDt;

		// もし一定時間以上止まっていたら終了フラグ
		if (m_stopTimer > BowlingBallConsts::RollEndTime)
		{
			m_isRolling = false;
		}
	}
	else
	{
		// 連続停止時間リセット
		m_stopTimer = 0.0f;
	}
}
