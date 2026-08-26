#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Result/ResultUIObjects.h"
#include "../../StageManager/StageManager.h"

#include "../../GameObject/Camera/StageViewCamera/StageViewCamera.h"
#include "../../FadeManager/FadeManager.h"
#include "../../GameObject/Camera/CameraManager.h"

void ResultScene::Init()
{
	//背景ステージ読み込み
	STAGEMGR.LoadStage(SCENEMGR.GetStageNo());
	//背景モードで生成
	STAGEMGR.BuildStage(StageBuildMode::Background);

	//ステージ確認カメラ
	std::shared_ptr<StageViewCamera> camera = std::make_shared<StageViewCamera>();
	camera->Init();
	camera->SetViewDistance(20.0f);

	//さっき生成した地形をターゲットに
	if (!STAGEMGR.GetTerrain().expired())
	{
		camera->SetTarget(STAGEMGR.GetTerrain().lock());
	}

	//追加
	AddObject(camera);

	//マネージャーに追加
	CAMERAMGR.SetGameCamera(camera);
	CAMERAMGR.SetDefaultCamera(CameraType::Game);

	//UI全般
	std::shared_ptr<ResultUIObject> UIObj = std::make_shared<ResultUIObject>();
	AddObject(UIObj);

	//ゲーム速度戻す
	SCENEMGR.SetGameSpeed(1.0f);

	//フェードイン
	FADEMGR.StartFadeIn();

	//さっきのリザルトとセーブデータのリザルトを比べて更新してよいなら更新
	auto saveData = STAGEMGR.WorkUserSave();
	auto resultData = STAGEMGR.GetLastGameResult();
	
	// セーブデータが存在している
	// ステージをクリアしている
	if (saveData != nullptr && resultData.m_isCleared)
	{
		// データが変わったか
		bool isSaveUpdated = false;

		// 新規クリア確認
		if (!saveData->m_isClear)
		{
			//			false					 true
			saveData->m_isClear = resultData.m_isCleared;

			// 更新した
			isSaveUpdated = true;
		}

		// 最大ピン数更新
		if (saveData->m_bestPinFallen < resultData.m_fallenPinCnt)
		{
			// 更新
			saveData->m_bestPinFallen = resultData.m_fallenPinCnt;

			// した
			isSaveUpdated = true;
		}

		// 更新していたらセーブ読み込み
		if (isSaveUpdated)
		{
			STAGEMGR.SaveUserData();
		}
	}
}

void ResultScene::Event()
{
	//フェードイン終了待ち
	if (FADEMGR.IsFadeInEnd())
	{
		m_isFadeInEnd = true;
	}

	if (m_isFadeInEnd && FADEMGR.IsFadeOutEnd())
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::StageSelect
		);

		return;
	}

	static bool isSpacePressed = true;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!isSpacePressed)
		{
			if (m_isFadeInEnd)
			{
				FADEMGR.StartFadeOut();
			}
		}

		isSpacePressed = true;
	}
	else isSpacePressed = false;
}
