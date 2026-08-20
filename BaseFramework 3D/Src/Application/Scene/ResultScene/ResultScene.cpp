#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Result/ResultUIObjects.h"
#include "../../StageManager/StageManager.h"

void ResultScene::Init()
{
	//UI全般
	std::shared_ptr<ResultUIObject> UIObj = std::make_shared<ResultUIObject>();
	AddObject(UIObj);

	//ゲーム速度戻す
	SCENEMGR.SetGameSpeed(1.0f);
}

void ResultScene::Event()
{
	if (GetAsyncKeyState('R') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}

	KdDebugGUI::Instance().AddLog("Stage Time : %.2f\n", STAGEMGR.GetLastClearTime());
	KdDebugGUI::Instance().AddLog("Stage Star : %d\n", STAGEMGR.GetLastStarCount());
}
