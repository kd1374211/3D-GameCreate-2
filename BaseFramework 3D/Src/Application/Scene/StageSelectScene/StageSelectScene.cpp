#include "StageSelectScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/StageSelect/StageSelectUIObjects.h"
#include "../../Cursor/CursorManager.h"
#include "../../Const/DeviceAndKey.h"

void StageSelectScene::Init()
{
	//UI全般
	std::shared_ptr<StageSelectUIObject> UIObj = std::make_shared<StageSelectUIObject>();
	m_wpUI = UIObj;
	AddObject(UIObj);

	//フェードイン
	FADEMGR.StartFadeIn(&m_isFadeInEnd);

	// カーソル表示・固定解除
	CURSOR.SetIsShowCursor(true);
	CURSOR.UnlockCursor();
}

void StageSelectScene::Event()
{
	//フェードインが終わってかつフェードアウトも終わったら
	if (m_isSceneChangeReady)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);

		return;
	}

	// 押されたことを確認
	if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_LBUTTON)))
	{
		//フェードインが終わっていたら
		if (m_isFadeInEnd)
		{
			auto spUI = m_wpUI.lock();

			// いずれかのステージが選択状態なことを確認
			if (spUI && spUI->GetIsSelect())
			{
				//ステージ準備
				if (!m_wpUI.expired())
				{
					SCENEMGR.SetStageNo(m_wpUI.lock()->GetSelectedStageNo());
				}

				//フェードアウト
				FADEMGR.StartFadeOut(&m_isSceneChangeReady);
			}
		}
	}
}
