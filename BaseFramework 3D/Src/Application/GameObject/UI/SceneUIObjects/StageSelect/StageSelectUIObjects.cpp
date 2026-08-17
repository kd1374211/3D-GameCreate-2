#include "StageSelectUIObjects.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../Scene/SceneManager.h"

void StageSelectUIObject::Update()
{
	static bool isUpKey = true;
	static bool isDownKey = true;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (!isUpKey)
		{
			m_selectStageNo--;
			if (m_selectStageNo < m_minStageNo)
			{
				m_selectStageNo = m_maxStageNo;
			}

			//サムネ画像切り替え
			ChangeThumbTex();

			isUpKey = true;
		}
	}
	else isUpKey = false;

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (!isDownKey)
		{
			m_selectStageNo++;
			if (m_selectStageNo > m_maxStageNo)
			{
				m_selectStageNo = m_minStageNo;
			}

			//サムネ画像切り替え
			ChangeThumbTex();

			isDownKey = true;
		}
	}
	else isDownKey = false;

	KdDebugGUI::Instance().AddLog("SelectStage : %d\n", m_selectStageNo);
}

void StageSelectUIObject::DrawSprite()
{
	//情報取得
	const auto* stageInfo = STAGEMGR.GetStageInfo(m_selectStageNo);

	//黒背景
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &kBlackColor, true);

	//初期位置
	Math::Vector2 drawPos = UILayoutConfig::ListStartPos;
	//ステージリスト箱
	for (int i = m_minStageNo; i <= m_maxStageNo; i++)
	{
		//フレーム（仮）

		//選択中のものは色を変える
		Math::Color color;
		if (i == m_selectStageNo)
		{
			color = Math::Color(0.7f, 0.7f, 0.2f, 1.0f);
		}
		else
		{
			color = Math::Color(0.6f, 0.6f, 0.6f, 1.0f);
		}
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageListFrameTex, drawPos.x, drawPos.y, UILayoutConfig::FrameSize.x, UILayoutConfig::FrameSize.y, nullptr, &color);

		//ステージリスト名
		std::string stageListName = STAGEMGR.GetStageInfo(i)->m_stageListName;
		KdShaderManager::Instance().m_spriteShader.DrawFont(1, UILayoutConfig::StageListNameFontSize, drawPos, &kWhiteColor, stageListName.c_str());

		//位置移動
		drawPos.y -= UILayoutConfig::LineSpacing;
	}

	//現在選択中のステージの情報
	//ウィンドウ
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageInfoFrameTex, UILayoutConfig::DetailWindowPos.x, UILayoutConfig::DetailWindowPos.y, UILayoutConfig::DetailWindowSize.x, UILayoutConfig::DetailWindowSize.y, nullptr);

	//サムネイル
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageThumbTex, UILayoutConfig::ThumbnailPos.x, UILayoutConfig::ThumbnailPos.y, UILayoutConfig::ThumbnailSize.x, UILayoutConfig::ThumbnailSize.y, nullptr);

	//ステージ名
	KdShaderManager::Instance().m_spriteShader.DrawFont(1, UILayoutConfig::StageNameFontSize, UILayoutConfig::StageNamePos, &kWhiteColor, stageInfo->m_stageName.c_str());

	// 操作キーヘルプ
	std::string keyHelpText = "[▲/▼] 選択   [SPACE] 決定";

	KdShaderManager::Instance().m_spriteShader.DrawFont(
		1,
		UILayoutConfig::KeyHelpFontSize,
		UILayoutConfig::KeyHelpPos,
		&kWhiteColor,
		keyHelpText.c_str()
	);
}

void StageSelectUIObject::Init()
{
	//現在選択・最大・最小選択ステージ番号を取得
	m_selectStageNo = SCENEMGR.GetStageNo();
	m_maxStageNo = STAGEMGR.GetMaxStageNo();
	m_minStageNo = STAGEMGR.GetMinStageNo();

	//画像ロード
	m_stageListFrameTex = std::make_shared<KdTexture>();
	m_stageListFrameTex->Load("Asset/Textures/UI/SceneUI/StageSelect/StageListFrame.png");

	m_stageInfoFrameTex = std::make_shared<KdTexture>();
	m_stageInfoFrameTex->Load("Asset/Textures/UI/SceneUI/StageSelect/StageInfoFrame.png");

	m_stageThumbTex = std::make_shared<KdTexture>();
	ChangeThumbTex();
}

void StageSelectUIObject::ChangeThumbTex()
{
	std::string startStagePath = STAGEMGR.GetStageInfo(m_selectStageNo)->m_stageThumbPath;
	m_stageThumbTex->Load(startStagePath);
}
