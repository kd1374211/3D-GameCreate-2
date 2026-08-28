#include "BowlingBall.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"

BowlingBall::BowlingBall()
{
}

void BowlingBall::Init(const Math::Vector3& a_startPos, float a_radius)
{
	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = a_startPos;
	initData.rot = Math::Quaternion::Identity;
	initData.motionType = JPH::EMotionType::Dynamic;
	initData.motionQuality = JPH::EMotionQuality::LinearCast;
	initData.isStatic = false;
	initData.layer = Layers::PLAYER;
	initData.mass = 10000.0f;
	initData.friction = 0.0f;
	initData.restitution = 0.0f;
	initData.linearDamping = 0.0f;
	initData.angularDamping = 0.0f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);	//自分自身のポインタを登録

	//物理Init
	m_cPhysics = std::make_shared<PhysicsComponent>();
	m_cPhysics->Init(a_radius, initData);

	//半径保存
	m_radius = a_radius;
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
}

void BowlingBall::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::Throw(const JPH::Vec3& startPos, const JPH::Vec3& direction, float power)
{
	if (m_isRolling) return;

	m_isRolling = true;
	m_isFall = false;
	m_stopTimer = 0.0f;

	// 1. 速度・角速度のリセット（前回の残像速度を消す）
	m_cPhysics->SetLinearVelocity(JPH::Vec3::sZero());
	m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());

	// 2. プレイヤーから受け取った投球位置へ移動 & 回転をリセット
	m_cPhysics->SetPosition(startPos);
	m_cPhysics->SetRotation(JPH::Quat::sIdentity());

	// 3. 物理ボディをアクティブ化（描画フラグ等があればそれもON）
	m_cPhysics->ActivateBody();

	// 4. 指定された方向と強さでインパルス（初速）を与える
	JPH::Vec3 impulse = direction.Normalized() * power;
	m_cPhysics->AddImpulse(impulse);
}

void BowlingBall::Reset()
{}

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
