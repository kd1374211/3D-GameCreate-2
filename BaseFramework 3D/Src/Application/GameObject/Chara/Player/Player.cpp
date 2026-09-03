#include "Player.h"
#include "../../../StageManager/StageManager.h"
#include "../../Camera/CameraBase.h"
#include "../../Camera/CameraManager.h"
#include "../../../Scene/SceneManager.h"
#include "../BowlingBall/BowlingBall.h"

void Player::Update()
{
	// エディットモード中はプレイヤーの移動・操作・物理を停止
	if (STAGEMGR.IsEditMode())
	{
		return;
	}

	//移動不可ならリターン
	if (!m_isMovable)return;

	// ゲームタイム
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 操作関連
	if (m_isInputEnabled)
	{
		// 投げる方向決め
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			// 左回転
			m_facingAngle -= PlayerConsts::TurnSpeed * gameDt;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			// 右回転
			m_facingAngle += PlayerConsts::TurnSpeed * gameDt;
		}
		// 補正
		if (m_facingAngle >= 360.0f)m_facingAngle -= 360.0f;
		else if (m_facingAngle <= 0.0f)m_facingAngle += 0.0f;

		// 投げる強さ決め
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			// 強く
			m_throwPower += PlayerConsts::PowerChangeSpeed * gameDt;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			// 弱く
			m_throwPower -= PlayerConsts::PowerChangeSpeed * gameDt;
		}
		// 補正
		if (m_throwPower >= PlayerConsts::MaxPower)m_throwPower = PlayerConsts::MaxPower;
		else if (m_throwPower <= PlayerConsts::MinPower)m_throwPower = PlayerConsts::MinPower;

		// 投げ
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			// ボールがあること
			if (auto ball = m_wpBowlingBall.lock())
			{
				// 投げ
				ball->Throw(m_pos + Math::Vector3(0, 0.2f, 0), GetForwardVectorFromFacingAngle(m_facingAngle), m_throwPower);
				
				// カメラのターゲットを移行
				if (const auto& cam = CAMERAMGR.GetGameCamera().lock())
				{
					cam->SetTarget(m_wpBowlingBall.lock());
				}

				// 投げたら操作不可に
				m_isInputEnabled = false;
			}
		}
	}

	//カメラに設定
	if (m_wpCamera.expired())return;

	m_wpCamera.lock()->SetRotationYMatrix(Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_facingAngle)));
}

void Player::PostUpdate()
{
	// 4. Math::Matrix を使って行列を作成
	// ※度数法 -> 弧度法（ラジアン）変換 ( m_facingAngle * (π / 180.0f) )
	float yawRad = m_facingAngle * static_cast<float>(M_PI / 180.0f);

	// B. 進行方向への旋回（Y軸）
	Math::Matrix matYaw = Math::Matrix::CreateRotationY(yawRad);

	// C. 位置（Translation）
	Math::Matrix matTrans = Math::Matrix::CreateTranslation(
		m_pos
	);

	// D. ワールド行列の合成（ 転がり → 旋回 → 移動  )
	// ※ご使用の環境の行列乗算順序に合わせて「*」の順序を調整してください
	m_mWorld = matYaw * matTrans;

	//GUI
	KdDebugGUI::Instance().AddLog("Pos : %.2f,%.2f,%.2f\n", m_pos.x, m_pos.y, m_pos.z);
	KdDebugGUI::Instance().AddLog("Facing Angle : %.2f\n", m_facingAngle);
	KdDebugGUI::Instance().AddLog("ThrowPower : %.2f\n", m_throwPower);
}

void Player::DrawLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::Respawn(const Math::Vector3& pos, const Math::Quaternion rot)
{
	// まずは状態のリセット
	Reset();

	// 位置と向きを設定
	m_pos = pos;
	m_facingAngle = GetFacingAngleFromQuaternion(rot);
}

void Player::Reset()
{
	// フラグリセット
	m_isMovable = true;
	m_isInputEnabled = true;

	// 数値リセット
	m_throwPower = PlayerConsts::StartPower;
}

void Player::Init()
{
	//モデル
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Chara/PlayerBall/bowling_ball.gltf");

	//デバッグ
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}
