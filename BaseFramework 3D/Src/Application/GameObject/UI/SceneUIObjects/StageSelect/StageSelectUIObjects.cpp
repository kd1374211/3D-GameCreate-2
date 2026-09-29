#include "StageSelectUIObjects.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../UserSave/UserSaveManager.h"
#include "../../../../Scene/SceneManager.h"
#include "StageListBox/StageListBox.h"

void StageSelectUIObject::Update()
{
	// カーソルフラグリセット
	m_isCursorOnAnyStage = false;

	// 各ステージリストの更新を呼ぶ
	for (const auto& list : m_spList)
	{
		// 更新
		list->Update();

		// 現在選択中のステージが変更されたかの確認
		if (list->GetIsCursor())
		{
			// 違うステージか
			if (m_selectStageNo != list->GetStageNo())
			{
				// 選択中ステージの更新
				m_selectStageNo = list->GetStageNo();

				// サムネイル更新
				ChangeThumbTex();
			}

			// カーソルがあることを確認
			m_isCursorOnAnyStage = true;
		}
	}
}

void StageSelectUIObject::DrawSprite()
{
	//情報取得
	const auto* stageInfo = STAGEMGR.GetStageInfo(m_selectStageNo);
	const auto* stageSave = SAVEMGR.GetUserSave(m_selectStageNo);

	//黒背景
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &kBlackColor, true);

	//現在選択中のステージの情報
	//ウィンドウ
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageInfoFrameTex, StageSelectUIConsts::DetailWindowPos.x, StageSelectUIConsts::DetailWindowPos.y, StageSelectUIConsts::DetailWindowSize.x, StageSelectUIConsts::DetailWindowSize.y, nullptr);

	//サムネイル
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageThumbTex, StageSelectUIConsts::ThumbnailPos.x, StageSelectUIConsts::ThumbnailPos.y, StageSelectUIConsts::ThumbnailSize.x, StageSelectUIConsts::ThumbnailSize.y, nullptr);

	//ステージ名
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::StageSelect_StageName, StageSelectUIConsts::StageNamePos, &kWhiteColor, stageInfo->m_stageName.c_str(), TextAlign::Center);

	//クリアしているか
	if (stageSave->m_isClear)
	{
		KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::StageSelect_Cleared, StageSelectUIConsts::ClearedTextPos, &kGreenColor, "Cleared!", TextAlign::Center);
	}

	// ピン
	// アイコン
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_pinTex, StageSelectUIConsts::PinIconPos.x, StageSelectUIConsts::PinIconPos.y);

	// 操作キーヘルプ
	std::string keyHelpText = "[MOUSE] 選択   [LCLICK] 決定";

	KdShaderManager::Instance().m_spriteShader.DrawFont(
		FontTypeConst::StageSelect_KeyGuide,
		StageSelectUIConsts::KeyHelpPos,
		&kWhiteColor,
		keyHelpText.c_str(),
		TextAlign::Center
	);

	// 各ステージの描画を呼ぶ
	for (const auto& list : m_spList)
	{
		// 描画
		list->DrawSprite();
	}
}

void StageSelectUIObject::Init()
{
	//現在選択・最大・最小選択ステージ番号を取得
	m_selectStageNo = SCENEMGR.GetStageNo();

	// ステージ数分のリストを召喚・設定
	for (int i = STAGEMGR.GetMinStageNo(); i <= STAGEMGR.GetMaxStageNo(); i++)
	{
		// 情報取得
		const StageInfo* info = STAGEMGR.GetStageInfo(i);
		// 箱準備
		std::shared_ptr<StageListBox> box = std::make_shared<StageListBox>();
		// 0基準に
		int indexNum = i - STAGEMGR.GetMinStageNo();
		// ↑から配置する場所を決定
		Math::Vector2 pos = Math::Vector2(StageSelectUIConsts::ListPosBase.x + StageSelectUIConsts::ListPosDiff * (indexNum % StageSelectUIConsts::ListIndexX),
										  StageSelectUIConsts::ListPosBase.y - StageSelectUIConsts::ListPosDiff * (indexNum / StageSelectUIConsts::ListIndexX));

		// 各値の設定
		box->SetStageNo(i);
		box->SetPos(pos);
		box->SetStageListName(info->m_stageListName);
		box->LoadStageThumbPath(info->m_stageThumbPath);

		// 追加
		m_spList.push_back(box);
	}
	
	//画像ロード
	m_stageInfoFrameTex = std::make_shared<KdTexture>();
	m_stageInfoFrameTex->Load("Asset/Textures/UI/SceneUI/StageSelect/StageInfoFrame.png");

	m_stageThumbTex = std::make_shared<KdTexture>();
	ChangeThumbTex();

	m_pinTex = std::make_shared<KdTexture>();
	m_pinTex->Load("Asset/Textures/UI/SceneUI/PinIcon.png");
}

void StageSelectUIObject::ChangeThumbTex()
{
	std::string startStagePath = STAGEMGR.GetStageInfo(m_selectStageNo)->m_stageThumbPath;
	m_stageThumbTex->Load(startStagePath);
}
