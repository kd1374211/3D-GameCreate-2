#include "TerrainManager.h"
#include "../../Scene/SceneManager.h"

void TerrainManager::GenerateTerrain()
{
	//とりあえず仮地面を配置
	std::shared_ptr<Ground> ground = std::make_shared<Ground>();
	ground->SetPos(Math::Vector3::Zero);
	SCENEMGR.AddWaitingObject(ground);
}

void TerrainManager::Init()
{}

void TerrainManager::Release()
{}
