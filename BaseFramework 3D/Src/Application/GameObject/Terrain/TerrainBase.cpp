#include "TerrainBase.h"

void TerrainBase::Update()
{
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = trans;
}

void TerrainBase::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
