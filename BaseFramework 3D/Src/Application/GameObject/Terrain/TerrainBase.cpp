#include "TerrainBase.h"
#include "../Camera/CameraManager.h"
#include "../Camera/CameraBase.h"

void TerrainBase::Update()
{
}

void TerrainBase::DrawLit()
{
	if (!m_model)return;
	if (!m_isDraw)return;
	
	// トゥーン切り替え
	KdShaderManager::Instance().m_StandardShader.SetToonEnable(true);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);

	// アウトラインを切り替えて再度描画
	KdShaderManager::Instance().m_StandardShader.SetOutLineEnable(true);
	KdShaderManager::Instance().m_StandardShader.SetToonEnable(false);

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void TerrainBase::GenerateDepthMapFromLight()
{
	if (!m_model)return;
	if (!m_isDraw)return;

	// 描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void TerrainBase::RegistToCamera(bool isRegistToLDCamera)
{
	// GameCamもDebugCamもあれば登録する
	const auto& gameCam = CAMERAMGR.GetCamera(CameraType::Game_Normal).lock();
	if (gameCam)
	{
		gameCam->RegistHitObject(m_cPhysics->GetBodyID());
	}

	const auto& debugCam = CAMERAMGR.GetCamera(CameraType::Debug).lock();
	if (debugCam)
	{
		debugCam->RegistHitObject(m_cPhysics->GetBodyID());
	}

	// フラグがあれば見下ろしカメラにも登録
	if (isRegistToLDCamera)
	{
		if (const auto& ldCam = CAMERAMGR.GetCamera(CameraType::Game_LookingDown).lock())
		{
			ldCam->RegistHitObject(m_cPhysics->GetBodyID());
		}
	}
}
