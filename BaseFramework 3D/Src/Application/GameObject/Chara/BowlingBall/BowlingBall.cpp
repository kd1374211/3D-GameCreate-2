#include "BowlingBall.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"
#include "../../Camera/CameraManager.h"
#include "../../Camera/CameraBase.h"

BowlingBall::BowlingBall()
{
	//モデル
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/Chara/PlayerBall/bowling_ball.gltf");

	// 矢印
	m_arrowModel = std::make_shared<KdModelData>();
	m_arrowModel->Load("Asset/Models/PowerArrow/PowerArrow.gltf");
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
	initData.restitution = 0.0f;
	initData.linearDamping = 0.1f;
	initData.angularDamping = 0.1f;
	initData.userData = reinterpret_cast<JPH::uint64>(this);	//自分自身のポインタを登録

	//物理Init
	m_cPhysics = std::make_shared<PhysicsComponent>();
	m_cPhysics->Init(a_radius, initData);
}

void BowlingBall::Update()
{
	// エディットモード中はプレイヤーの移動・操作・物理を停止
	if (STAGEMGR.IsEditMode())
	{
		return;
	}

	// 転がっていないとき
	if (!m_isRolling)
	{
		//移動不可ならリターン
		if (!m_canRoll)return;

		// ゲームタイム
		float gameDt = SCENEMGR.GetDeltaGameTime();

		// 操作関連
		if (m_isInputEnabled)
		{
			// 投げる方向決め
			if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			{
				// 左回転
				m_facingAngle -= BowlingBallConsts::TurnSpeed * gameDt;
			}
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			{
				// 右回転
				m_facingAngle += BowlingBallConsts::TurnSpeed * gameDt;
			}
			// 補正
			if (m_facingAngle >= 360.0f)m_facingAngle -= 360.0f;
			else if (m_facingAngle <= 0.0f)m_facingAngle += 0.0f;

			// 投げる強さ決め
			if (GetAsyncKeyState(VK_UP) & 0x8000)
			{
				// 強く
				m_throwPower += BowlingBallConsts::PowerChangeSpeed * gameDt;
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				// 弱く
				m_throwPower -= BowlingBallConsts::PowerChangeSpeed * gameDt;
			}
			// 補正
			if (m_throwPower >= BowlingBallConsts::MaxPower)m_throwPower = BowlingBallConsts::MaxPower;
			else if (m_throwPower <= BowlingBallConsts::MinPower)m_throwPower = BowlingBallConsts::MinPower;

			// 投げ
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{
				// 投げ
				Throw(m_pos, GetForwardVectorFromFacingAngle(m_facingAngle), m_throwPower);

				// 投げたら操作不可に
				m_isInputEnabled = false;
			}
		}
	}
	else
	{
		// 停止チェック
		CheckIsStop();

		// DEBUG
		JPH::Vec3 linearV;
		JPH::Vec3 angularV;
		PHYSICSMGR.GetBodyInterface().GetLinearAndAngularVelocity(m_cPhysics->GetBodyID(), linearV, angularV);
		KdDebugGUI::Instance().AddLog("Linear Velocity : %.2f,%.2f,%.2f\n", linearV.GetX(), linearV.GetY(), linearV.GetZ());
		KdDebugGUI::Instance().AddLog("Angular Velocity : %.2f,%.2f,%.2f\n", angularV.GetX(), angularV.GetY(), angularV.GetZ());
	}

	//カメラに設定
	if (m_wpCamera.expired())return;

	m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_facingAngle)));
}

void BowlingBall::PostUpdate()
{
	// 1. 物理座標の同期（Joltから最新座標を反映）
	m_cPhysics->Sync(m_pos, m_rot);

	// 落下チェック
	if (m_pos.y < STAGEMGR.GetStageInfo()->m_fallOutLine)
	{
		m_isRolling = false;
		m_reason = RollEndReason::Fall;
	}

	// 1. 回転
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);

	// 2. 位置（Translation）
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);

	// 3. ワールド行列の合成（旋回 → 移動)
	m_mWorld = rotat * trans;

	// 矢印配置テスト
	Math::Matrix arrowLocalPos = Math::Matrix::CreateTranslation(0, 0, 0.25f);
	Math::Matrix arrowScale = Math::Matrix::CreateScale(Math::Vector3(0.25f, m_throwPower * 0.75f, 0.25f));
	Math::Matrix arrowRotX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90.0f));


	// 4. Math::Matrix を使って行列を作成
	// ※度数法 -> 弧度法（ラジアン）変換 ( m_facingAngle * (π / 180.0f) )
	float yawRad = m_facingAngle * static_cast<float>(M_PI / 180.0f);

	// B. 進行方向への旋回（Y軸）
	Math::Matrix matYaw = Math::Matrix::CreateRotationY(yawRad);

	// C. 位置（Translation）
	Math::Matrix matTrans = Math::Matrix::CreateTranslation(
		m_pos
	);

	//
	Math::Matrix arrowLocalMat = arrowScale * arrowRotX * arrowLocalPos;
	m_arrowMat = arrowLocalMat * matYaw * matTrans;

	// デバッグ
	KdDebugGUI::Instance().AddLog("BallPos : %.2f,%.2f,%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	KdDebugGUI::Instance().AddLog("isRolling : %d\n", m_isRolling);
}

void BowlingBall::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);

	// 操作不可ならリターン
	if (!m_isInputEnabled)return;

	// 矢印
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_arrowModel, m_arrowMat);
}

void BowlingBall::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::Throw(const Math::Vector3& startPos, const Math::Vector3& direction, float power)
{
	if (m_isRolling) return;

	m_isRolling = true;
	m_reason = RollEndReason::None;
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
	m_canRoll = true;
	m_isInputEnabled = true;
	m_reason = RollEndReason::None;
	m_stopTimer = 0.0f;
	m_throwPower = BowlingBallConsts::StartPower;
}

void BowlingBall::Respawn(const Math::Vector3& pos, const Math::Quaternion& rot)
{
	// 状態のリセット
	Reset();

	// 物理ボディを一旦非活性化してから座標と回転を設定
	DeactivateBody();

	m_cPhysics->SetPosition(JPH::Vec3(pos.x, pos.y, pos.z));
	m_cPhysics->SetRotation(JPH::Quat(rot.x, rot.y, rot.z, rot.w));

	// 物理ボディを再度活性化
	ActivateBody();

	// ↓仮置き
	// 位置と向きを設定
	m_pos = pos;
	m_facingAngle = GetFacingAngleFromQuaternion(rot);
}

void BowlingBall::HitFinishArea()
{
	if (!m_isRolling)return;

	m_isRolling = false;
	m_reason = RollEndReason::Finish;
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
			m_reason = RollEndReason::Stop;
		}
	}
	else
	{
		// 連続停止時間リセット
		m_stopTimer = 0.0f;
	}
}
