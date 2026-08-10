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

	if (ImGui::Begin("Stage Editor"))
	{
		static int selectedIndex = -1;

		// モード切り替えボタン部分
		if (STAGEMGR.IsEditMode())
		{
			if (ImGui::Button("PLAY STAGE", ImVec2(-1, 30)))
			{
				STAGEMGR.SetMode(StageMode::Play);
				// ★ カメラ呼び出し処理を削除
			}
		}
		else
		{
			if (ImGui::Button("EDIT STAGE (Reset)", ImVec2(-1, 30)))
			{
				STAGEMGR.SetMode(StageMode::Edit);
				// ★ カメラ呼び出し処理を削除
			}
		}

		ImGui::Separator();

		// エディットモード中のみ編集UIを触れるようにする
		if (STAGEMGR.IsEditMode())
		{
			// --- 1. ファイル保存 / 読み込み ---
			static char stageName[128] = "Stage01";
			ImGui::InputText("StageName", stageName, sizeof(stageName));

			// 保存・読み込みのパス構築
			std::string filePath = "Asset/Data/StageData/" + std::string(stageName) + ".json";

			if (ImGui::Button("Save Stage"))
			{
				STAGEMGR.SaveStage(filePath);
			}
			ImGui::SameLine();
			if (ImGui::Button("Load Stage"))
			{
				STAGEMGR.LoadStage(filePath);
			}

			ImGui::Separator();

			// --- 2. 地形（Terrain）パラメータの編集 ---
			if (ImGui::TreeNode("Terrain Settings"))
			{
				auto& terrainPath = STAGEMGR.GetTerrainPath();

				// ロード等で外部変更された場合に対応するため毎回バッファを同期
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
			static int selectedIndex = -1;

			ImGui::Text("Placed Objects (%d)", static_cast<int>(stageObjects.size()));

			if (ImGui::Button("+ Add NormalPin"))
			{
				StageObjectData newPin;
				newPin.type = "NormalPin";
				newPin.position = { 0.0f, 0.0f, 0.0f };
				newPin.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
				newPin.scale = { 1.0f, 1.0f, 1.0f };

				STAGEMGR.AddStageObject(newPin);
				selectedIndex = static_cast<int>(stageObjects.size()) - 1;

				// ★ 追加したら即座にステージ再構築
				STAGEMGR.BuildStage();
			}

			// 一覧リスト部分
			ImGui::BeginChild("ObjectList", ImVec2(0, 150), true);
			for (int i = 0; i < stageObjects.size(); ++i)
			{
				std::string label = "[" + std::to_string(i) + "] " + stageObjects[i].type;
				if (ImGui::Selectable(label.c_str(), selectedIndex == i))
				{
					selectedIndex = i;
					// ★ カメラ呼び出し処理を削除
				}
			}
			ImGui::EndChild();

			// 選択アイテムの範囲チェック
			if (selectedIndex >= static_cast<int>(stageObjects.size()))
			{
				selectedIndex = -1;
			}

			// 選択アイテムのパラメータ編集
			if (selectedIndex >= 0 && selectedIndex < stageObjects.size())
			{
				auto& obj = stageObjects[selectedIndex];

				bool isChanged = false;
				// ImGui::DragFloat などの戻り値（bool）を利用して、値が変化したか検知する
				isChanged |= ImGui::DragFloat3("Position", &obj.position.x, 0.05f);
				isChanged |= ImGui::DragFloat4("Rotation (Quat)", &obj.rotation.x, 0.01f);
				isChanged |= ImGui::DragFloat3("Scale", &obj.scale.x, 0.05f);

				// ★ 値がドラッグ操作で動いた瞬間だけ再構築
				if (isChanged)
				{
					STAGEMGR.BuildStage();
				}

				if (ImGui::Button("Delete Selected"))
				{
					STAGEMGR.RemoveStageObject(selectedIndex);
					selectedIndex = -1;

					// ★ 削除時も即座に再構築
					STAGEMGR.BuildStage();
				}
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "[ PLAYING MODE ]");
			ImGui::Text("Edit operations are disabled.");
		}
	}
	ImGui::End();


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
