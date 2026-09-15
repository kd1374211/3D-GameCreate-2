#include "BowlingBall.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"
#include "../../Camera/CameraManager.h"
#include "../../Camera/TPSCamera/TPSCamera.h"
#include "../../../Const/WindowConsts.h"
#include "../../ShotCursor/ShotCursor.h"

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

	// カーソル生成
	std::shared_ptr<ShotCursor> cursor = std::make_shared<ShotCursor>();
	SCENEMGR.AddObject(cursor);
	m_wpCursor = cursor;
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
			POINT cursorPos;
			// 現在のマウス位置を取得
			GetCursorPos(&cursorPos);
			Math::Vector2 fixedPos = GetFixedCursorPos(cursorPos);

			KdDebugGUI::Instance().AddLog("CursorPos : %.2f,%.2f\n", fixedPos.x, fixedPos.y);

			// 左クリックで準備
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				// まだ押してなければ
				if (!m_isShootStart)
				{
					// ショット開始フラグをオンに
					m_isShootStart = true;

					// カメラの回転を固定
					if (!m_wpCamera.expired())
					{
						m_wpCamera.lock()->SetIsCamLocked(true);
					}
				}
			}
			// 左クリックを離す
			else
			{
				// もし押している状態だったら
				if (m_isShootStart)
				{
					// 前回と今回の差を確認
					float mouseMoveDist = fixedPos.y;

					// ゲーム時間から1秒ごとの速度を確認
					mouseMoveDist /= gameDt;

					// 速度から発射速度を確定(最大値は制限)
					float throwPower = std::min(mouseMoveDist / BowlingBallConsts::ThrowSpeedDiv, BowlingBallConsts::ThrowSpeedMax);

					// 速度が一定値以上＆上向き
					if (throwPower > BowlingBallConsts::ThrowSpeedMin)
					{
						// 角度取得
						Math::Vector3 direction = Math::Vector3(0, 1, 0);
						if (!m_wpCamera.expired())
						{
							direction = m_wpCamera.lock()->GetRotationYMatrix().Backward();
						}

						// 速度と角度をもとに投げる
						Throw(m_pos, direction, throwPower);

						// 操作不可に
						m_isInputEnabled = false;
					}
					else
					{
						// カメラの回転を解放(投げキャンセル)
						if (!m_wpCamera.expired())
						{
							m_wpCamera.lock()->SetIsCamLocked(false);
						}
					}
					
					// 投げ開始フラグを戻す
					m_isShootStart = false;
				}
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

	// カーソルの描画フラグを更新
	auto cursor = m_wpCursor.lock();
	if (cursor)
	{
		// 描画フラグを更新
		cursor->SetIsDraw(m_isShootStart);

		// 描画フラグオンなら位置も更新
		if (m_isShootStart)
		{
			auto activeCam = CAMERAMGR.GetActiveCamera().lock();

			if (activeCam)
			{
				// ボールの位置を変換
				Math::Vector3 resultPos;
				activeCam->WorkCamera()->ConvertWorldToScreenDetail(m_pos, resultPos);

				// そこに指定
				cursor->SetDrawPos(Math::Vector2(resultPos.x, resultPos.y));
			}
		}
	}

	//カメラに設定
	//if (m_wpCamera.expired())return;

	//m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_facingAngle)));
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
	if (m_isShootStart && m_isInputEnabled) // 左クリックホールド中
	{
		Math::Matrix arrowLocalPos = Math::Matrix::CreateTranslation(0, 0, 0.25f);
		Math::Matrix arrowScale = Math::Matrix::CreateScale(Math::Vector3(0.25f, 0.25f, 0.25f));
		Math::Matrix arrowRotX = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90.0f));

		// カメラからY回転を取得
		Math::Matrix rotY;
		if (!m_wpCamera.expired())
		{
			rotY = m_wpCamera.lock()->GetRotationYMatrix();
		}
		else
		{
			rotY = Math::Matrix::CreateRotationY(0);
		}

		// C. 位置（Translation）
		Math::Matrix matTrans = Math::Matrix::CreateTranslation(
			m_pos
		);

		//
		Math::Matrix arrowLocalMat = arrowScale * arrowRotX * arrowLocalPos;
		m_arrowMat = arrowLocalMat * rotY * matTrans;
	}

	// デバッグ
	KdDebugGUI::Instance().AddLog("BallPos : %.2f,%.2f,%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	KdDebugGUI::Instance().AddLog("isRolling : %d\n", m_isRolling);
}

void BowlingBall::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);

	// 左クリックホールド中のみ
	if (m_isShootStart && m_isInputEnabled)
	{
		// 矢印
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_arrowModel, m_arrowMat);
	}
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

	// 2. 受け取った投球位置へ移動
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
	m_isInputEnabled = false;
	m_reason = RollEndReason::None;
	m_stopTimer = 0.0f;

	// マウス関連のリセット
	m_isShootStart = false;

	// カメラの回転を解放
	if (!m_wpCamera.expired())
	{
		m_wpCamera.lock()->SetIsCamLocked(false);
	}
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
	if (!m_wpCamera.expired())
	{
		m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateFromQuaternion(rot));
	}
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
