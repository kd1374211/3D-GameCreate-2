#include "../../../Application/main.h"

#include "KdDebugGUI.h"

//エディター関連
#include "../../../Application/StageManager/StageManager.h"
#include "../../../Application/Scene/SceneManager.h"
#include "../../../Application/Component/CharaHandler/CharaHandler.h"
#include "../../../Application/GameObject/Camera/PointTargetCamera/PointTargetCamera.h"
#include "../../../Application/GameObject/Camera/CameraManager.h"
#include "../../../Application/Physics/PhysicsLayer.h"
#include "../../../Application/Physics/PhysicsManager.h"
#include "../../../Application/Component/ScoreHandler/ScoreHandler.h"

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

	// スコアボード
	static bool isScoreGUI = false;
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

				if (STAGEMGR.IsEditMode())
				{
					STAGEMGR.SetMode(StageMode::Play);
					SCENEMGR.SetGameSpeed(1.0f);
					CAMERAMGR.SetDefaultCamera(CameraType::Game);
				}
				else
				{
					STAGEMGR.SetMode(StageMode::Edit);
					SCENEMGR.SetGameSpeed(0.0f);

					//ポイントターゲットカメラ
					//今のカメラのターゲット取得
					std::weak_ptr<CameraBase> currentCamera = CAMERAMGR.GetGameCamera();
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

					std::shared_ptr<PointTargetCamera> camera = std::make_shared<PointTargetCamera>();
					camera->Init(targetPos);
					SCENEMGR.AddObject(camera);

					CAMERAMGR.SetDebugCamera(camera);
					CAMERAMGR.SetDefaultCamera(CameraType::Debug);

					// 選択中のindexを合わせる
					currentStageNo = SCENEMGR.GetStageNo();
					currentLaneNo = BowlingSystemConsts::StartFrame;

					// ★ LoadStage(int stageNo) を使用
					if (STAGEMGR.LoadStage(currentStageNo))
					{
						STAGEMGR.BuildStage(currentLaneNo);
					}
				}
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
		isEditWindow = false;
		STAGEMGR.SetMode(StageMode::Play);
		SCENEMGR.SetGameSpeed(1.0f);
		CAMERAMGR.SetDefaultCamera(CameraType::Game);
	}

	if (isEditWindow)
	{
		// カメラ取得
		std::weak_ptr<CameraBase> parent = CAMERAMGR.GetDebugCamera();
		std::shared_ptr<PointTargetCamera> camera;
		if (!parent.expired())
		{
			camera = std::dynamic_pointer_cast<PointTargetCamera>(parent.lock());

			//取得成功時
			if (camera)
			{
				Math::Vector3 currentView = camera->GetCurrentViewPoint();
				Math::Vector3 move = Math::Vector3::Zero;

				//カメラ移動
				if (GetAsyncKeyState(VK_UP) & 0x8000)
				{
					move.z += 1.0f;
				}
				if (GetAsyncKeyState(VK_DOWN) & 0x8000)
				{
					move.z -= 1.0f;
				}
				if (GetAsyncKeyState(VK_LEFT) & 0x8000)
				{
					move.x -= 1.0f;
				}
				if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
				{
					move.x += 1.0f;
				}
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					move.y += 1.0f;
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					move.y -= 1.0f;
				}

				move.Normalize();
				move *= 0.2f;
				camera->MoveCamera(currentView + move);
			}
		}

		// 位置固定
		ImGui::SetNextWindowPos(ImVec2(5.f, 5.f), ImGuiCond_Always);
		if (ImGui::Begin("Stage Editor", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
		{
			// --- A. ステージ番号選択 ---
			ImGui::Text("Stage Select:");

			int maxStageNo = STAGEMGR.GetMaxStageNo();
			for (int stage = STAGEMGR.GetMinStageNo(); stage <= maxStageNo; ++stage)
			{
				if (stage > STAGEMGR.GetMinStageNo()) ImGui::SameLine();

				std::string stageLabel = std::to_string(stage) + "##stage";
				bool isSelected = (currentStageNo == stage);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
				}

				if (ImGui::Button(stageLabel.c_str(), ImVec2(30, 0)))
				{
					currentStageNo = stage;

					if (STAGEMGR.LoadStage(currentStageNo))
					{
						// 内部インデックス(0〜9)をそのまま渡す
						STAGEMGR.BuildStage(currentLaneNo);
					}
				}

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}
			}

			// --- 仕切り ---
			ImGui::Separator();

			// --- B. レーン番号選択 (内部インデックス 0〜9 管理) ---
			ImGui::Text("Lane Select (1 - 10):");

			for (int lane = 0; lane < 10; ++lane)
			{
				// 0番目と5番目（UI上の1と6）以外は横に並べる
				if (lane != 0 && lane != 5) ImGui::SameLine();

				// ★ 表示テキストのみ +1（"1" 〜 "10"）
				std::string laneLabel = std::to_string(lane + 1) + "##lane";
				bool isSelected = (currentLaneNo == lane);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
				}

				if (ImGui::Button(laneLabel.c_str(), ImVec2(40, 0)))
				{
					currentLaneNo = lane; // 内部値は 0〜9

					if (STAGEMGR.LoadStage(currentStageNo))
					{
						// 計算不要でそのままインデックスを渡す
						STAGEMGR.BuildStage(currentLaneNo);
					}
				}

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}
			}

			// ★ 表示時のみ currentLaneNo + 1
			ImGui::Spacing();
			ImGui::TextDisabled("Current Target: Stage %d / Lane %d (Index: %d)",
				currentStageNo, currentLaneNo + 1, currentLaneNo);
		}

		// -----------------------------------------------------------------
		// 2. Save & Load 階層
		// -----------------------------------------------------------------
		if (ImGui::CollapsingHeader("Save & Load"))
		{
			ImGui::Text("Target Stage: Stage %02d", currentStageNo);
			ImGui::Separator();

			// --- A. セーブボタン ---
			if (ImGui::Button("Save Stage", ImVec2(90, 30)))
			{
				char filePath[64];
				snprintf(filePath, sizeof(filePath), "Asset/Data/StageData/Stage%02d.json", currentStageNo);

				STAGEMGR.SaveStage(filePath);
			}

			ImGui::SameLine();

			// --- B. ロード（リセット）ボタン ---
			if (ImGui::Button("Load Stage", ImVec2(90, 30)))
			{
				if (STAGEMGR.LoadStage(currentStageNo))
				{
					// 内部インデックス(0〜9)をそのまま渡す
					STAGEMGR.BuildStage(currentLaneNo);
				}
			}

			// --- 仕切り ---
			ImGui::Separator();

			// --- C. コピーメニュー ---
			if (ImGui::TreeNode("Copy Stage"))
			{
				// 値持ち
				static int copyLaneNo = 0;

				// --- レーン番号選択 (内部インデックス 0〜9 管理) ---
				ImGui::Text("Lane Select (1 - 10):");

				for (int lane = 0; lane < 10; ++lane)
				{
					// 0番目と5番目（UI上の1と6）以外は横に並べる
					if (lane != 0 && lane != 5) ImGui::SameLine();

					// ★ 表示テキストのみ +1（"1" 〜 "10"）
					std::string laneLabel = std::to_string(lane + 1) + "##laneCP";
					bool isSelected = (copyLaneNo == lane);

					if (isSelected)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
					}

					if (ImGui::Button(laneLabel.c_str(), ImVec2(40, 0)))
					{
						copyLaneNo = lane;
					}

					if (isSelected)
					{
						ImGui::PopStyleColor();
					}
				}

				// --- 仕切り ---
				ImGui::Separator();

				// --- コピーボタン ---
				std::string text = "Copy Lane " + std::to_string(currentLaneNo + 1) + " to Lane " + std::to_string(copyLaneNo + 1);
				if (ImGui::Button(text.c_str(), ImVec2(180, 30)))
				{
					// データ取得
					auto& stageData = STAGEMGR.WorkStageData();

					// コピペ
					stageData.m_stageLaneData[copyLaneNo] = stageData.m_stageLaneData[currentLaneNo];

					// セーブ
					char filePath[64];
					snprintf(filePath, sizeof(filePath), "Asset/Data/StageData/Stage%02d.json", currentStageNo);

					STAGEMGR.SaveStage(filePath);
				}

				ImGui::TreePop();
			}
		}
	
		// -----------------------------------------------------------------
		// 3. StageEdit 階層 (※new トップ階層)
		// -----------------------------------------------------------------
		if (ImGui::CollapsingHeader("StageEdit", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// -------------------------------------------------------------
			// A. 選択ターゲットの一元管理用定義（StageEdit 内または静的変数）
			// -------------------------------------------------------------
			enum class SelectedTargetCategory
			{
				None,
				Gimmick,
				Pin,
				Player
			};

			static SelectedTargetCategory selectedCategory = SelectedTargetCategory::None;
			static int selectedIndex = -1;

			auto& stageData = STAGEMGR.WorkStageData();
			auto& currentFrame = stageData.m_stageLaneData[currentLaneNo];

			// --- A. StageEdit 直下で選択状態の変化をまとめて監視する変数 ---
			static int lastStageNo = -1;
			static int lastLaneNo = -1;

			// --- B. 各入力用バッファとエラー状態 ---
			static std::string skyPathInput = "";
			static bool isSkyError = false;

			static std::string terrainPathInput = "";
			static bool isTerrainError = false;

			// --- C. ステージまたはレーンが切り替わった場合の一括同期処理 ---
			if (lastStageNo != currentStageNo || lastLaneNo != currentLaneNo)
			{
				skyPathInput = stageData.m_skyPath;
				isSkyError = false;

				terrainPathInput = currentFrame.m_terrainPath;
				isTerrainError = false;

				selectedCategory = SelectedTargetCategory::None;
				selectedIndex = -1;

				lastStageNo = currentStageNo;
				lastLaneNo = currentLaneNo;
			}

			// -------------------------------------------------------------
			// 3-1. Sky Asset Settings (StageEdit 直下)
			// -------------------------------------------------------------
			if (ImGui::TreeNode("Sky Asset Settings"))
			{
				ImGui::Text("Current Sky Path: %s", stageData.m_skyPath.c_str());
				ImGui::InputText("Model Path##SkyInput", &skyPathInput);

				if (isSkyError)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
				}

				if (ImGui::Button("Apply Sky Path", ImVec2(150, 0)))
				{
					if (std::filesystem::exists(skyPathInput))
					{
						isSkyError = false;
						stageData.m_skyPath = skyPathInput;

						// 構造変更のため BuildStage
						STAGEMGR.BuildStage(currentLaneNo);
					}
					else
					{
						isSkyError = true;
					}
				}

				ImGui::TreePop();
			}

			// ★ セクション間の区切り
			ImGui::Separator();

			// -------------------------------------------------------------
			// 3-2. Lane Placement Settings (※1) (StageEdit 直下)
			// -------------------------------------------------------------
			if (ImGui::TreeNode("Lane Placement Settings (※1)"))
			{
				// ★ 各要素の区切り
				ImGui::Separator();

				// --- A. Lane Terrain Asset Path ---
				if (ImGui::TreeNode("Lane Terrain Asset Path"))
				{
					ImGui::Text("Current Terrain Path: %s", currentFrame.m_terrainPath.c_str());
					ImGui::InputText("Model Path##TerrainInput", &terrainPathInput);

					if (isTerrainError)
					{
						ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
					}

					if (ImGui::Button("Apply Terrain Path", ImVec2(150, 0)))
					{
						if (std::filesystem::exists(terrainPathInput))
						{
							isTerrainError = false;
							currentFrame.m_terrainPath = terrainPathInput;

							// 構造変更のため BuildStage
							STAGEMGR.BuildStage(currentLaneNo);
						}
						else
						{
							isTerrainError = true;
						}
					}

					ImGui::TreePop();
				}

				// ★ 各要素の区切り
				ImGui::Separator();

				// -------------------------------------------------------------
				// --- B. Gimmick Management ---
				// -------------------------------------------------------------
				if (ImGui::TreeNode("Gimmick Management"))
				{
					auto& gimmicks = currentFrame.m_laneGimmickData;

					// 1. ギミック一覧リストボックス
					ImGui::BeginChild("GimmickListChild", ImVec2(0, 120), true);
					for (int i = 0; i < static_cast<int>(gimmicks.size()); ++i)
					{
						std::string label = "[" + std::to_string(i) + "] Gimmick " + std::to_string(i + 1);

						// カテゴリとインデックスが両方一致している場合のみハイライト
						bool isSelected = (selectedCategory == SelectedTargetCategory::Gimmick && selectedIndex == i);

						if (ImGui::Selectable(label.c_str(), isSelected))
						{
							// 選択カテゴリとインデックスを更新（他カテゴリの選択は自動解除）
							selectedCategory = SelectedTargetCategory::Gimmick;
							selectedIndex = i;
						}
					}
					ImGui::EndChild();

					// 2. 範囲チェック（ギミック削除等でオーバーした場合の安全対策）
					if (selectedCategory == SelectedTargetCategory::Gimmick)
					{
						if (gimmicks.empty())
						{
							selectedCategory = SelectedTargetCategory::None;
							selectedIndex = -1;
						}
						else if (selectedIndex >= static_cast<int>(gimmicks.size()))
						{
							selectedIndex = static_cast<int>(gimmicks.size()) - 1;
						}
					}

					// 3. 選択中ギミックの編集UI
					if (selectedCategory == SelectedTargetCategory::Gimmick && selectedIndex >= 0 && selectedIndex < static_cast<int>(gimmicks.size()))
					{
						ImGui::Separator();
						ImGui::Text("Edit Gimmick [%d]", selectedIndex + 1);

						auto& gimmick = gimmicks[selectedIndex];
						bool isChanged = false;

						if (ImGui::DragFloat3("Position", &gimmick.m_data.m_position.x, 0.01f)) isChanged = true;
						if (ImGui::DragFloat3("Rotation", &gimmick.m_data.m_rotation.x, 0.1f))  isChanged = true;
						if (ImGui::DragFloat3("Scale", &gimmick.m_data.m_scale.x, 0.01f)) isChanged = true;

						if (isChanged)
						{
							STAGEMGR.BuildStage(currentLaneNo);
						}

						if (ImGui::Button("Delete Selected Gimmick", ImVec2(180, 0)))
						{
							gimmicks.erase(gimmicks.begin() + selectedIndex);

							// 削除後は選択解除
							selectedCategory = SelectedTargetCategory::None;
							selectedIndex = -1;

							STAGEMGR.BuildStage(currentLaneNo);
						}
					}

					ImGui::TreePop();
				}

				ImGui::Separator();

				// -------------------------------------------------------------
				// --- C. Pin List (10 Pins) ---
				// -------------------------------------------------------------
				if (ImGui::TreeNode("Pin List (10 Pins)"))
				{
					auto& pins = currentFrame.m_lanePinData;

					// 1. ピン一覧リストボックス
					ImGui::BeginChild("PinListChild", ImVec2(0, 120), true);
					for (int i = 0; i < static_cast<int>(pins.size()); ++i)
					{
						std::string label = "[" + std::to_string(i) + "] Pin " + std::to_string(i + 1);

						// カテゴリとインデックスが両方一致している場合のみハイライト
						bool isSelected = (selectedCategory == SelectedTargetCategory::Pin && selectedIndex == i);

						if (ImGui::Selectable(label.c_str(), isSelected))
						{
							// 選択カテゴリとインデックスを更新（他カテゴリの選択は自動解除）
							selectedCategory = SelectedTargetCategory::Pin;
							selectedIndex = i;
						}
					}
					ImGui::EndChild();

					// 2. 範囲チェック
					if (selectedCategory == SelectedTargetCategory::Pin)
					{
						if (pins.empty())
						{
							selectedCategory = SelectedTargetCategory::None;
							selectedIndex = -1;
						}
						else if (selectedIndex >= static_cast<int>(pins.size()))
						{
							selectedIndex = static_cast<int>(pins.size()) - 1;
						}
					}

					// 3. 選択中ピンの編集UI
					if (selectedCategory == SelectedTargetCategory::Pin && selectedIndex >= 0 && selectedIndex < static_cast<int>(pins.size()))
					{
						ImGui::Separator();
						ImGui::Text("Edit Pin [%d]", selectedIndex + 1);

						auto& pin = pins[selectedIndex];
						bool isChanged = false;

						if (ImGui::DragFloat3("Position", &pin.m_data.m_position.x, 0.01f)) isChanged = true;
						if (ImGui::DragFloat3("Rotation", &pin.m_data.m_rotation.x, 0.1f))  isChanged = true;
						if (ImGui::DragFloat3("Scale", &pin.m_data.m_scale.x, 0.01f)) isChanged = true;

						if (isChanged)
						{
							STAGEMGR.RespawnStage(currentLaneNo);
						}
					}

					ImGui::TreePop();
				}

				ImGui::Separator();

				// -------------------------------------------------------------
				// --- D. Player Transform Settings ---
				// -------------------------------------------------------------
				if (ImGui::TreeNode("Player Transform Settings"))
				{
					auto& player = currentFrame.m_playerData;

					// 1. サイズ 1 の選択リストボックス（高さ 35px 程度で固定）
					ImGui::BeginChild("PlayerListChild", ImVec2(0, 35), true);

					bool isSelected = (selectedCategory == SelectedTargetCategory::Player);
					if (ImGui::Selectable("[Player] Start Position", isSelected))
					{
						selectedCategory = SelectedTargetCategory::Player;
						selectedIndex = 0; // プレイヤーは単一要素のため 0 固定
					}
					ImGui::EndChild();

					// 2. 選択中プレイヤーの編集UI
					if (selectedCategory == SelectedTargetCategory::Player)
					{
						ImGui::Separator();
						ImGui::Text("Edit Player Transform");

						bool isChanged = false;

						if (ImGui::DragFloat3("Position", &player.m_position.x, 0.01f)) isChanged = true;
						if (ImGui::DragFloat3("Rotation", &player.m_rotation.x, 0.1f))  isChanged = true;

						if (isChanged)
						{
							STAGEMGR.RespawnStage(currentLaneNo);
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop(); // Lane Placement Settings (※1) の TreePop

				//右クリックで現在選択中のオブジェクトをカーソル位置に飛ばす
				if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
				{
					// マウス座標(2D)を3D座標へ変換
					if (camera)
					{
						// 手順①
						// マウス座標を取得
						POINT _mousePos;
						GetCursorPos(&_mousePos);
						ScreenToClient(Application::Instance().GetWindowHandle(), &_mousePos);

						// 手順②
						// マウスの2D座標を3D座標へ変換する
						Math::Vector3	rayPos = camera->GetCurrentViewPoint();
						Math::Vector3	rayDir = Math::Vector3::Zero;
						float			range = 2000.f;
						camera->WorkCamera()->GenerateRayInfoFromClientPos(_mousePos, rayPos, rayDir, range);

						// Jolt Physics への RayCast
						JPH::RRayCast rayCast;
						rayCast.mOrigin = JPH::RVec3(rayPos.x, rayPos.y, rayPos.z);
						rayCast.mDirection = JPH::Vec3(rayDir.x, rayDir.y, rayDir.z) * range; // 飛ばす長さ
						GroundObjectFilter groundFilter;

						JPH::RayCastResult hit;
						bool hasHit = PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastRay(rayCast, hit, {}, groundFilter);

						if (hasHit)
						{
							// ★レイが何かに当たった！その「衝突点の3D座標」を取得
							JPH::RVec3 hitPos = rayCast.GetPointOnRay(hit.mFraction);
							Math::Vector3 hitVec3Pos = Math::Vector3(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());

							// 選択中の種類と Index から直接データを特定して移動
							if (selectedCategory == SelectedTargetCategory::Gimmick)
							{
								if (selectedIndex >= 0 && selectedIndex < static_cast<int>(currentFrame.m_laneGimmickData.size()))
								{
									auto& gimmick = currentFrame.m_laneGimmickData[selectedIndex];
									gimmick.m_data.m_position = DirectX::XMFLOAT3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

									STAGEMGR.BuildStage(currentLaneNo);
								}
							}
							else if (selectedCategory == SelectedTargetCategory::Pin)
							{
								if (selectedIndex >= 0 && selectedIndex < static_cast<int>(currentFrame.m_lanePinData.size()))
								{
									auto& pin = currentFrame.m_lanePinData[selectedIndex];
									pin.m_data.m_position = DirectX::XMFLOAT3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

									STAGEMGR.RespawnStage(currentLaneNo);
								}
							}
							else if (selectedCategory == SelectedTargetCategory::Player)
							{
								auto& player = currentFrame.m_playerData;
								player.m_position = DirectX::XMFLOAT3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

								STAGEMGR.RespawnStage(currentLaneNo);
							}
						}
					}
				}

				// -------------------------------------------------------------
				// E. 選択オブジェクトの位置を取得して DebugSphere に表示
				// -------------------------------------------------------------
				Math::Vector3 debugPos = { 0.0f, 0.0f, 0.0f };
				bool hasSelection = false;

				if (selectedCategory == SelectedTargetCategory::Gimmick && selectedIndex >= 0 && selectedIndex < static_cast<int>(currentFrame.m_laneGimmickData.size()))
				{
					debugPos = currentFrame.m_laneGimmickData[selectedIndex].m_data.m_position;
					hasSelection = true;
				}
				else if (selectedCategory == SelectedTargetCategory::Pin && selectedIndex >= 0 && selectedIndex < static_cast<int>(currentFrame.m_lanePinData.size()))
				{
					debugPos = currentFrame.m_lanePinData[selectedIndex].m_data.m_position;
					hasSelection = true;
				}
				else if (selectedCategory == SelectedTargetCategory::Player)
				{
					debugPos = currentFrame.m_playerData.m_position;
					hasSelection = true;
				}

				// STAGEMGR に選択中の座標を渡して DebugSphere を表示
				STAGEMGR.SetDebugOutlinePos(hasSelection, debugPos);
			}
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
	static bool isLog = false;
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

	static bool isCursor = false;
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

void KdDebugGUI::GuiRelease()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
