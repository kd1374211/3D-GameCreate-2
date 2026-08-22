#include "SkySphere.h"

SkySphere::SkySphere(std::string modelPath)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(modelPath);

	//固定配置
	Math::Matrix trans = Math::Matrix::CreateTranslation(Math::Vector3::Zero);
	Math::Matrix scale = Math::Matrix::CreateScale(100.0f);
	m_mWorld = scale * trans;
}

void SkySphere::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
