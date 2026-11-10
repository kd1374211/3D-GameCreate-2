#include "RotatingTerrain.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../Const/Function.h"
#include "../../../../Scene/SceneManager.h"

RotatingTerrain::RotatingTerrain(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, RotatingParams param)
{
	Math::Quaternion quat = ConvertEulerVec3ToQuat(rot);

	m_model = KdAssets::Instance().m_modeldatas.GetData(param.m_modelPath);
	m_rotatePivotDotModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Terrain/StageGimmicks/WayPoint/WayPointDot.gltf");

	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = quat;
	initData.scale = sca;
	initData.rotPivotOffset = param.m_rotPivotPosOfs;
	initData.motionType = JPH::EMotionType::Kinematic;
	initData.isStatic = false;
	initData.layer = Layers::TERRAIN;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	// 🚀 ファイルパスを渡すだけで、ロードから Jolt への地形登録まで完結！
	if (!m_cPhysics->InitTerrainModel(param.m_modelPath, initData)) {
		// エラー処理
	}

	// 回転を加える
	m_cPhysics->SetAngularVelocity(JPH::Vec3(0, DirectX::XMConvertToRadians(param.m_rotateSpeed), 0));

	// 回転軸位置を取得
	Math::Vector3 modelCenter = m_model->FindNode("ModelCenterPoint")->m_worldTransform.Translation();
	m_rotatePivotDotPos = pos + modelCenter + param.m_rotPivotPosOfs;

	// 回転モードを取得
	m_isBounceMode = param.m_rotateMode == RotatingParams::RotateMode::Bounce;
	// バウンスなら追加情報を取得
	if (m_isBounceMode)
	{
		// バウンド角度取得
		m_bounceDeg = param.m_bounceDeg;

		// プログレスの進行スピードを計算
		m_progSpeed = param.m_rotateSpeed / m_bounceDeg;
	}

	// 情報設定
	m_pos = pos;
	m_rot = quat;
	m_scale = sca;

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;

	// カメラに登録
	RegistToCamera();
}

void RotatingTerrain::Update()
{
	// 円モードならスキップ
	if (!m_isBounceMode)return;

	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 進行前のプログレスを保持
	float prevProg = m_bounceProg;

	// プログレス進行
	m_bounceProg += gameDt * m_progSpeed * m_progSpeedMulti;

	// 1.0を超えている・0.0を下回っている分プログレスを折り返す
	while (m_bounceProg > 1.0f || m_bounceProg < 0.0f)
	{
		// 超過分を確認
		float overRotate = IsReverse() ? fabs(m_bounceProg) : (m_bounceProg - 1.0f);

		// プログレス進行を反転
		ProgReverse();

		// 超過分を折り返し
		m_bounceProg += overRotate * m_progSpeedMulti;
	}

	// 現在の回転との差を確認して回転速度を設定
	m_cPhysics->SetAngularVelocity(JPH::Vec3(0, DirectX::XMConvertToRadians((m_bounceDeg * (m_bounceProg - prevProg)) / gameDt), 0));

	KdDebugGUI::Instance().AddLog("Bounce Prog : %.2f\n", m_bounceProg);
}

void RotatingTerrain::PostUpdate()
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

void RotatingTerrain::DrawUnLit()
{
	// デバッグ中でないなら消す
	if (!STAGEMGR.IsEditMode())return;

	// 回転軸に点オブジェクトを配置
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_rotatePivotDotPos);
	Math::Matrix wayPointWorld = trans;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_rotatePivotDotModel, wayPointWorld);
}
