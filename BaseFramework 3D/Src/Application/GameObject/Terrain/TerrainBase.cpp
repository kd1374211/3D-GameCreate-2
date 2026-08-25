#include "TerrainBase.h"

void TerrainBase::Update()
{
}

void TerrainBase::DrawLit()
{
	if (!m_model)return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
