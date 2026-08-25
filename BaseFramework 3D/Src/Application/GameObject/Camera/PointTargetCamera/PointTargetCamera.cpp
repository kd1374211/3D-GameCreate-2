#include "PointTargetCamera.h"

void PointTargetCamera::Init(Math::Vector3 targetPos)
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準位置
	SetTarget(targetPos);
	SetViewPoint(targetPos + Math::Vector3(0, 0.7f, -3.0f));
}

void PointTargetCamera::PostUpdate()
{
	//視点適応
	Math::Vector3 _tmpPos = m_viewPoint;
	Math::Matrix _shadowVP = DirectX::XMMatrixLookAtLH(_tmpPos, m_targetPoint, Math::Vector3::Up);

	m_mWorld = _shadowVP.Invert();
}

void PointTargetCamera::MoveCamera(Math::Vector3 move)
{
	//移動距離チェック
	Math::Vector3 moveDist = move - m_viewPoint;

	SetTarget(m_targetPoint + moveDist);
	SetViewPoint(move);
}

void PointTargetCamera::SetTarget(Math::Vector3 target)
{
	m_targetPoint = target;
}

void PointTargetCamera::SetViewPoint(Math::Vector3 point)
{
	m_viewPoint = point;
}
