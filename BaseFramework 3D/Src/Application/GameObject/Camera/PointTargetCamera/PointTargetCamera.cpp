#include "PointTargetCamera.h"
#include "../../../Cursor/CursorManager.h"

void PointTargetCamera::Init(Math::Vector3 targetPos)
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準位置
	SetTarget(targetPos);
	SetViewPoint(targetPos + Math::Vector3(0, 0.7f, -3.0f));
	m_DegAng = Math::Vector3::Zero;
}

void PointTargetCamera::PostUpdate()
{
	//視点適応
	Math::Vector3 _tmpPos = m_viewPoint;
	Math::Matrix _shadowVP = DirectX::XMMatrixLookAtLH(_tmpPos, m_targetPoint, Math::Vector3::Up);

	m_mWorld = _shadowVP.Invert();
}

void PointTargetCamera::MoveCamera()
{
	Math::Vector3 currentView = GetCurrentViewPoint();
	Math::Vector3 move = Math::Vector3::Zero;

	//カメラ移動
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		move.z += 1.0f;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		move.z -= 1.0f;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		move.x -= 1.0f;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		move.x += 1.0f;
	}
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		move.y += 1.0f;
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		move.y -= 1.0f;
	}

	move.Normalize();
	move *= 0.2f;

	// 移動を歪める
	Math::Vector3 moveNormal = Math::Vector3::TransformNormal(move, GetRotationYMatrix());

	// 移動
	SetTarget(m_targetPoint + moveNormal);
	SetViewPoint(m_viewPoint + moveNormal);
}

void PointTargetCamera::RotateCamera()
{
	// 変更前のターゲット方向を保持
	Math::Vector3 TargetDir = m_targetPoint - m_viewPoint;

	// TPSと同じように回転
	// マウスでカメラを回転させる処理
	POINT _nowPos = CURSOR.GetFixedCursorPos();

	POINT _mouseMove{};
	_mouseMove.x = _nowPos.x - m_FixMousePos.x;
	_mouseMove.y = _nowPos.y - m_FixMousePos.y;

	// 実際にカメラを回転させる処理(0.15はただの補正値)
	float MoveAngY = _mouseMove.x * 0.15f;
	m_DegAng.y += MoveAngY;

	// 回転に合わせてターゲット位置を更新
	SetTarget(m_viewPoint + Math::Vector3::TransformNormal(TargetDir, Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(MoveAngY))));
}

void PointTargetCamera::SetTarget(Math::Vector3 target)
{
	m_targetPoint = target;
}

void PointTargetCamera::SetViewPoint(Math::Vector3 point)
{
	m_viewPoint = point;
}

void PointTargetCamera::MoveTargetPoint(Math::Vector3 target)
{
	SetTarget(target);
	SetViewPoint(target + Math::Vector3(0, 0.7f, -3.0f));
	m_DegAng = Math::Vector3::Zero;
}
