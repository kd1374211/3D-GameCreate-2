#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Title/TitleUIObjects.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::StageSelect
		);
	}
}

void TitleScene::Init()
{
	//UI全般
	std::shared_ptr<TitleUIObject> UIObj = std::make_shared<TitleUIObject>();
	AddObject(UIObj);
}
