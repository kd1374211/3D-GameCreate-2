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
	m_cPhysics->SetFriction(0.8f);
	m_cPhysics->SetDamping(0.01f, 0.01f);

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
	const float BASE_MAX_SPEED = 5.0f;   // 基本の最高速度
	const float ACCEL_POWER = 8.0f;   // 加速度
	const float DECEL_POWER = 3.0f;   // 最高速を超えた時の減速の滑らかさ
	const float TURN_SPEED = 8.0f;   // 旋回性能
	float dt = Application::Instance().GetDeltaTime(); //デルタタイム

	bool isAccelPressed = (GetAsyncKeyState('W') & 0x8000) != 0;


	// =================================================================
	// 1. 方向転換（A/Dキーによる向き更新）
	// =================================================================
	if (GetAsyncKeyState('A') & 0x8000) { m_facingAngle -= TURN_SPEED * 30.0f * dt; }
	if (GetAsyncKeyState('D') & 0x8000) { m_facingAngle += TURN_SPEED * 30.0f * dt; }

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
	float currentSpeedXZ = currentVelXZ.Length();


	// =================================================================
// 5. 坂道による可変パラメータの計算（符号修正版）
// =================================================================
	float dynamicMaxSpeed = BASE_MAX_SPEED;
	float dynamicAccel = ACCEL_POWER;
	float slopeFactor = 0.0f;

	if (isGrounded)
	{
		// ★修正：符号を正しく設定
		// targetDir と groundNormal の内積（登りなら targetDir が上を向くので正、下りなら負）
		// slopeFactor > 0 : 登り坂（減速・重力で戻される方向）
		// slopeFactor < 0 : 下り坂（加速・滑り落ちる方向）
		slopeFactor = targetDir.Dot(groundNormal);

		// 下り坂（slopeFactor < 0）なら最高速・加速度アップ
		// 登り坂（slopeFactor > 0）なら最高速・加速度ダウン
		float speedMultiplier = 1.0f - slopeFactor * 1.2f;
		speedMultiplier = std::clamp(speedMultiplier, 0.4f, 1.6f);

		dynamicMaxSpeed = BASE_MAX_SPEED * speedMultiplier;
		dynamicAccel = ACCEL_POWER * speedMultiplier;
	}

	// =================================================================
	// 6. 加速・旋回・坂道・壁判定処理（オート前進・新仕様版）
	// =================================================================

	// -----------------------------------------------------------------
	// A. スピード（currentSpeedXZ）の計算（坂道の影響のみを受ける）
	// -----------------------------------------------------------------
	if (isGrounded)
	{
		// 基本の目標速度（BASE_MAX_SPEED）に向けて加速しつつ、
		// 坂道（slopeFactor）の影響で最高速と加速度がリアルタイムに変化する
		if (currentSpeedXZ < dynamicMaxSpeed)
		{
			currentSpeedXZ += dynamicAccel * dt;
			if (currentSpeedXZ > dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
		}
		else if (currentSpeedXZ > dynamicMaxSpeed)
		{
			// 下り坂から平地に戻った時など、オーバーしたスピードを自然に減速
			currentSpeedXZ -= DECEL_POWER * dt;
			if (currentSpeedXZ < dynamicMaxSpeed) currentSpeedXZ = dynamicMaxSpeed;
		}
	}

	// -----------------------------------------------------------------
	// B. 進行方向（moveDir）の算出と旋回（ターニング）処理
	// -----------------------------------------------------------------
	JPH::Vec3 moveDir = targetDir;

	if (isGrounded)
	{
		// 地面の法線（groundNormal）に対する正射影で、坂道の傾斜に完全に沿ったベクトルを作成
		float dot = targetDir.Dot(groundNormal);
		JPH::Vec3 projected = targetDir - groundNormal * dot;
		if (projected.LengthSq() > 0.0001f) {
			moveDir = projected.Normalized();
		}
	}

	// 基礎となる速度ベクトル（坂道の傾きを含む）
	JPH::Vec3 newVel = moveDir * currentSpeedXZ;

	// 常に移動しているため、入力方向（moveDir）へ滑らかに旋回
	JPH::Vec3 currentDir = (currentVelXZ.LengthSq() > 0.0001f) ? currentVelXZ.Normalized() : moveDir;
	float turnResponse = std::min(TURN_SPEED * 2.5f * dt, 1.0f);
	JPH::Vec3 blendedDir = currentDir + (moveDir - currentDir) * turnResponse;

	if (blendedDir.LengthSq() > 0.0001f) {
		newVel = blendedDir.Normalized() * currentSpeedXZ;
	}

	// -----------------------------------------------------------------
	// C. 垂直な壁の判定と滑り・離脱補正（ShapeCastResult / JPH::完全指定版）
	// -----------------------------------------------------------------
	JPH::SphereShapeSettings sphereSettings(m_radius);
	JPH::Shape::ShapeResult sphereResult = sphereSettings.Create();

	if (sphereResult.IsValid())
	{
		JPH::RefConst<JPH::Shape> sphereShape = sphereResult.Get();

		JPH::Vec3 checkDir = targetDir;
		if (checkDir.LengthSq() < 0.0001f) checkDir = moveDir;

		// 1. スフィアキャストの構築
		JPH::RShapeCast shapeCast(
			sphereShape.GetPtr(),               // シェイプポインタ
			JPH::Vec3::sReplicate(1.0f),       // スケール
			JPH::RMat44::sTranslation(ballPos), // 開始位置
			checkDir * 0.15f                    // キャスト方向と距離
		);

		JPH::ShapeCastSettings castSettings;

		// 2. Jolt標準の「最も近い1ヒットだけを集める」コレクターを使用
		// ※ テンプレート引数は JPH::ShapeCastCollector になります
		JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;

		// 3. Sweep判定（CastShape）を実行
		PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastShape(shapeCast, castSettings, ballPos, collector, {}, {}, bodyFilter);

		// 4. 衝突があったか確認
		if (collector.HadHit())
		{
			// 衝突結果（JPH::ShapeCastResult）を取り出す
			const JPH::ShapeCastResult& castHit = collector.mHit;

			JPH::BodyLockRead lock(PHYSICSMGR.GetSystem().GetBodyLockInterface(), castHit.mBodyID2);
			if (lock.Succeeded())
			{
				const JPH::Body& body = lock.GetBody();
				// 接触した面の法線を正確に取得
				JPH::Vec3 wallNormal = -castHit.mPenetrationAxis.Normalized();

				// 垂直な壁（Y成分 < 0.3）のみ判定対象
				if (abs(wallNormal.GetY()) < 0.3f)
				{
					// プレイヤーの指向方向（targetDir）と壁の法線の内積
					float targetDot = targetDir.Dot(wallNormal);

					if (targetDot < -0.05f)
					{
						// 衝突角度（1.0 = 正面衝突, 0.0 = 平行）
						float impactFactor = abs(targetDot);

						// ★ 1. 正面衝突（impactFactor > 0.7 程度）の場合は「壁沿いに進ませない」
						if (impactFactor > 0.7f)
						{
							// 完全〜大幅減速（ブレーキ）
							float newSpeed = currentSpeedXZ * 0.1f; // 速度を10%まで落とす

							// 速度が小さすぎる場合はピタッと止める（変な微小慣性を消去）
							if (newSpeed < 0.5f) {
								newSpeed = 0.0f;
							}

							// 壁沿いへの投影を行わず、速度を減衰させるだけにする
							// これにより「壁に突き刺さった状態」になり、次に右や手前を向いてアクセルを踏んだ瞬間にすぐ脱出できる！
							newVel = targetDir * newSpeed;
						}
						else
						{
							// ★ 2. 浅い角度（擦り抜け）の場合は、今まで通り滑らかに壁沿いに滑らせる
							JPH::Vec3 wallParallelDir = targetDir - wallNormal * targetDot;

							if (wallParallelDir.LengthSq() > 0.0001f)
							{
								// 浅い衝突に応じた軽い減速 (例: 80%〜95% 保持)
								float speedDamping = 1.0f - (impactFactor * 0.3f);
								float newSpeed = currentSpeedXZ * speedDamping;

								newVel = wallParallelDir.Normalized() * newSpeed;
							}
						}
					}
				}
			}
		}
	}

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
