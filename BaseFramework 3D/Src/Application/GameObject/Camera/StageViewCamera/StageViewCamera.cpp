#include "StageViewCamera.h"

void StageViewCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 0.7f, -3.0f);

	// 見下ろし
	m_mLocalRot = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(20.0f));
}

void StageViewCamera::PostUpdate()
{
	//回転速度
	float angleChange = 0.3f;

	//回転
	m_angleDeg += angleChange;
	if (m_angleDeg >= 360.0f)m_angleDeg -= 360.0f;

	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	//カメラの位置確定
	m_mLocalPos = Math::Matrix::CreateTranslation(GetRotationMatrix().Forward() * m_viewDistance);
	m_mRotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angleDeg));
	m_mWorld = m_mLocalPos * m_mLocalRot * m_mRotation * _targetMat;
}
