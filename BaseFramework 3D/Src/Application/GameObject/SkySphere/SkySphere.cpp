#include "SkySphere.h"
#include "../../Scene/SceneManager.h"

SkySphere::SkySphere(std::string modelPath)
{
	m_model = std::make_shared<KdModelData>();
	m_model->Load(modelPath);
}

void SkySphere::Update()
{
	// ゲームdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 回転し続ける
	m_angleDeg += gameDt;

	if (m_angleDeg >= 360.0f)m_angleDeg -= 360.0f;
	else if (m_angleDeg <= 0.0f)m_angleDeg += 360.0f;

	// 行列更新
	Math::Matrix rotY = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angleDeg));
	m_mWorld = rotY;
}

void SkySphere::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
