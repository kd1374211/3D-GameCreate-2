#include "Player.h"
#include "../../Camera/CameraBase.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../main.h"

Player::Player(const Math::Vector3& a_startPos, float a_radius)
{
	//物理Init
	m_cPhysics = std::make_shared<PhysicsComponent>();
	m_cPhysics->Init(a_startPos, a_radius, false);
	m_cPhysics->SetFriction(0.0f);
	m_cPhysics->SetRestitution(0.0f);
	m_cPhysics->SetDamping(1.0f, 1.0f);

	//半径保存
	m_radius = a_radius;

	//本体Init
	Init();
}

void Player::Update()
{
	// =================================================================
	// パラメータ設定（エアライダー風チューニング）
	// =================================================================
	const float BASE_MAX_SPEED = 4.0f;   // 基本の最高速度
	const float ACCEL_POWER = 2.0f;   // 加速度(秒)
	const float DECEL_POWER = 3.0f;   // 最高速を超えた時の減速の滑らかさ(秒)
	const float TURN_SPEED = 120.0f;   // 旋回性能(秒)
	float dt = Application::Instance().GetDeltaTime(); //デルタタイム

	bool isAccelPressed = (GetAsyncKeyState('W') & 0x8000) != 0;


	// =================================================================
	// 1. 方向転換（A/Dキーによる向き更新）
	// =================================================================
	if (GetAsyncKeyState('A') & 0x8000) { m_facingAngle -= TURN_SPEED * dt; }
	if (GetAsyncKeyState('D') & 0x8000) { m_facingAngle += TURN_SPEED * dt; }

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
	float rayLength = m_radius + 0.10f; // 中心から足元＋10cmまでレイを伸ばす

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
		// 中心から見て足元付近で当たっているか確認
		if (hitDistance <= m_radius + 0.08f)
		{
			rawGrounded = true;

			JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), hit.mBodyID);
			if (lock.Succeeded()) {
				const JPH::Body& body = lock.GetBody();
				groundNormal = body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, ray.GetPointOnRay(hit.mFraction)).Normalized();
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
	JPH::Vec3 currentVelXZ(currentVel.GetX(), 0.0f, currentVel.GetZ());
	float joltSpeedXZ = currentVelXZ.Length(); // Joltが計算した現実の速度

	// 内部で管理している速度をベースにする
	float currentSpeedXZ = m_currentSpeedXZ;

	// ★【重要】正面衝突などで Jolt 側で物理的にガツンと止められた場合だけ、内部速度を同期して下げる
	// (壁擦りや坂道の微小な引っかかりでスピードが殺されるのを防ぐ)
	if (joltSpeedXZ < currentSpeedXZ * 0.5f) {
		currentSpeedXZ = joltSpeedXZ;
	}


	// =================================================================
	// 5. 坂道による可変パラメータの計算（減速感をマイルドに調整）
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

		// ★ 坂道の影響度をマイルドに修正 (1.2f 程度に下げる)
		float speedMultiplier = 1.0f - (slopeFactor * 1.2f);

		// 最小でも 0.6 倍は保証（登り坂で極端に遅くならないようにする）
		speedMultiplier = std::clamp(speedMultiplier, 0.6f, 1.5f);

		dynamicMaxSpeed = BASE_MAX_SPEED * speedMultiplier;
		dynamicAccel = ACCEL_POWER * speedMultiplier;
	}


	// =================================================================
	// 6. 加速と旋回処理
	// =================================================================

	// --- A. スピードの計算 ---
	if (isGrounded)
	{
		// 自分の内部速度から素直に ACCEL_POWER で加速する
		if (currentSpeedXZ < dynamicMaxSpeed)
		{
			currentSpeedXZ += dynamicAccel * dt;
			if (currentSpeedXZ > dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
		}
		else if (currentSpeedXZ > dynamicMaxSpeed)
		{
			currentSpeedXZ -= DECEL_POWER * dt;
			if (currentSpeedXZ < dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
		}
	}

	// --- B. 進行方向の作成と旋回 ---
	JPH::Vec3 moveDir = targetDir;
	if (isGrounded)
	{
		float dot = targetDir.Dot(groundNormal);
		JPH::Vec3 projected = targetDir - groundNormal * dot;
		if (projected.LengthSq() > 0.0001f) {
			moveDir = projected.Normalized();
		}
	}

	// 基礎となる速度ベクトル
	JPH::Vec3 newVel = moveDir * currentSpeedXZ;

	// 旋回（レスポンス補元）
	JPH::Vec3 currentDir = (currentVelXZ.LengthSq() > 0.0001f) ? currentVelXZ.Normalized() : moveDir;
	float turnResponse = std::min(TURN_SPEED * 2.5f * dt, 1.0f);
	JPH::Vec3 blendedDir = currentDir + (moveDir - currentDir) * turnResponse;

	if (blendedDir.LengthSq() > 0.0001f) {
		newVel = blendedDir.Normalized() * currentSpeedXZ;
	}

	// =================================================================
	// 7. 物理エンジン（Jolt）への反映
	// =================================================================
	if (!isGrounded) {
		newVel.SetY(currentVel.GetY());
	}

	// Joltに速度をセット（壁に突っ込むとJoltが勝手に滑らせて速度を削ってくれる）
	m_cPhysics->SetLinearVelocity(newVel);
	m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());

	// 内部変数に保存
	m_currentSpeedXZ = currentSpeedXZ;

//	// =================================================================
//	// 4. 現在の物理状態の取得
//	// =================================================================
//	JPH::Vec3 currentVel = m_cPhysics->GetDirection();
//	JPH::Vec3 currentVelXZ(currentVel.GetX(), 0.0f, currentVel.GetZ());
//
//	// ★ 速度の「大きさ」は物理から拾わず、内部変数を使う（物理の微小な減衰で最高速がブレるのを防ぐ）
//	float currentSpeedXZ = m_currentSpeedXZ;
//
//	// 向きのベクトルだけ物理から取得
//	JPH::Vec3 currentDir = (currentVelXZ.LengthSq() > 0.0001f) ? currentVelXZ.Normalized() : targetDir;
//
//	// =================================================================
//// 5. 坂道による可変パラメータの計算（moveDirのY成分直接判定版）
//// =================================================================
//	float dynamicMaxSpeed = BASE_MAX_SPEED;
//	float dynamicAccel = ACCEL_POWER;
//	float slopeFactor = 0.0f;
//
//	if (isGrounded)
//	{
//		// 地面の傾きに沿わせた進行方向ベクトルをまず作成
//		JPH::Vec3 moveDirOnSlope = targetDir;
//		float dot = targetDir.Dot(groundNormal);
//		JPH::Vec3 projected = targetDir - groundNormal * dot;
//
//		if (projected.LengthSq() > 0.0001f) {
//			moveDirOnSlope = projected.Normalized();
//		}
//
//		// ★ 進行ベクトルの Y 成分を直接見る！
//		// 上り坂: moveDirOnSlope.GetY() > 0 (プラス)
//		// 下り坂: moveDirOnSlope.GetY() < 0 (マイナス)
//		slopeFactor = moveDirOnSlope.GetY();
//
//		// 上り坂(slopeFactor > 0) ➔ 1.0 - (プラス) ＝ 減速
//		// 下り坂(slopeFactor < 0) ➔ 1.0 - (マイナス) ＝ 加速
//		float speedMultiplier = 1.0f - (slopeFactor * 2.0f); // 影響度を2.0fでわかりやすく調整
//
//		// 速度倍率を 0.3倍 〜 1.8倍 の範囲に収める
//		speedMultiplier = std::clamp(speedMultiplier, 0.3f, 1.8f);
//
//		dynamicMaxSpeed = BASE_MAX_SPEED * speedMultiplier;
//		dynamicAccel = ACCEL_POWER * speedMultiplier;
//	}
//
//	// =================================================================
//	// 6. 加速・旋回・坂道・壁判定処理（オート前進・新仕様版）
//	// =================================================================
//
//	// -----------------------------------------------------------------
//	// A. スピード（currentSpeedXZ）の計算（坂道の影響のみを受ける）
//	// -----------------------------------------------------------------
//	if (isGrounded)
//	{
//		// 基本の目標速度（BASE_MAX_SPEED）に向けて加速しつつ、
//		// 坂道（slopeFactor）の影響で最高速と加速度がリアルタイムに変化する
//		if (currentSpeedXZ < dynamicMaxSpeed)
//		{
//			currentSpeedXZ += dynamicAccel * dt;
//			if (currentSpeedXZ > dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
//		}
//		else if (currentSpeedXZ > dynamicMaxSpeed)
//		{
//			// 下り坂から平地に戻った時など、オーバーしたスピードを自然に減速
//			currentSpeedXZ -= DECEL_POWER * dt;
//			if (currentSpeedXZ < dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
//		}
//	}
//
//	// -----------------------------------------------------------------
//	// B. 進行方向（moveDir）の算出と旋回（ターニング）処理
//	// -----------------------------------------------------------------
//	JPH::Vec3 moveDir = targetDir;
//
//	if (isGrounded)
//	{
//		float dot = targetDir.Dot(groundNormal);
//		JPH::Vec3 projected = targetDir - groundNormal * dot;
//		if (projected.LengthSq() > 0.0001f) {
//			moveDir = projected.Normalized();
//		}
//	}
//
//	// 旋回（現在の進行方向から目標方向へ向ける）
//	float turnResponse = std::min(TURN_SPEED * 2.5f * dt, 1.0f);
//	JPH::Vec3 blendedDir = currentDir + (moveDir - currentDir) * turnResponse;
//
//	if (blendedDir.LengthSq() > 0.0001f) {
//		blendedDir = blendedDir.Normalized();
//	}
//	else {
//		blendedDir = moveDir;
//	}
//
//	// ★ 決定した「向き」に「内部管理の速度」を掛けて速度ベクトルを作成
//	JPH::Vec3 newVel = blendedDir * currentSpeedXZ;
//
//	// =================================================================
//	// C. 垂直な壁の判定と滑り・離脱補正（手前判定防止＆比例減速版）
//	// =================================================================
//	JPH::SphereShapeSettings sphereSettings(m_radius);
//	JPH::Shape::ShapeResult sphereResult = sphereSettings.Create();
//
//	if (sphereResult.IsValid())
//	{
//		JPH::RefConst<JPH::Shape> sphereShape = sphereResult.Get();
//
//		JPH::Vec3 checkDir = targetDir;
//		if (checkDir.LengthSq() < 0.0001f) checkDir = moveDir;
//
//		// ★修正1: 判定距離を 0.15f -> 0.03f に短縮（手前での誤検知を防止）
//		JPH::RShapeCast shapeCast(
//			sphereShape.GetPtr(),
//			JPH::Vec3::sReplicate(1.0f),
//			JPH::RMat44::sTranslation(ballPos),
//			checkDir * 0.03f                    // 見た目通りのギリギリで反応させる
//		);
//
//		JPH::ShapeCastSettings castSettings;
//		JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;
//
//		PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastShape(shapeCast, castSettings, ballPos, collector, {}, {}, bodyFilter);
//
//		if (collector.HadHit())
//		{
//			const JPH::ShapeCastResult& castHit = collector.mHit;
//
//			JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), castHit.mBodyID2);
//			if (lock.Succeeded())
//			{
//				const JPH::Body& body = lock.GetBody();
//				JPH::Vec3 wallNormal = -castHit.mPenetrationAxis.Normalized();
//
//				// 垂直な壁（Y成分 < 0.3）のみ判定
//				if (abs(wallNormal.GetY()) < 0.3f)
//				{
//					float targetDot = targetDir.Dot(wallNormal);
//
//					if (targetDot < -0.01f)
//					{
//						float impactFactor = abs(targetDot); // 1.0 = 正面衝突, 0.0 = 完全な平行
//
//						// ★ 1. 乗算（*=）ではなく、衝突角度に応じた「目標速度」を計算する
//						// 正面衝突(1.0)なら目標速度 0.0、浅い擦り(0.2)なら目標速度は最高速の 80%
//						float targetSpeedOnWall = dynamicMaxSpeed * (1.0f - impactFactor);
//
//						// ★ 2. 現在の速度が目標速度より高い場合、減速パワー（DECEL_POWER）に沿って「徐々に」落とす
//						// これにより、毎フレーム一瞬で 0 になる現象を完全に防ぐ！
//						if (currentSpeedXZ > targetSpeedOnWall)
//						{
//							// 衝突角度が急なほど強くブレーキがかかる
//							float brakePower = DECEL_POWER * (1.0f + impactFactor * 3.0f);
//							currentSpeedXZ -= brakePower * dt;
//
//							if (currentSpeedXZ < targetSpeedOnWall) {
//								currentSpeedXZ = targetSpeedOnWall;
//							}
//						}
//
//						// ★ 3. 浅い角度（画像のような擦り）の場合は、絶対に完全停止させない最低保障速度をつくる
//						if (impactFactor < 0.6f && currentSpeedXZ < 1.0f)
//						{
//							currentSpeedXZ = 1.0f; // 1.0 m/s は最低維持して進ませる
//						}
//
//						// 4. 壁に沿う平行方向ベクトルを作成
//						JPH::Vec3 wallParallelDir = targetDir - wallNormal * targetDot;
//
//						if (wallParallelDir.LengthSq() > 0.0001f)
//						{
//							// 減速された速度で壁沿いに進む
//							newVel = wallParallelDir.Normalized() * currentSpeedXZ;
//						}
//						else
//						{
//							newVel = targetDir * currentSpeedXZ;
//						}
//
//						// 内部速度変数を更新
//						m_currentSpeedXZ = currentSpeedXZ;
//					}
//				}
//			}
//		}
//	}

	// -----------------------------------------------------------------
	// D. 物理エンジン（Jolt）への反映
	// -----------------------------------------------------------------
	if (!isGrounded) {
		// 空中時は物理エンジンの落下Y速度（重力）を優先
		newVel.SetY(currentVel.GetY());
	}

	// 移動速度を適用（常に一定以上のスピードで動き続けるため非常に安定します）
	m_cPhysics->SetLinearVelocity(newVel);

	// 物理的な角速度は0に固定（横暴走を完璧に防止）
	m_cPhysics->SetAngularVelocity(JPH::Vec3::sZero());

	//速度記録
	m_currentSpeedXZ = currentSpeedXZ;

	//カメラに設定
	if (m_wpCamera.expired())return;

	m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_facingAngle)));
	KdDebugGUI::Instance().AddLog("Facing Angle : %.2f\n", m_facingAngle);
	KdDebugGUI::Instance().AddLog("IsGround : %d\n", isGrounded == true ? 1 : 0);
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
		(float)ballPos.GetX(),
		(float)ballPos.GetY(),
		(float)ballPos.GetZ()
	);

	// D. ワールド行列の合成（ 転がり → 旋回 → 移動 ）
	// ※ご使用の環境の行列乗算順序に合わせて「*」の順序を調整してください
	m_mWorld = matRoll * matYaw * matTrans;
}

void Player::Init()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/PlayerBall/bowling_ball.gltf");

	//デバッグ
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}
