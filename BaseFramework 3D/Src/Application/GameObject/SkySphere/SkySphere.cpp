#include "SkySphere.h"

SkySphere::SkySphere(std::string modelPath)
{
	m_model = std::make_shared<KdModelData>();
	m_model->Load(modelPath);

	//固定配置
	Math::Matrix trans = Math::Matrix::CreateTranslation(Math::Vector3::Zero);
	m_mWorld = trans;
}

void SkySphere::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
