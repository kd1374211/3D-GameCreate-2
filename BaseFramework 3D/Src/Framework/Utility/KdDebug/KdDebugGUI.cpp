#include "../../../Application/main.h"

#include "KdDebugGUI.h"

//エディター関連
#include "../../../Application/StageManager/StageManager.h"
#include "../../../Application/Scene/SceneManager.h"
#include "../../../Application/GameObject/Chara/CharaManager.h"
#include "../../../Application/GameObject/Camera/CameraBase.h"

KdDebugGUI::KdDebugGUI()
{}
KdDebugGUI::~KdDebugGUI()
{ 
	GuiRelease(); 
}

void KdDebugGUI::GuiInit(int w, int h)
{
	// 初期化済みなら動作させない
	if (m_uqLog) return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle(), ImVec2(w,h));
	ImGui_ImplDX11_Init(KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);
	m_uqLog = std::make_unique<ImGuiAppLog>();
}

void KdDebugGUI::GuiProcess()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	//===========================================================
	// ImGui開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================
	static bool isEditWindow = false;
	static bool isEditWindowKey = false;
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		if (!isEditWindowKey)
		{
			isEditWindow = !isEditWindow;
			isEditWindowKey = true;

			if (STAGEMGR.IsEditMode())
			{
				STAGEMGR.SetMode(StageMode::Play);
				SCENEMGR.SetGameSpeed(1.0f);
			}
			else
			{
				STAGEMGR.SetMode(StageMode::Edit);
				SCENEMGR.SetGameSpeed(0.0f);
			}
		}
	}
	else
	{
		isEditWindowKey = false;
	}

	if (isEditWindow)
	{
		if (ImGui::Begin("Stage Editor"))
		{
			// 選択インデックス（関数全体で共通の1つだけ定義）
			static int selectedIndex = -1;

			// --- 1. ファイル保存 / 読み込み ---
			static char stageName[128] = "Stage01";
			ImGui::InputText("StageName", stageName, sizeof(stageName));

			std::string filePath = "Asset/Data/StageData/" + std::string(stageName) + ".json";

			if (ImGui::Button("Save Stage"))
			{
				STAGEMGR.SaveStage(filePath);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Stage"))
			{
				STAGEMGR.LoadStage(filePath);
				selectedIndex = -1; // ロードした際は選択状態をリセット
			}

			ImGui::Separator();

			// --- 2. 地形（Terrain）パラメータの編集 ---
			if (ImGui::TreeNode("Terrain Settings"))
			{
				auto& terrainPath = STAGEMGR.GetTerrainPath();

				char pathBuf[256];
				strcpy_s(pathBuf, terrainPath.c_str());
				if (ImGui::InputText("Model Path", pathBuf, sizeof(pathBuf)))
				{
					terrainPath = pathBuf;
				}

				if (ImGui::Button("Rebuild Stage"))
				{
					STAGEMGR.BuildStage();
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			// --- 3. 配置オブジェクト（Objects）の編集 ---
			auto& stageObjects = STAGEMGR.GetStageObjects();

			ImGui::Text("Placed Objects (%d)", static_cast<int>(stageObjects.size()));

			if (ImGui::Button("+ Add NormalPin"))
			{
				StageObjectData newPin;
				newPin.m_type = "NormalPin";
				newPin.m_position = { 0.0f, 0.0f, 0.0f };
				newPin.m_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
				newPin.m_scale = { 1.0f, 1.0f, 1.0f };

				STAGEMGR.AddStageObject(newPin);
				selectedIndex = static_cast<int>(stageObjects.size()) - 1;

				STAGEMGR.BuildStage();
			}

			// 一覧リスト部分
			ImGui::BeginChild("ObjectList", ImVec2(0, 150), true);
			for (int i = 0; i < stageObjects.size(); ++i)
			{
				std::string label = "[" + std::to_string(i) + "] " + stageObjects[i].m_type;
				bool isSelected = (selectedIndex == i);

				if (ImGui::Selectable(label.c_str(), isSelected))
				{
					selectedIndex = i;
				}
			}
			ImGui::EndChild();

			// 選択アイテムの範囲チェック（削除等でオーバーした場合の安全対策）
			if (selectedIndex >= static_cast<int>(stageObjects.size()))
			{
				selectedIndex = static_cast<int>(stageObjects.size()) - 1;
			}

			// 選択アイテムのパラメータ編集
			if (selectedIndex >= 0 && selectedIndex < static_cast<int>(stageObjects.size()))
			{
				ImGui::Separator();
				auto& obj = stageObjects[selectedIndex];

				bool isChanged = false;
				isChanged |= ImGui::DragFloat3("Position", &obj.m_position.x, 0.05f);
				//isChanged |= ImGui::DragFloat4("Rotation (Quat)", &obj.m_rotation.x, 0.01f);
				//isChanged |= ImGui::DragFloat3("Scale", &obj.m_scale.x, 0.05f);

				if (isChanged)
				{
					STAGEMGR.BuildStage();
				}

				if (ImGui::Button("Delete Selected"))
				{
					STAGEMGR.RemoveStageObject(selectedIndex);
					selectedIndex = -1;

					STAGEMGR.BuildStage();
				}
			}	

			// 最後に StageManager にインデックスを渡す
			STAGEMGR.SetSelectedIndex(selectedIndex);
			AddLog("Selected Index: %d", selectedIndex);
		}
		ImGui::End();
	}

	// デバッグウィンドウ(日本語を表示したい場合はこう書く)
//	if (ImGui::Begin(U8("えふぴぃえす")))
//	{
		// FPS
//		ImGui::Text("FPS : %d", Application::Instance().GetNowFPS());
//	}
//	ImGui::End();

	// ログウィンドウ
	m_uqLog->Draw("Log Window");

	//=====================================================
	// ログ出力 ・・・ AddLog("～") で追加
	//=====================================================

//	m_uqLog->AddLog("hello world\n");

	//=====================================================
	// 別ソースファイルからログを出力する場合
	//=====================================================

//	KdDebugGUI::Instance().AddLog("TestLog\n");

	//===========================================================
	// ここより上にImGuiの描画はする事
	//===========================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void KdDebugGUI::AddLog(const char* fmt,...)
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	char tmpStr[128] = {};
	va_list args;
	va_start(args, fmt);
	vsprintf_s(tmpStr, fmt, args);
	m_uqLog->AddLog(tmpStr);
	va_end(args);
}

void KdDebugGUI::ClearLog()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog->Clear();
}

void KdDebugGUI::GuiRelease()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
