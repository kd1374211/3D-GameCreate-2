#include "TerrainBase.h"

void TerrainBase::Update()
{
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos);
	Math::Matrix rotat = Math::Matrix::CreateFromQuaternion(m_rot);
	m_mWorld = rotat * trans;
}

void TerrainBase::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
