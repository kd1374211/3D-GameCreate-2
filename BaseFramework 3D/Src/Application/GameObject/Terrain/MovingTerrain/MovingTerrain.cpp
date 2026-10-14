#include "MovingTerrain.h"
#include "../../../StageManager/StageManager.h"
#include "../../../Const/Function.h"
#include "../../../Scene/SceneManager.h"

MovingTerrain::MovingTerrain(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, MovingParams param)
{
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	m_model = std::make_shared<KdModelData>();
	m_model->Load(param.m_modelPath);
	m_wayPointDotModel = std::make_shared<KdModelData>();
	m_wayPointDotModel->Load("Asset/Models/Terrain/StageGimmicks/WayPoint/WayPointDot.gltf");

	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = quat;
	initData.scale = sca;
	initData.motionType = JPH::EMotionType::Kinematic;
	initData.isStatic = false;
	initData.layer = Layers::TERRAIN;
	initData.friction = MatPhysicsConst::Friction[(size_t)PhysicMaterials::NormalGround];
	initData.restitution = MatPhysicsConst::Restitution[(size_t)PhysicMaterials::NormalGround];
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 壁用
	WallInitData wallData = {};
	wallData.friction = MatPhysicsConst::Friction[(size_t)PhysicMaterials::NormalWall];
	wallData.restitution = MatPhysicsConst::Restitution[(size_t)PhysicMaterials::NormalWall];

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->InitTerrainModel(param.m_modelPath, initData, wallData)) {
		// エラー処理
	}
	
	// 情報設定
	m_pos = pos;
	m_rot = quat;
	m_scale = sca;
	m_param = param;

	// 中継地点の合成
	m_combinedWayPoints.push_back(param.m_startPos);
	for (const auto& point : param.m_waypoints)
	{
		m_combinedWayPoints.push_back(point);
	}
	m_combinedWayPoints.push_back(param.m_goalPos);

	// 中継点関連の値を設定
	m_totalStopPoint = m_combinedWayPoints.size();
	m_currentPointIndex = 0;
	m_distToNextPoint = CalcDistToNextPoint();

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;
}

void MovingTerrain::Update()
{
	// ゲームタイム取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 待機時間中の処理
	if (m_isWait)
	{
		m_waitTime -= gameDt;
		m_cPhysics->SetLinearVelocity(JPH::Vec3::sZero());

		if (m_waitTime <= 0.0f)
		{
			m_isWait = false;
		}
		return;
	}

	// 1. タイマー進行度の更新 (0.0 ～ 1.0)
	m_progress += m_param.m_speed * gameDt / m_distToNextPoint;
	if (m_progress >= 1.0f)
	{
		m_progress = 0.0f;
		// 次の方向に移動
		UpdatePoints();

		m_isWait = true;
		m_waitTime = m_param.m_stopTime;
	}

	// イージングなら進行パターンを調整する
	float fixedProgress = m_progress;
	if (m_param.m_movePattern == MovingParams::MovePattern::Easing)
	{
		// 2. イージング処理 (Ease-InOut-Sine)
		// t: 0.0 -> 1.0 / easedT: 0.0 -> 1.0
		fixedProgress = 0.5f * (1.0f - std::cos(m_progress * M_PI));
	}

	// 3. 現在目標とする位置の算出
	Math::Vector3 targetPos_v = m_combinedWayPoints[m_currentPointIndex] + (CalcVecToNextPoint() * fixedProgress);
	JPH::RVec3 targetPos = JPH::Vec3(targetPos_v.x, targetPos_v.y, targetPos_v.z);

	// 4. Jolt Physics: 現在位置から目標位置へ動かすための速度（LinearVelocity）を計算して設定
	JPH::RVec3 currentPos = m_cPhysics->GetPos();
	JPH::Vec3 requiredVelocity = JPH::Vec3(targetPos - currentPos) / gameDt;

	m_cPhysics->SetLinearVelocity(requiredVelocity);
}

void MovingTerrain::PostUpdate()
{
	// 停止中ならリターン
	if (STAGEMGR.IsEditMode() && STAGEMGR.GetIsGimmickStop())return;

	//  物理座標の同期（Joltから最新座標を反映）
	m_cPhysics->Sync(m_pos, m_rot);

	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	//  ワールド行列の合成（旋回 → 移動)
	m_mWorld = scale * rotat * trans;
}

void MovingTerrain::DrawUnLit()
{
	// デバッグ中でないなら消す
	if (!STAGEMGR.IsEditMode())return;

	// 各ウェイポイント(スタートゴール含む)に点オブジェクトを配置
	for (const auto& points : m_combinedWayPoints)
	{
		Math::Matrix trans = Math::Matrix::CreateTranslation(points);
		Math::Matrix wayPointWorld = trans;

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_wayPointDotModel, wayPointWorld);
	}
}

float MovingTerrain::CalcDistToNextPoint()
{
	// 現在と次の番号を確認
	const int currentIndex = m_currentPointIndex;
	const int nextIndex = (m_currentPointIndex + 1) % m_totalStopPoint;

	return Math::Vector3::Distance(m_combinedWayPoints[currentIndex], m_combinedWayPoints[nextIndex]);
}

Math::Vector3 MovingTerrain::CalcVecToNextPoint()
{
	// 現在と次の番号を確認
	const int currentIndex = m_currentPointIndex;
	const int nextIndex = (m_currentPointIndex + 1) % m_totalStopPoint;

	return m_combinedWayPoints[nextIndex] - m_combinedWayPoints[currentIndex];
}

void MovingTerrain::UpdatePoints()
{
	// インデックス更新
	m_currentPointIndex++;
	m_currentPointIndex %= m_totalStopPoint; // ループした
	m_distToNextPoint = CalcDistToNextPoint();	// 距離の更新
}
	
