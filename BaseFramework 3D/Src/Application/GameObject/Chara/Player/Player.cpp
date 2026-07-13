#include "Player.h"
#include "../../Camera/CameraBase.h"
#include "../../../Scene/SceneManager.h"

void Player::Update()
{
	//カメラ取得
	Math::Matrix camRotYMat = Math::Matrix::Identity;
	if (!m_wpCamera.expired())
	{
		//カメラ回転取得
		camRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
	}

	//抵抗
	m_moveVelocity *= 0.98f;

	//移動
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_moveVelocity += Math::Vector3::TransformNormal(Math::Vector3(0, 0, MOVESPEED), camRotYMat);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_moveVelocity += Math::Vector3::TransformNormal(Math::Vector3(-MOVESPEED, 0, 0), camRotYMat);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_moveVelocity += Math::Vector3::TransformNormal(Math::Vector3(0, 0, -MOVESPEED), camRotYMat);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_moveVelocity += Math::Vector3::TransformNormal(Math::Vector3(MOVESPEED, 0, 0), camRotYMat);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)m_moveVelocity.y = 0.1f;

	//重力更新
	if (m_moveVelocity.y > GRAVITYMAX)
	{
		m_moveVelocity.y -= GRAVITY;
		if (m_moveVelocity.y < GRAVITYMAX)m_moveVelocity.y = GRAVITYMAX;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)m_moveVelocity = Math::Vector3::Zero;

	//移動方向更新
	m_moveDir = m_moveVelocity;
	m_moveDir.Normalize();
	//未来座標更新
	m_nextPos = m_pos + m_moveVelocity;

	//マトリックス
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = trans;
}

void Player::PostUpdate()
{
	//移動距離取得
	float moveDist = (m_nextPos - m_pos).Length();

	//スフィア判定
	KdCollider::SphereInfo sphere;

	//設定(未来座標を基準に)
	sphere.m_sphere.Center = m_nextPos;
	sphere.m_sphere.Radius = HITRADIUS;
	sphere.m_type = KdCollider::TypeGround;

	//デバッグ
	m_pDebugWire->AddDebugSphere(sphere.m_sphere.Center, sphere.m_sphere.Radius, kWhiteColor);

	//当たり判定
	std::list<KdCollider::CollisionResult> retSphereList;
	for (auto& obj : SCENEMGR.GetObjList())
	{
		obj->Intersects(sphere, &retSphereList);
	}

	//一番めり込みが長いもの
	bool isHit = false;
	float maxOverlap = 0.0f;
	Math::Vector3 hitPos = Math::Vector3::Zero;
	Math::Vector3 hitDir = Math::Vector3::Zero;
	Math::Vector3 normal = Math::Vector3::Zero;
	for (auto& result : retSphereList)
	{
		if (maxOverlap < result.m_overlapDistance)
		{
			hitPos = result.m_hitPos;
			hitDir = result.m_hitDir;
			normal = result.m_hitNDir;
			isHit = true;
			maxOverlap = result.m_overlapDistance;
		}
	}

	//当たっていたら跳ね返り
	if (isHit)
	{
		//反射ベクトルを求める
		float dot = -m_moveDir.Dot(normal);
		Math::Vector3 reflect = m_moveDir + 2 * dot * normal;
		reflect.Normalize();

		//接触位置に合うように位置を調整
		Math::Vector3 adjPos = m_nextPos + normal * maxOverlap;

		//反射の角度(XYZ)を元に速度補正
		Math::Vector3 bounceSpeed = Math::Vector3::Zero;
		Math::Vector3 currentDir = m_moveDir;				//現在移動方向
		Math::Vector3 toDir = hitDir;						//向きたい方向(反射角度)

		Math::Vector3 cDir_test;
		Math::Vector3 toDir_test;

		//X横
		cDir_test = Math::Vector3(currentDir.x, 0, 0);
		toDir_test = Math::Vector3(toDir.x, 0, 0);
		float dotX = cDir_test.Dot(toDir_test);

		//Y高さ
		cDir_test = Math::Vector3(0, currentDir.y, 0);
		toDir_test = Math::Vector3(0, toDir.y, 0);
		float dotY = cDir_test.Dot(toDir_test);
		
		//Z奥行き
		cDir_test = Math::Vector3(0, 0, currentDir.z);
		toDir_test = Math::Vector3(0, 0, toDir.z);
		float dotZ = cDir_test.Dot(toDir_test);

		//角度差×3
		Math::Vector3 angle = Math::Vector3(fabs(asin(dotX)), fabs(acos(dotY)), fabs(atan(dotZ)));
		bounceSpeed.x = (M_PI - angle.x) / M_PI;
		bounceSpeed.y = (M_PI - angle.y) / M_PI;
		bounceSpeed.z = (M_PI - angle.z) / M_PI;

		//反射
		//m_nextPos = adjPos + reflect * distAfter;
		m_nextPos = adjPos;

		//反射後の移動方向と速度を適応
		m_moveVelocity = reflect * moveDist;
		m_moveVelocity *= bounceSpeed;

		m_pDebugWire->AddDebugLine(hitPos, hitPos + normal * HITRADIUS, kGreenColor);
	}

	m_pDebugWire->AddDebugLine(m_pos, m_pos + m_moveVelocity, kRedColor);

	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("MovePowerX :%.2f\n", m_moveVelocity.x);
	KdDebugGUI::Instance().AddLog("MovePowerY :%.2f\n", m_moveVelocity.y);
	KdDebugGUI::Instance().AddLog("MovePowerZ :%.2f\n", m_moveVelocity.z);

	//未来座標を適用
	m_pos = m_nextPos;

	//マトリックス
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = trans;
}

void Player::Init()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/Player/Player.gltf");

	//移動速度設定
	m_moveSpeed = MOVESPEED;

	//デバッグ
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}
