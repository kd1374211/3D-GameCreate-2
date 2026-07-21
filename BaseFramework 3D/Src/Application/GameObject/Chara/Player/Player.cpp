#include "Player.h"
#include "../../Camera/CameraBase.h"
#include "../../../Scene/SceneManager.h"

Player::Player(const Math::Vector3& a_startPos, float a_radius)
{
	//物理Init
	m_cPhysics = std::make_shared<PhysicsComponent>();
	m_cPhysics->Init(a_startPos, a_radius, false);

	//本体Init
	Init();
}

void Player::Update()
{
	//カメラ取得
	Math::Matrix camRotYMat = Math::Matrix::Identity;
	if (!m_wpCamera.expired())
	{
		//カメラ回転取得
		camRotYMat = m_wpCamera.lock()->GetRotationYMatrix();
	}
	//移動方向（仮）
	Math::Vector3 moveDir = Math::Vector3::Zero;

	//移動
	if (GetAsyncKeyState('W') & 0x8000)
	{
		moveDir += Math::Vector3::TransformNormal(Math::Vector3(0, 0, 1.0f), camRotYMat);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		moveDir += Math::Vector3::TransformNormal(Math::Vector3(-1.0f, 0, 0), camRotYMat);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		moveDir += Math::Vector3::TransformNormal(Math::Vector3(0, 0, -1.0f), camRotYMat);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		moveDir += Math::Vector3::TransformNormal(Math::Vector3(1.0f, 0, 0), camRotYMat);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) moveDir.y == 1.0f;

	moveDir.Normalize();

	if (m_cPhysics) {
		// キー入力方向へ物理的な力を加え、ボールをゴロゴロ転がす！
		// 例: moveDirectionが (1, 0, 0) なら右方向へ
		float moveSpeed = 15.0f;
		m_cPhysics->AddForce(moveDir * moveSpeed);
	}
}

void Player::PostUpdate()
{
	//計算後の速度を適用
	m_cPhysics->Sync(m_pos, m_rotation);

	//マトリックス
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = trans;
}

void Player::Init()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/Player/Player.gltf");
}
