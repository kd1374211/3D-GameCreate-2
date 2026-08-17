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
	//長押し対策
	static bool isSpacePressed = true;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!isSpacePressed)
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

		isSpacePressed = true;
	}
	else isSpacePressed = false;
}
