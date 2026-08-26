#include "FinishArea.h"
#include "../../../StageManager/StageManager.h"
#include "../../../Scene/SceneManager.h"

FinishArea::FinishArea(Math::Vector3 pos, Math::Quaternion rot, Math::Vector3 size)
{
	m_cPhysics = std::make_shared<PhysicsComponent>();

	//物理Initに投げるパラメータ設定
	PhysicsInitData initData = {};
	initData.pos = pos;
	initData.rot = Math::Quaternion::Identity;
	initData.motionType = JPH::EMotionType::Static;
	initData.isStatic = true;
	initData.isSensor = true;
	initData.layer = Layers::FINISHAREA;
	initData.userData = reinterpret_cast<JPH::uint64>(this);

	m_cPhysics->Init(size * 0.5f, initData);

	// 情報設定
	m_pos = pos;
	m_rot = rot;
	m_scale = size;

	Init();
}

void FinishArea::Init()
{
	//トレイル初期化
	m_polygon = std::make_shared<KdSquarePolygon>();
	m_polygon->SetMaterial("Asset/Textures/Goal/GoalLine.png");

	// 移動チェックポイント設定
	m_trailCheckPoint[0] = Math::Vector3(m_pos.x + m_scale.x / 2.0f, m_pos.y, m_pos.z + m_scale.z / 2.0f);
	m_trailCheckPoint[1] = Math::Vector3(m_pos.x - m_scale.x / 2.0f, m_pos.y, m_pos.z + m_scale.z / 2.0f);
	m_trailCheckPoint[2] = Math::Vector3(m_pos.x - m_scale.x / 2.0f, m_pos.y, m_pos.z - m_scale.z / 2.0f);
	m_trailCheckPoint[3] = Math::Vector3(m_pos.x + m_scale.x / 2.0f, m_pos.y, m_pos.z - m_scale.z / 2.0f);
	m_lineLength = m_scale.x * 2 + m_scale.z * 2;

	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	m_mWorld = scale * rotat * trans;

	// 各面のマトリックス
	for (int i = 0; i < FinishAreaConsts::TrailCPCount; i++)
	{
		Math::Vector3 start = m_trailCheckPoint[i];
		Math::Vector3 end = m_trailCheckPoint[(i + 1) % FinishAreaConsts::TrailCPCount];
		float length = Math::Vector3::Distance(start, end);

		trans = Math::Matrix::CreateTranslation((m_trailCheckPoint[i] + m_trailCheckPoint[(i + 1) % 4]) / 2.0f);
		rotat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(i * -90.0f));
		m_trailFaceMat[i] = scale * rotat * trans;

		m_trailFaceProgress[i] = length / m_lineLength;
	}
}

void FinishArea::Update()
{
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 移動進捗追加
	m_moveProgress += FinishAreaConsts::TrailMoveSpeed * gameDt / m_lineLength;

	// ループ用
	if (m_moveProgress >= 1.0f)m_moveProgress -= 1.0f;
}

void FinishArea::DrawUnLit()
{
	// 初期位置
	float trailStart = CalcTrailStart();

	KdDebugGUI::Instance().AddLog("TrailStart : %.2f\n", trailStart);

	// 画像UV縦
	Math::Vector2 totalTexSize = Math::Vector2(m_lineLength, m_scale.y) / FinishAreaConsts::TexLoopSize;
	totalTexSize *= FinishAreaConsts::GoalLineTexSize;
	
	// rectスタート地点
	Math::Vector2 uvRect = Math::Vector2(totalTexSize.x * (1.0f - trailStart), 0.0f);
	
	// 4回ループ
	for (int i = 0; i < FinishAreaConsts::TrailCPCount; i++)
	{
		// ループ用
		float deci = uvRect.x - (int)uvRect.x;
		uvRect.x = (int)uvRect.x % (int)FinishAreaConsts::GoalLineTexSize;
		uvRect.x += deci;

		// 画像Rect
		Math::Rectangle rec = Math::Rectangle(uvRect.x, uvRect.y, (long)(totalTexSize.x * m_trailFaceProgress[i]), totalTexSize.y);
		m_polygon->SetUVRect(rec);

		// 描画(Wrapバージョン使用)
		KdShaderManager::Instance().m_StandardShader.DrawWrapPolygon(*m_polygon, m_trailFaceMat[i]);

		// rectスタートを進める
		uvRect += Math::Vector2(totalTexSize.x * m_trailFaceProgress[i], 0.0f);
	}

	// 置いておく
	//KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZEnable);
}

float FinishArea::CalcTrailStart()
{
	// トレイルスタート位置生成
	float totalMoveDist = m_moveProgress * m_lineLength;
	float currentMoveDist = 0.0f;
	float trailStartPoint = 0.0f;
	float currentMoveProg = 0.0f;
	
	for (int i = 0; i < FinishAreaConsts::TrailCPCount; i++)
	{
		Math::Vector3 lastPoint = m_trailCheckPoint[i];
		Math::Vector3 nextPoint = m_trailCheckPoint[(i + 1) % 4];
		
		//次への移動距離を求める
		float nextPointDist = (lastPoint - nextPoint).Length();

		//総移動距離が次のCPとの間にあるか
		if (totalMoveDist < currentMoveDist + nextPointDist)
		{
			float moveDistLeft = totalMoveDist - currentMoveDist;

			// あれば現在の点から総移動距離-この点までの移動距離の位置に基準点を作成
			trailStartPoint = currentMoveProg + moveDistLeft / nextPointDist * m_trailFaceProgress[i];

			return trailStartPoint;
		}

		// 無ければ次の点に
		currentMoveDist += nextPointDist;
		currentMoveProg += m_trailFaceProgress[i];
	}
}
