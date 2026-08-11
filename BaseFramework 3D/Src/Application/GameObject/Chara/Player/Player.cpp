#include "Player.h"
#include "../../Camera/CameraBase.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../main.h"

#include "../../../StageManager/StageManager.h"

Player::Player(const Math::Vector3& a_startPos, float a_radius)
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

	//本体Init
	Init();
}

void Player::Update()
{
	// エディットモード中はプレイヤーの移動・操作・物理を停止
	if (STAGEMGR.IsEditMode())
	{
		return;
	}

	// =================================================================
	// パラメータ設定（エアライダー風チューニング）
	// =================================================================
	const float BASE_MAX_SPEED = 4.0f;   // 基本の最高速度
	const float ACCEL_POWER = 2.0f;   // 加速度(秒)
	const float DECEL_POWER = 3.0f;   // 最高速を超えた時の減速の滑らかさ(秒)
	const float TURN_SPEED = 120.0f;  // 旋回性能(秒)
	float dt = Application::Instance().GetDeltaTime(); // デルタタイム

	// =================================================================
	// 1. 方向転換（A/Dキーによる向き更新）
	// =================================================================
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) { m_facingAngle -= TURN_SPEED * dt; }
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { m_facingAngle += TURN_SPEED * dt; }

	if (m_facingAngle >= 360.0f)     m_facingAngle -= 360.0f;
	else if (m_facingAngle < 0.0f)   m_facingAngle += 360.0f;


	// =================================================================
	// 2. 「Z+ 基準」の水平正面ベクトル（targetDir）を作成
	// =================================================================
	float rad = DirectX::XMConvertToRadians(m_facingAngle);
	JPH::Vec3 targetDir(std::sin(rad), 0.0f, std::cos(rad));
	targetDir = targetDir.Normalized();


	// =================================================================
	// 3. 接地判定 & 地面の傾き（法線）の取得
	// =================================================================
	// ★ レイの長さを 0.10f -> 0.03f に短縮（小段差でのチカチカ防止）
	float rayLength = m_radius + 0.03f;

	JPH::RVec3 ballPos = m_cPhysics->GetPos(); // 中心位置を取得
	JPH::RRayCast ray{ ballPos, JPH::Vec3(0.0f, -rayLength, 0.0f) };
	JPH::RayCastResult hit;

	// 自分自身を除外するフィルター
	JPH::IgnoreSingleBodyFilter bodyFilter(m_cPhysics->GetBodyID());

	bool rawGrounded = false;
	JPH::Vec3 groundNormal(0.0f, 1.0f, 0.0f);

	if (PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastRay(ray, hit, {}, {}, bodyFilter))
	{
		float hitDistance = hit.mFraction * rayLength;

		if (hitDistance <= m_radius + 0.03f)
		{
			rawGrounded = true;

			// スコープを作って lock の寿命をここで終わらせる
			{
				JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), hit.mBodyID);
				if (lock.Succeeded()) {
					const JPH::Body& body = lock.GetBody();
					groundNormal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, ray.GetPointOnRay(hit.mFraction)).Normalized();
				}
			}
		}
	}

	// チャタリング防止タイマー
	static int groundedGradiantTimer = 0;
	if (rawGrounded) {
		groundedGradiantTimer = 3;
	}
	else if (groundedGradiantTimer > 0) {
		groundedGradiantTimer--;
	}
	bool isGrounded = (groundedGradiantTimer > 0);


	// =================================================================
	// 4. 現在の物理状態の取得
	// =================================================================
	JPH::Vec3 currentVel = m_cPhysics->GetDirection();

	// ★【重要】Joltの壁押し戻し速度による内部速度（m_currentSpeedXZ）の直接上書きを廃止。
	// （減速処理は OnHitWall 側の接触イベントに完全に任せることでガタツキをカット）


	// =================================================================
	// 5. 坂道による可変パラメータの計算
	// =================================================================
	float dynamicMaxSpeed = BASE_MAX_SPEED;
	float dynamicAccel = ACCEL_POWER;

	if (isGrounded)
	{
		JPH::Vec3 moveDirOnSlope = targetDir;
		float dot = targetDir.Dot(groundNormal);
		JPH::Vec3 projected = targetDir - groundNormal * dot;

		if (projected.LengthSq() > 0.0001f) {
			moveDirOnSlope = projected.Normalized();
		}

		float slopeFactor = moveDirOnSlope.GetY();

		// 坂道の影響度をマイルドに計算
		float speedMultiplier = 1.0f - (slopeFactor * 1.2f);
		speedMultiplier = std::clamp(speedMultiplier, 0.6f, 1.5f);

		dynamicMaxSpeed = BASE_MAX_SPEED * speedMultiplier;
		dynamicAccel = ACCEL_POWER * speedMultiplier;
	}


	// =================================================================
	// 6. 加速と進行方向の計算（壁すべり投影・完全対応版）
	// =================================================================

	// 壁接触タイマーの更新（OnHitWallで2がセットされ、毎フレーム減算）
	if (m_wallContactTimer > 0) {
		m_wallContactTimer--;
	}

	// -----------------------------------------------------------------
	// A. スピードの計算（オートアクセル）
	// -----------------------------------------------------------------
	if (isGrounded)
	{
		if (m_currentSpeedXZ < dynamicMaxSpeed)
		{
			m_currentSpeedXZ += dynamicAccel * dt;
			if (m_currentSpeedXZ > dynamicMaxSpeed) m_currentSpeedXZ = dynamicMaxSpeed;
		}
		else if (m_currentSpeedXZ > dynamicMaxSpeed)
		{
			m_currentSpeedXZ -= DECEL_POWER * dt;
			if (m_currentSpeedXZ < dynamicMaxSpeed) m_currentSpeedXZ = dynamicMaxSpeed;
		}
	}

	// -----------------------------------------------------------------
	// B. 進行方向の作成（地面の傾き・坂道に沿わせる）
	// -----------------------------------------------------------------
	JPH::Vec3 moveDir = targetDir;
	if (isGrounded)
	{
		float dot = targetDir.Dot(groundNormal);
		JPH::Vec3 projected = targetDir - groundNormal * dot;
		if (projected.LengthSq() > 0.0001f) {
			moveDir = projected.Normalized();
		}
	}

	// -----------------------------------------------------------------
	// C. 壁衝突時のベクトル投影（★最初の10Fの荒ぶりを消す最重要処理）
	// -----------------------------------------------------------------
	JPH::Vec3 finalMoveDir = moveDir;

	if (m_wallContactTimer > 0)
	{
		// 壁法線（m_lastWallNormal：壁 -> プレイヤー向き）への突撃度を計算
		float dotWall = finalMoveDir.Dot(m_lastWallNormal);

		// 壁の中に突き進もうとしている場合（dot < 0）のみ、壁と平行な成分だけに投影する
		if (dotWall < 0.0f)
		{
			// 壁に突き刺さる成分を除去し、壁と完全に平行な滑りベクトルを作成
			finalMoveDir = finalMoveDir - m_lastWallNormal * dotWall;

			if (finalMoveDir.LengthSq() > 0.0001f) {
				finalMoveDir = finalMoveDir.Normalized();
			}
			else {
				// 正面完全衝突時はその場に留まるベクトル（ゼロ）
				finalMoveDir = JPH::Vec3::sZero();
			}
		}
	}

	// -----------------------------------------------------------------
	// D. 最終的な目標速度ベクトルの生成
	// -----------------------------------------------------------------
	JPH::Vec3 newVel = finalMoveDir * m_currentSpeedXZ;

	// =================================================================
	// 7. 物理エンジン（Jolt）への反映（SetLinearVelocity）
	// =================================================================
	JPH::Vec3 finalVel = newVel;

	if (isGrounded)
	{
		// 接地中：上向き速度（Y > 0）をシャットアウト（0以下にリセット）し、
		// 地面への吸いつきと跳ね防止を両立させる
		float currentY = currentVel.GetY();
		finalVel.SetY(std::min(currentY, 0.0f));
	}
	else
	{
		// 空中：Jolt の重力・落下速度をそのまま維持
		finalVel.SetY(currentVel.GetY());
	}

	// 速度と回転の書き換え
	m_cPhysics->SetLinearVelocity(finalVel);
	m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());

	//カメラに設定
	if (m_wpCamera.expired())return;

	m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_facingAngle)));
	KdDebugGUI::Instance().AddLog("Facing Angle : %.2f\n", m_facingAngle);
	KdDebugGUI::Instance().AddLog("IsGround : %d\n", isGrounded == true ? 1 : 0);
	KdDebugGUI::Instance().AddLog("IsHitWall : %d\n", m_wallContactTimer > 0 ? 1 : 0);
	KdDebugGUI::Instance().AddLog("Current Speed : %.2f\n", m_currentSpeedXZ);
	KdDebugGUI::Instance().AddLog("DeltaTime : %.3f\n", Application::Instance().GetDeltaTime());
}

void Player::PostUpdate()
{
	float dt = Application::Instance().GetDeltaTime(); // デルタタイム

	// 1. 物理座標の同期（Joltから最新座標を反映）
	m_cPhysics->Sync(m_pos);

	// 2. 最新座標を取得
	JPH::RVec3 ballPos = m_cPhysics->GetPos();

	// 3. 移動距離に応じた「転がり角度」の加算更新
	if (m_radius > 0.001f)
	{
		m_rollAngle += (m_currentSpeedXZ * dt) / m_radius;

		// 360度（2π）超えのオーバーフロー防止
		if (m_rollAngle >= M_PI * 2.0f) {
			m_rollAngle -= M_PI * 2.0f;
		}
	}

	// 4. Math::Matrix を使って行列を作成
	// ※度数法 -> 弧度法（ラジアン）変換 ( m_facingAngle * (π / 180.0f) )
	float yawRad = m_facingAngle * static_cast<float>(M_PI / 180.0f);

	// A. 転がり回転（X軸）
	Math::Matrix matRoll = Math::Matrix::CreateRotationX(m_rollAngle);

	// B. 進行方向への旋回（Y軸）
	Math::Matrix matYaw = Math::Matrix::CreateRotationY(yawRad);

	// C. 位置（Translation）
	Math::Matrix matTrans = Math::Matrix::CreateTranslation(
		m_pos
	);

	// D. ワールド行列の合成（ 転がり → 旋回 → 移動  )
	// ※ご使用の環境の行列乗算順序に合わせて「*」の順序を調整してください
	m_mWorld = matRoll * matYaw * matTrans;
}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::OnHitWall(const JPH::Vec3& wallNormal)
{
	JPH::Vec3 horizNormal(wallNormal.GetX(), 0.0f, wallNormal.GetZ());
	if (horizNormal.LengthSq() < 0.0001f) return;

	// 壁法線を保存（壁からプレイヤーに向かう向き）
	m_lastWallNormal = horizNormal.Normalized();
	m_wallContactTimer = 2; // 2フレーム接触状態を保持

	// --- 減速処理（dt考慮版） ---
	float rad = DirectX::XMConvertToRadians(m_facingAngle);
	JPH::Vec3 moveDir(std::sin(rad), 0.0f, std::cos(rad));
	if (moveDir.LengthSq() < 0.0001f) return;
	moveDir = moveDir.Normalized();

	float wallDot = -moveDir.Dot(m_lastWallNormal);
	if (wallDot > 0.0f)
	{
		float dt = Application::Instance().GetDeltaTime();
		m_currentSpeedXZ -= 5.0f * wallDot * dt;
		if (m_currentSpeedXZ < 0.0f) m_currentSpeedXZ = 0.0f;
	}
}

void Player::Init()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/PlayerBall/bowling_ball.gltf");

	//デバッグ
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}
