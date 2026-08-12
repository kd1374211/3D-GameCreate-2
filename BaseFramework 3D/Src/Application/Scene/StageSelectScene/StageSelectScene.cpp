#include "StageSelectScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/StageSelect/StageSelectUIObjects.h"

void StageSelectScene::Init()
{
	//UI全般
	std::shared_ptr<StageSelectUIObject> UIObj = std::make_shared<StageSelectUIObject>();
	m_wpUI = UIObj;
	AddObject(UIObj);
}

void StageSelectScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		//ステージ準備
		if (!m_wpUI.expired())
		{
			SCENEMGR.SetStageNo(m_wpUI.lock()->GetSelectedStageNo());
		}

		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}
