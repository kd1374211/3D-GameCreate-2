#include "TerrainBase.h"

void TerrainBase::Update()
{
}

void TerrainBase::DrawLit()
{
	if (!m_model)return;
	
	// トゥーン切り替え
	KdShaderManager::Instance().m_StandardShader.SetToonEnable(true);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);

	// アウトラインを切り替えて再度描画
	KdShaderManager::Instance().m_StandardShader.SetOutLineEnable(true);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void TerrainBase::GenerateDepthMapFromLight()
{
	if (!m_model)return;

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
