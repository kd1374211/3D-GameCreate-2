#include "GameScene.h"
#include"../SceneManager.h"

#include "../../GameObject/Terrain/TerrainManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Chara/Player/Player.h"

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void GameScene::Init()
{
	//地形生成
	TERRAINMGR.GenerateTerrain();

	//カメラとプレイヤー生成
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();
	std::shared_ptr<Player> player = std::make_shared<Player>(Math::Vector3(0, 0.5f, 0), 0.5f);
	//player->SetPos(Math::Vector3(0, 0.5f, 0));
	
	//相互リンク
	camera->SetTarget(player);
	player->SetCamera(camera);

	//追加
	AddObject(camera);
	AddObject(player);
}
