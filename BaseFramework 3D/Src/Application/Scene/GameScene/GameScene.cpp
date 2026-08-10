#include "GameScene.h"
#include"../SceneManager.h"

#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Chara/Player/Player.h"
#include "../../GameObject/BowlingPin/NormalPin/NormalPin.h"
#include "../../GameObject/Chara/CharaManager.h"

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
	//ステージデータ読み込み
	STAGEMGR.LoadStage("Asset/Data/StageData/Stage01.json");

	//カメラとプレイヤー生成
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();
	std::shared_ptr<Player> player = std::make_shared<Player>(Math::Vector3(0, 1.0f, 0), 0.1f);
	
	//取得用にセット
	m_wpCamera = camera;
	CHARAMGR.SetPlayer(player);

	//リンク
	camera->SetTarget(player);
	player->SetCamera(camera);
	
	//追加
	AddObject(camera);
	AddObject(player);
}
