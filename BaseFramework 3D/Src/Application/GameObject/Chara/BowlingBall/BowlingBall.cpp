#include "BowlingBall.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Scene/SceneManager.h"
#include "../../../StageManager/StageManager.h"
#include "../../Camera/CameraManager.h"
#include "../../Camera/TPSCamera/TPSCamera.h"
#include "../../UI/ShotUI/ShotCursor/ShotCursor.h"
#include "../../UI/ShotUI/ShotPowerBar/ShotPowerBar.h"
#include "../../../Const/Function.h"
#include "../../../Cursor/CursorManager.h"
#include "../../Effect/EffectManager.h"
#include "../../../Sound/SoundManager.h"
#include "../../UI/SceneUIObjects/Game/GameUIObjects.h"

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

	// バー生成
	std::shared_ptr<ShotPowerBar> bar = std::make_shared<ShotPowerBar>();
	SCENEMGR.AddObject(bar);
	bar->SetIsDraw(true);
	bar->SetPower(0.0f);
	m_wpPowerBar = bar;
}

void BowlingBall::Update()
{
	// ゲームタイム
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// エディットモード中はプレイヤーの移動・操作・物理を停止
	if (STAGEMGR.IsEditMode())
	{
		return;
	}

	// 接地フラグリセット前に音の更新
	UpdateRollingSound();

	// 接地フラグリセット
	m_isOnGroundThisFrame = false;

	// 転がっていないとき
	if (!m_isRolling)
	{
		//移動不可ならリターン
		if (!m_canRoll)return;

		// 操作関連
		if (m_isInputEnabled)
		{
			Math::Vector2 cursorPos = CURSOR.GetFixedCursorPosVec2();

			// 左クリックで準備
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				// まだ押してなければ
				if (!m_isShootStart)
				{
					// ショット開始フラグをオンに
					m_isShootStart = true;
					// 平均をリセット
					m_speedStoreData.clear();
					m_totalStoredTime = 0.0f;
					// UIに通知
					if (const auto& spUI = m_wpGameUI.lock())
					{
						spUI->ChangeGuideMode(MouseGuideMode::ReadyThrow);
					}

					// カメラの回転を固定
					if (auto gameCam = m_wpCamera.lock())
					{
						gameCam->SetIsCamLocked(true);
					}
				}

				// 押している間
				// このフレームでの移動量を追加
				m_speedStoreData.push_back(SpeedStoreData(gameDt, std::max(cursorPos.y, 0.0f)));
				m_totalStoredTime += gameDt;

				// 保持上限時間を超えなくなるまで古い記録を削除
				while (m_totalStoredTime - m_speedStoreData.begin()->m_gameDt > BowlingBallConsts::AverageCalcTime)
				{
					m_totalStoredTime -= m_speedStoreData.begin()->m_gameDt;
					m_speedStoreData.erase(m_speedStoreData.begin());
				}

				// バーに強さを設定
				if (!m_wpPowerBar.expired())
				{
					m_wpPowerBar.lock()->SetPower(std::clamp(CalcAvgMoveSpeed() / BowlingBallConsts::ThrowSpeedDiv, 0.0f, BowlingBallConsts::ThrowSpeedMax));
				}
			}
			// 左クリックを離す
			else
			{
				// もし押している状態だったら
				if (m_isShootStart)
				{
					// このフレームでの移動量を追加
					m_speedStoreData.push_back(SpeedStoreData(gameDt, std::max(cursorPos.y, 0.0f)));
					m_totalStoredTime += gameDt;

					// ゲーム時間から1秒ごとの速度を確認
					float mouseMoveAvg = CalcAvgMoveSpeed();

					// 速度から発射速度を確定(最大値は制限)
					float throwPower = std::min(mouseMoveAvg / BowlingBallConsts::ThrowSpeedDiv, BowlingBallConsts::ThrowSpeedMax);

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

						// カメラアンロック時間設定
						m_camUnlockTime = BowlingBallConsts::CameraUnlockTimeAfterThrow;

						// アンロック待ち状態に
						m_isCamUnlockWait = true;

						// 操作不可に
						m_isInputEnabled = false;

						// UIに通知
						if (const auto& spUI = m_wpGameUI.lock())
						{
							spUI->ChangeGuideMode(MouseGuideMode::AfterThrow);
						}
					}
					else
					{
						// カメラの回転を解放(投げキャンセル)
						if (auto gameCam = m_wpCamera.lock())
						{
							gameCam->SetIsCamLocked(false);
						}

						// UIに通知
						if (const auto& spUI = m_wpGameUI.lock())
						{
							spUI->ChangeGuideMode(MouseGuideMode::Default);
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

	// カメラの回転再開待ち時間減少
	if (m_isCamUnlockWait)
	{
		m_camUnlockTime -= gameDt;

		// 待ち時間が終わったらカメラの回転を再開
		if (m_camUnlockTime <= 0.0f)
		{
			if (auto gameCam = m_wpCamera.lock())
			{
				gameCam->SetIsCamLocked(false);
			}
		}
	}
}

void BowlingBall::PostUpdate()
{
	// エディットモード中はプレイヤーの物理を停止
	if (!STAGEMGR.IsEditMode())
	{
		// 1. 物理座標の同期（Joltから最新座標を反映）
		m_cPhysics->Sync(m_pos, m_rot);

		// 落下チェック
		if (m_pos.y < STAGEMGR.GetStageInfo()->m_fallOutLine)
		{
			m_isRolling = false;
			m_reason = RollEndReason::Fall;
		}

		// 消滅チェック
		if (m_pos.y < BowlingBallConsts::DisappearPosY)
		{
			// Stop
			DeactivateBody();
			m_isDraw = false;
		}		
	}

	// 1. 回転
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);

	// 2. 位置（Translation）
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);

	// 3. ワールド行列の合成（旋回 → 移動)
	m_mWorld = rotat * trans;

	// 矢印配置
	if (m_isInputEnabled)
	{
		// チャージ中なら伸ばす
		Math::Vector3 scaleMulti = Math::Vector3::One;
		if (m_isShootStart)
		{
			// ゲーム時間から1秒ごとの速度を確認
			float mouseMoveAvg = CalcAvgMoveSpeed();

			// 速度から発射速度を確定(最大値は制限)
			float throwPower = std::min(mouseMoveAvg / BowlingBallConsts::CursorPowerDiv, BowlingBallConsts::CursorPowerMax);

			// パワーに合わせてアローを伸ばす
			scaleMulti.y += throwPower;
		}

		Math::Matrix arrowLocalPos = Math::Matrix::CreateTranslation(0, 0, 0.25f);
		Math::Matrix arrowScale = Math::Matrix::CreateScale(BowlingBallConsts::ShotArrowScale * scaleMulti);
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

		// 矢印の行列
		Math::Matrix arrowLocalMat = arrowScale * arrowRotX * arrowLocalPos;
		m_arrowMat = arrowLocalMat * rotY * matTrans;
	}
}

void BowlingBall::DrawUnLit()
{
	// 左クリックホールド中のみ
	if (m_isInputEnabled)
	{
		// 矢印
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_arrowModel, m_arrowMat);
	}
}

void BowlingBall::DrawLit()
{
	// 描画しないならスキップ
	if (!m_isDraw)return;

	// トゥーン切り替え
	KdShaderManager::Instance().m_StandardShader.SetToonEnable(true);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);

	// アウトラインを切り替えて再度描画
	KdShaderManager::Instance().m_StandardShader.SetOutLineEnable(true);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void BowlingBall::GenerateDepthMapFromLight()
{
	// 描画しないならスキップ
	if (!m_isDraw)return;

	// 描画
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

	// 5. 音の再生＆管理用に受け取る
	m_wpStoredSound = SOUNDMGR.PlayAndGetInst(SoundType::SE_Throw);
	// ステート更新
	m_soundState = RollingSoundState::Throw;
}

void BowlingBall::EndThrow()
{
	// アクティブな音の削除
	if (const auto& sound = m_wpStoredSound.lock())
	{
		sound->Stop();
	}

	// サウンドステートのリセット
	m_soundState = RollingSoundState::None;
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
	m_isHitPin = false;
	m_canRoll = true;
	m_isInputEnabled = false;
	m_reason = RollEndReason::None;
	m_stopTimer = 0.0f;
	m_isDraw = true;

	// マウス関連のリセット
	m_isShootStart = false;
	m_speedStoreData.clear();
	m_totalStoredTime = 0.0f;

	// カメラの回転を解放
	if (auto gameCam = m_wpCamera.lock())
	{
		gameCam->SetIsCamLocked(false);
	}

	// 回転待ち状態ならオフに
	m_isCamUnlockWait = false;

	// パワーリセット
	if (auto powerBar = m_wpPowerBar.lock())
	{
		powerBar->SetPower(0.0f);
	}
}

void BowlingBall::Respawn(const Math::Vector3& pos, const Math::Vector3& rot)
{
	// rotをQuatに変換
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	// 状態のリセット
	Reset();

	// 物理ボディを一旦非活性化してから座標と回転を設定
	DeactivateBody();

	m_cPhysics->SetPosition(JPH::Vec3(pos.x, pos.y, pos.z));
	m_cPhysics->SetRotation(JPH::Quat(quat.x, quat.y, quat.z, quat.w));

	// 物理ボディを再度活性化
	ActivateBody();

	// ↓仮置き
	// 位置と向きを設定
	m_pos = pos;
	if (auto gameCam = m_wpCamera.lock())
	{
		gameCam->SetRotationYMatrix(Math::Matrix::CreateFromQuaternion(quat));
		gameCam->SetDegAng(rot);
	}
}

void BowlingBall::HitFinishArea()
{
	if (!m_isRolling)return;

	m_isRolling = false;
	m_reason = RollEndReason::Finish;
}

void BowlingBall::HitGround(const std::string& matName, float velocityY, JPH::Vec3 pos, JPH::Vec3 normal)
{
	// 接地フラグをオン
	OnGround();

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// velYを1秒あたりに変換
	velocityY /= gameDt;

	// 下方向の強さが一定以上なら着地エフェクトを召喚
	float scale = 1.0f;
	if (velocityY <= BowlingBallConsts::LandingEffectSpawnPowerMin)
	{
		// 拡大倍率を計算
		float scaleMulti = (velocityY - BowlingBallConsts::LandingEffectSpawnPowerMin) * BowlingBallConsts::LandingEffectScaleMulti + BowlingBallConsts::LandingEffectScaleMin;
		scaleMulti = std::min(scaleMulti, BowlingBallConsts::LandingEffectScaleMax);

		// 衝突位置と法線を取得・正規化
		Math::Vector3 hitPos = Math::Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
		Math::Vector3 hitNormal = Math::Vector3(normal.GetX(), normal.GetY(), normal.GetZ());
		hitNormal.Normalize();

		// 1. (0,0,1) から hitNormal 方向への回転クォータニオンを作成
		Math::Quaternion rot = Math::Quaternion::FromToRotation(Math::Vector3(0, 0, 1), hitNormal);

		// 2. クォータニオンをオイラー角(Vector3)に変換
		Math::Vector3 rotAngles = rot.ToEuler();
		// ※ライブラリによっては degrees(度数法) への変換が必要な場合があります

		// エフェクトの召喚（オイラー角を渡す）
		EFFECTMGR.SpawnEffect(hitPos + hitNormal * 0.015f, rotAngles, scale * scaleMulti, EffectType::LandingAndWallHit);

		// 衝突音が鳴る材質なら鳴らす
		if (MaterialNameCheck::IsImpactSound(matName))
		{
			SOUNDMGR.Play(SoundType::SE_WallHitAndLanding, scaleMulti);
		}
	}
}

void BowlingBall::HitWall(const std::string& matName, float velocity, JPH::Vec3 pos, JPH::Vec3 normal)
{
	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// velを1秒あたりに変換
	velocity /= gameDt;

	// 強さが一定以上なら着地エフェクトを召喚
	float scale = 1.0f;
	if (velocity >= BowlingBallConsts::WallHitEffectSpawnPowerMin)
	{
		// 拡大倍率を計算
		float scaleMulti = (velocity - BowlingBallConsts::WallHitEffectSpawnPowerMin) * BowlingBallConsts::WallHitEffectScaleMulti + BowlingBallConsts::WallHitEffectScaleMin;
		scaleMulti = std::min(scaleMulti, BowlingBallConsts::WallHitEffectScaleMax);

		// 衝突位置と法線を取得・正規化
		Math::Vector3 hitPos = Math::Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
		Math::Vector3 hitNormal = Math::Vector3(normal.GetX(), normal.GetY(), normal.GetZ());
		hitNormal.Normalize();

		// 1. (0,0,1) から hitNormal 方向への回転クォータニオンを作成
		Math::Quaternion rot = Math::Quaternion::FromToRotation(Math::Vector3(0, 0, 1), hitNormal);

		// 2. クォータニオンをオイラー角(Vector3)に変換
		Math::Vector3 rotAngles = rot.ToEuler();
		// ※ライブラリによっては degrees(度数法) への変換が必要な場合があります

		// エフェクトの召喚（オイラー角を渡す）
		EFFECTMGR.SpawnEffect(hitPos + hitNormal * 0.015f, rotAngles, scale * scaleMulti, EffectType::LandingAndWallHit);

		// 衝突音が鳴る材質なら鳴らす
		if (MaterialNameCheck::IsImpactSound(matName))
		{
			SOUNDMGR.Play(SoundType::SE_WallHitAndLanding, scaleMulti);
		}
	}
}

void BowlingBall::HitIsland(const std::string& matName, float velocity, JPH::Vec3 pos, JPH::Vec3 normal)
{
	// 島に乗った
	OnIsland();

	// 後は壁と同じ扱い()
	HitWall(matName, velocity, pos, normal);
}

void BowlingBall::OnIsland()
{
	// 仮に地面扱い確定とする
	OnGround();

	// まだフラグが立っていないなら立てる
	if (m_isRolling)
	{
		// 落下判定
		m_isRolling = false;
		m_reason = RollEndReason::Fall;
	}
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

void BowlingBall::UpdateRollingSound()
{
	// 音が無いならリターン
	if (m_soundState == RollingSoundState::None)return;

	// 最初の音の停止確認
	if (m_soundState == RollingSoundState::Throw)
	{
		// 終わっていたら次の再生
		if (m_wpStoredSound.expired())
		{
			m_wpStoredSound = SOUNDMGR.PlayAndGetInst(SoundType::SE_Rolling);
			m_soundState = RollingSoundState::Roll;
		}
	}
	// 転がり更新(前フレーム接地していなかったら一時停止)
	else
	{
		const auto& sound = m_wpStoredSound.lock();
		if (!sound)return;

		if (m_isOnGroundThisFrame)
		{
			// 接地したので再生
			if (sound->IsPause())sound->Resume();
		}
		else
		{
			// 接地しなかったので一時停止
			if (sound->IsPlaying())sound->Pause();
		}
	}
}

float BowlingBall::CalcAvgMoveSpeed()
{
	float moveDist = 0.0f;
	float gdt = 0.0f;

	// 保持しているデータの合計gdtと移動距離を計算
	for (auto data : m_speedStoreData)
	{
		moveDist += data.m_moveDist;
		gdt += data.m_gameDt;
	}

	// moveDistをgdtで割って平均を取得
	float avg = moveDist / gdt;

	// DEBUG
	KdDebugGUI::Instance().AddLog("AvgSpeed : %.1f\n", avg);

	return avg;
}
