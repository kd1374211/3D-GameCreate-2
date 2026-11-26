#include "../../../Application/main.h"

#include "KdDebugGUI.h"

//エディター関連
#include "../../../Application/StageManager/StageManager.h"
#include "../../../Application/Scene/SceneManager.h"
#include "../../../Application/GameObject/Camera/PointTargetCamera/PointTargetCamera.h"
#include "../../../Application/GameObject/Camera/CameraManager.h"
#include "../../../Application/Component/ScoreHandler/ScoreHandler.h"
#include "../../../Application/Cursor/CursorManager.h"
#include "StageEditor/StageEditor.h"

KdDebugGUI::KdDebugGUI()
{
	InitStageEditor();
}
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

	static bool isScoreGUI = false;

	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		// スコアボード
		static bool isRKey = true;

		if (GetAsyncKeyState('R') & 0x8000)
		{
			if (!isRKey)
			{
				isScoreGUI = !isScoreGUI;
				isRKey = true;
			}
		}
		else isRKey = false;
	}

	if (isScoreGUI)KdDebugGUI::Instance().DrawDebugScoreGUI();


	static bool isEditWindow = false;
	static bool isEditWindowKey = false;

	// 現在選択中のステージ番号とレーン番号（Static変数で保持）
	static int currentStageNo = STAGEMGR.GetMinStageNo(); // デフォルトは最小値(1)
	static int currentLaneNo = BowlingSystemConsts::StartFrame; // デフォルトレーン番号

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		if (!isEditWindowKey)
		{
			// ゲームシーンでのみ開けるように
			if (SCENEMGR.GetCurrentSceneType() == SceneManager::SceneType::Game)
			{
				isEditWindow = !isEditWindow;

				// エディター切り替え処理
				ToggleStageEditor(currentStageNo, currentLaneNo);
			}

			isEditWindowKey = true;
		}
	}
	else
	{
		isEditWindowKey = false;
	}

	// ゲームシーンでないなら閉じる
	if (SCENEMGR.GetCurrentSceneType() != SceneManager::SceneType::Game)
	{
		if (isEditWindow)
		{
			isEditWindow = false;
			STAGEMGR.SetMode(StageMode::Play);
			SCENEMGR.SetGameSpeed(1.0f);
			CAMERAMGR.SetDefaultCamera(CameraType::Game_Normal);
		}
	}

	if (isEditWindow)
	{
		// カメラ取得
		std::weak_ptr<CameraBase> parent = CAMERAMGR.GetCamera(CameraType::Debug);
		std::shared_ptr<PointTargetCamera> camera;
		static bool camCastSuccess = false;
		if (camera = std::dynamic_pointer_cast<PointTargetCamera>(parent.lock()))
		{
			camCastSuccess = true;
		}
		else camCastSuccess = false;

		// 入力中はストップ
		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			if (camCastSuccess)
			{
				// カメラ更新
				UpdateCamera(camera);
			}

			// ギミックの停止・再生の切り替え
			ToggleGimmickStop();
		}

		// ステージエディタ本体
		UpdateStageEditor(currentStageNo, currentLaneNo, camera);
	
		ImGui::End();
	}

	// デバッグウィンドウ(日本語を表示したい場合はこう書く)
//	if (ImGui::Begin(U8("えふぴぃえす")))
//	{
		// FPS
//		ImGui::Text("FPS : %d", Application::Instance().GetNowFPS());
//	}
//	ImGui::End();

	static bool isLog = false;
	static bool isCursor = false;

	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		// ログウィンドウ
		static bool isLogKey = true;
		if (GetAsyncKeyState('W') & 0x8000)
		{
			if (!isLogKey)
			{
				isLog = !isLog;
			}

			isLogKey = true;
		}
		else isLogKey = false;
		if (isLog)m_uqLog->Draw("Log Window");

		static bool isCursorKey = true;
		if (GetAsyncKeyState('E') & 0x8000)
		{
			if (!isCursorKey)
			{
				isCursor = !isCursor;

				ShowCursor(isCursor);
			}

			isCursorKey = true;
		}
		else isCursorKey = false;
	}

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

void KdDebugGUI::ToggleStageEditor(int& currentStageNo, int& currentLaneNo)
{
	if (STAGEMGR.IsEditMode())
	{
		STAGEMGR.SetMode(StageMode::Play);
		// プレイヤーの操作を有効に
		STAGEMGR.BuildStageObjects_D(currentLaneNo);
		CAMERAMGR.SetDefaultCamera(CameraType::Game_Normal);

		// カーソルの固定を再開
		CURSOR.LockCursor();
		// カーソル非表示
		ShowCursor(false);
	}
	else
	{
		STAGEMGR.SetMode(StageMode::Edit);

		//ポイントターゲットカメラ
		//今のカメラのターゲット取得
		std::weak_ptr<CameraBase> currentCamera = CAMERAMGR.GetCamera(CameraType::Game_Normal);
		std::weak_ptr<KdGameObject> cameraTarget;
		Math::Vector3 targetPos = Math::Vector3::Zero;
		if (!currentCamera.expired())
		{
			cameraTarget = currentCamera.lock();
			if (!cameraTarget.expired())
			{
				targetPos = cameraTarget.lock()->GetPos();
			}
		}

		// カーソルの固定を解除
		CURSOR.UnlockCursor();
		// カーソルを表示
		ShowCursor(true);

		std::shared_ptr<PointTargetCamera> camera = std::make_shared<PointTargetCamera>();
		camera->Init(targetPos);
		SCENEMGR.AddObject(camera);

		CAMERAMGR.SetCamera(CameraType::Debug, camera);
		CAMERAMGR.SetDefaultCamera(CameraType::Debug);

		// 選択中のindexを合わせる
		currentStageNo = SCENEMGR.GetStageNo();
		currentLaneNo = BowlingSystemConsts::StartFrame;
		if (auto spScoreHandler = m_wpScoreHandler.lock())
		{
			currentLaneNo = spScoreHandler->GetCurrentFrame();
		}

		// ★ LoadStage(int stageNo) を使用
		if (STAGEMGR.LoadStage(currentStageNo))
		{
			STAGEMGR.BuildStageObjects(currentLaneNo);
		}
	}
}

void KdDebugGUI::UpdateCamera(std::shared_ptr<PointTargetCamera> camera)
{
	//カメラ移動
	camera->MoveCamera();

	// シフトを押している間は視点のY軸回転を認める
	static bool isShift = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		// falseだった（このフレームで押された）場合カーソル非表示と固定
		if (!isShift)
		{
			ShowCursor(false);
			CURSOR.LockCursor();
		}

		// PTカメラ内で回転をしてもらう
		camera->RotateCamera();

		isShift = true;
	}
	else
	{
		// trueだった（このフレームで離された）場合カーソル表示と移動フラグ
		if (isShift)
		{
			ShowCursor(true);
			CURSOR.UnlockCursor();
		}

		isShift = false;
	}
}

void KdDebugGUI::ToggleGimmickStop()
{
	// ギミックの再生、停止
	static bool isPKey = true;

	if (GetAsyncKeyState('P') & 0x8000)
	{
		if (!isPKey)
		{
			// 反転
			STAGEMGR.SetIsGimmickStop(!STAGEMGR.GetIsGimmickStop());

			isPKey = true;
		}
	}
	else isPKey = false;
}

void KdDebugGUI::UpdateStageEditor(int& currentStageNo, int& currentLaneNo, std::shared_ptr<PointTargetCamera> camera)
{
	if (!m_upStageEditor)return;

	m_upStageEditor->UpdateStageEditor(currentStageNo, currentLaneNo, camera);
}

void KdDebugGUI::DrawDebugScoreGUI()
{
	// 弱参照からスマートポインタを取得
	auto spScoreHandler = m_wpScoreHandler.lock();
	if (!spScoreHandler) return;

	// テーブルフラグ（SizingFixedSame で全列の個別設定を可能にする）
	ImGuiTableFlags flags = ImGuiTableFlags_Borders
		| ImGuiTableFlags_RowBg
		| ImGuiTableFlags_SizingFixedFit;

	ImGui::Begin("Score Board", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTable("BowlingScoreTable", 10, flags))
	{
		// -------------------------------------------------------------
		// 0. 各列の幅を設定 (1〜9フレーム: 45px / 10フレーム: 65px)
		// -------------------------------------------------------------
		for (int i = 0; i < 9; ++i)
		{
			ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 45.0f);
		}
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 65.0f);

		// ★ SetupColumn を反映するためにヘッダーコールを呼び出す
		ImGui::TableHeadersRow(); // またはこの行を省略する場合は下記ヘッダー描画で自動適用されます

		// -------------------------------------------------------------
		// 1行目：フレーム番号（ヘッダー）
		// -------------------------------------------------------------
		ImGui::TableNextRow(ImGuiTableRowFlags_None, 24.0f); // 高さ 24px

		for (size_t frameNo = BowlingSystemConsts::StartFrame; frameNo <= BowlingSystemConsts::LastFrame; frameNo++)
		{
			int drawFrame = static_cast<int>(frameNo + 1);

			ImGui::TableSetColumnIndex(static_cast<int>(frameNo));

			// テキストの中央寄せ表示
			float colWidth = ImGui::GetColumnWidth();
			float textWidth = ImGui::CalcTextSize(std::to_string(drawFrame).c_str()).x;
			if (colWidth > textWidth)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (colWidth - textWidth) * 0.5f);
			}

			ImGui::Text("%d", drawFrame);
		}

		// -------------------------------------------------------------
		// 2行目：各投のスコア表示エリア（1〜9フレームは2投、10フレームは3投）
		// -------------------------------------------------------------
		// スコアデータの事前取得
		std::string dummyScores[10][3] = {};
		for (size_t frameNo = BowlingSystemConsts::StartFrame; frameNo <= BowlingSystemConsts::LastFrame; frameNo++)
		{
			for (size_t throwNo = 0; throwNo < BowlingSystemConsts::MaxThrowCount; throwNo++)
			{
				dummyScores[frameNo][throwNo] = spScoreHandler->GetScore(frameNo, throwNo);
			}
		}

		ImGui::TableNextRow(ImGuiTableRowFlags_None, 28.0f); // 高さ 28px

		// 子テーブル内の余白を小さくしてマス目に納める
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0f, 2.0f));

		for (size_t frameNo = BowlingSystemConsts::StartFrame; frameNo <= BowlingSystemConsts::LastFrame; frameNo++)
		{
			ImGui::TableSetColumnIndex(static_cast<int>(frameNo));

			// ID衝突防止（子テーブル識別用）
			ImGui::PushID(static_cast<int>(frameNo));

			// 1〜9フレーム（2投分表示）
			if (frameNo < BowlingSystemConsts::LastFrame)
			{
				if (ImGui::BeginTable("SubTable_1_9", 2, ImGuiTableFlags_BordersInnerV))
				{
					// 左（1投目）を広め、右（2投目）を固定幅枠に設定
					ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch, 1.0f);
					ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 18.0f);

					ImGui::TableNextRow();

					for (int throwIdx = 0; throwIdx < 2; ++throwIdx)
					{
						ImGui::TableSetColumnIndex(throwIdx);
						const char* pText = dummyScores[frameNo][throwIdx].c_str();

						float colWidth = ImGui::GetColumnWidth();
						float textWidth = ImGui::CalcTextSize(pText).x;
						if (colWidth > textWidth)
						{
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (colWidth - textWidth) * 0.5f);
						}
						ImGui::TextUnformatted(pText);
					}

					ImGui::EndTable();
				}
			}
			// 10フレーム（3投分表示）
			else
			{
				if (ImGui::BeginTable("SubTable_10", 3, ImGuiTableFlags_BordersInnerV))
				{
					ImGui::TableNextRow();

					for (int throwIdx = 0; throwIdx < 3; ++throwIdx)
					{
						ImGui::TableSetColumnIndex(throwIdx);
						const char* pText = dummyScores[frameNo][throwIdx].c_str();

						float colWidth = ImGui::GetColumnWidth();
						float textWidth = ImGui::CalcTextSize(pText).x;
						if (colWidth > textWidth)
						{
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (colWidth - textWidth) * 0.5f);
						}
						ImGui::TextUnformatted(pText);
					}

					ImGui::EndTable();
				}
			}

			ImGui::PopID();
		}

		ImGui::PopStyleVar(); // CellPaddingを復元

		// -------------------------------------------------------------
		// 3行目：累計スコア表示エリア
		// -------------------------------------------------------------
		ImGui::TableNextRow(ImGuiTableRowFlags_None, 24.0f); // 高さ 24px

		for (size_t frameNo = BowlingSystemConsts::StartFrame; frameNo <= BowlingSystemConsts::LastFrame; frameNo++)
		{
			std::string dummyTotals = spScoreHandler->GetTotalScore(frameNo);

			ImGui::TableSetColumnIndex(static_cast<int>(frameNo));

			float colWidth = ImGui::GetColumnWidth();
			float textWidth = ImGui::CalcTextSize(dummyTotals.c_str()).x;
			if (colWidth > textWidth)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (colWidth - textWidth) * 0.5f);
			}

			ImGui::TextUnformatted(dummyTotals.c_str());
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void KdDebugGUI::InitStageEditor()
{
	m_upStageEditor = std::make_unique<StageEditor>();
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
