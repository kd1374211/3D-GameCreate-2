#include "StageEditor.h"
#include "../../../../Application/Const/Function.h"
#include "../../../../Application/StageManager/StageManager.h"
#include "../../../../Application/Physics/PhysicsManager.h"
#include "../../../../Application/GameObject/Camera/CameraManager.h"
#include "../../../../Application/GameObject/Camera/PointTargetCamera/PointTargetCamera.h"
#include "../../../../Application/main.h"

void StageEditor::UpdateStageEditor(int& currentStageNo, int& currentLaneNo, std::shared_ptr<PointTargetCamera> camera)
{

	// 位置固定
	ImGui::SetNextWindowPos(ImVec2(5.f, 5.f), ImGuiCond_Always);
	if (ImGui::Begin("Stage Editor", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
	{
		// ステージデータ取得
		auto& stageData = STAGEMGR.WorkStageData();

		// 編集ステージ・レーン切り替え
		UpdateFrameSelection(currentStageNo, currentLaneNo, stageData, camera);

		// セーブ・ロード・コピー
		UpdateSaveAndLoad(currentStageNo, currentLaneNo, stageData);

		// ステージ内Edit
		UpdateStageEdit(currentStageNo, currentLaneNo, stageData);
	}
}

void StageEditor::UpdateFrameSelection(int& currentStageNo, int& currentLaneNo, StageOverallData& stageData, std::shared_ptr<PointTargetCamera> camera)
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
				STAGEMGR.BuildAllObjects(currentLaneNo);

				// PTカメラの位置をプレイヤー位置へ
				if (camera)
				{
					// プレイヤー座標取得
					const auto playerPos = stageData.m_stageLaneData[currentLaneNo].m_playerData.m_position;

					camera->MoveTargetPoint(playerPos);
				}
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
				STAGEMGR.BuildAllObjects(currentLaneNo);

				// PTカメラの位置をプレイヤー位置へ
				if (camera)
				{
					// プレイヤー座標取得
					const auto playerPos = stageData.m_stageLaneData[currentLaneNo].m_playerData.m_position;

					camera->MoveTargetPoint(playerPos);
				}
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

void StageEditor::UpdateSaveAndLoad(int currentStageNo, int currentLaneNo, StageOverallData& stageData)
{
	if (ImGui::CollapsingHeader("Save & Load"))
	{
		ImGui::Text("Target Stage: Stage %02d", currentStageNo);
		ImGui::Separator();

		// --- A. セーブボタン ---
		if (ImGui::Button("Save Stage", ImVec2(90, 30)))
		{
			STAGEMGR.SaveStage(currentStageNo);
		}

		ImGui::SameLine();

		// --- B. ロード（リセット）ボタン ---
		if (ImGui::Button("Load Stage", ImVec2(90, 30)))
		{
			if (STAGEMGR.LoadStage(currentStageNo))
			{
				// 内部インデックス(0〜9)をそのまま渡す
				STAGEMGR.BuildAllObjects(currentLaneNo);
			}
		}

		// --- 仕切り ---
		ImGui::Separator();

		// --- C. コピーメニュー ---
		if (ImGui::TreeNode("Copy Stage"))
		{
			// 値持ち
			static int copyLaneNo = 0;
			static int copyStageNo = 1;

			// ステージ番号選択
			ImGui::Text("Stage Select:");

			for (int stage = STAGEMGR.GetMinStageNo(); stage <= STAGEMGR.GetMaxStageNo(); ++stage)
			{
				if (stage > STAGEMGR.GetMinStageNo()) ImGui::SameLine();

				std::string stageLabel = std::to_string(stage) + "##stage";
				bool isSelected = (copyStageNo == stage);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
				}

				if (ImGui::Button(stageLabel.c_str(), ImVec2(40, 0)))
				{
					copyStageNo = stage;
				}

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}
			}

			ImGui::Separator();

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
			std::string text = "Copy Stage " + std::to_string(currentStageNo) + "-" + std::to_string(currentLaneNo + 1) + " to Stage " + std::to_string(copyStageNo) + "-" + std::to_string(copyLaneNo + 1);
			if (ImGui::Button(text.c_str(), ImVec2(240, 30)))
			{
				// 同じステージ内のコピーかどうかを確認
				if (currentStageNo == copyStageNo)
				{
					// コピペ
					stageData.m_stageLaneData[copyLaneNo] = stageData.m_stageLaneData[currentLaneNo];

					// セーブ
					STAGEMGR.SaveStage(currentStageNo);
				}
				// 別ステージ間の場合
				else
				{
					// 値を保持
					StageLaneData copyData = stageData.m_stageLaneData[currentLaneNo];
					
					// ステージを切り替え
					if (STAGEMGR.LoadStage(copyStageNo))
					{
						// 切り替え先の対象レーンに代入
						stageData.m_stageLaneData[copyLaneNo] = copyData;

						// セーブ
						STAGEMGR.SaveStage(copyStageNo);
					}

					// ステージを戻す
					STAGEMGR.LoadStage(currentStageNo);
				}
				
			}

			ImGui::TreePop();
		}
	}
}

void StageEditor::UpdateStageEdit(int currentStageNo, int currentLaneNo, StageOverallData& stageData)
{
	if (ImGui::CollapsingHeader("StageEdit", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// --- A. StageEdit 直下で選択状態の変化をまとめて監視する変数 ---
		static int lastStageNo = -1;
		static int lastLaneNo = -1;

		// --- B. 各入力用バッファとエラー状態 ---
		static std::string skyPathInput = "";
		static bool isSkyError = false;

		static std::string terrainPathInput_Stage = "1";
		static std::string terrainPathInput_Frame = "1";
		static bool isTerrainError = false;

		// --- C. ステージまたはレーンが切り替わった場合の一括同期処理 ---
		if (lastStageNo != currentStageNo || lastLaneNo != currentLaneNo)
		{
			skyPathInput = stageData.m_skyPath;
			isSkyError = false;

			terrainPathInput_Stage = std::to_string(currentStageNo);
			terrainPathInput_Frame = std::to_string(currentLaneNo + 1);
			isTerrainError = false;

			// 初期値に戻す
			ChangeSelectedObject();

			lastStageNo = currentStageNo;
			lastLaneNo = currentLaneNo;
		}

		// 天球
		UpdateSkyAssetSettings(skyPathInput, isSkyError, stageData);

		// 区切り
		ImGui::Separator();

		// 島のデコレーション
		UpdateIslandAndDecorations(stageData, currentLaneNo);

		ImGui::Separator();

		// ステージ内編集
		UpdateFrameObjectsSetting(currentLaneNo, terrainPathInput_Stage, terrainPathInput_Frame, isTerrainError, stageData);
	}
}

void StageEditor::UpdateSkyAssetSettings(std::string& skyAssetPathStr, bool& isError, StageOverallData& stageData)
{
	if (ImGui::TreeNode("Sky Asset Settings"))
	{
		ImGui::Text("Current Sky Path: %s", stageData.m_skyPath.c_str());
		ImGui::InputText("Model Path##SkyInput", &skyAssetPathStr);

		if (isError)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
		}

		if (ImGui::Button("Apply Sky Path", ImVec2(150, 0)))
		{
			if (std::filesystem::exists(skyAssetPathStr))
			{
				isError = false;
				stageData.m_skyPath = skyAssetPathStr;

				// 空の再生成
				STAGEMGR.BuildSky();
			}
			else
			{
				isError = true;
			}
		}

		ImGui::TreePop();
	}
}

void StageEditor::UpdateIslandAndDecorations(StageOverallData& stageData, int currentLaneNumber)
{
	bool isIslandOrDecorationChanged = false;

	if (ImGui::TreeNode("Island Management"))
	{
		static std::string islandModelPathInput = stageData.m_islandPath;
		static bool isIslandModelError = false;

		ImGui::Text("Current Model Path: %s", stageData.m_islandPath.c_str());
		ImGui::InputText("Model Path##DecoInput", &islandModelPathInput);
		if (ImGui::ArrowButton("##DropdownArrow", ImGuiDir_Down))
		{
			ImGui::OpenPopup("##DropdownPopup");
		}

		if (isIslandModelError)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
		}

		// リスト選択機能を追加
		if (ImGui::BeginPopup("##DropdownPopup"))
		{
			for (const auto& item : m_islandPathData)
			{
				if (ImGui::Selectable(item.m_name.c_str(), false))
				{
					// 選択に合わせてパスを設定
					islandModelPathInput = m_baseIslandPath + item.m_path;
					stageData.m_islandPath = islandModelPathInput;

					// 変更を取得
					isIslandOrDecorationChanged = true;
				}
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Apply Model Path", ImVec2(150, 0)))
		{
			if (std::filesystem::exists(islandModelPathInput))
			{
				isIslandModelError = false;
				stageData.m_islandPath = islandModelPathInput;

				// 変更を取得
				isIslandOrDecorationChanged = true;
			}
			else
			{
				isIslandModelError = true;
			}
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	// カテゴリを定義
	const SelectedTargetCategory category = SelectedTargetCategory::Decoration;

	if (ImGui::TreeNode("Decoration Management"))
	{
		auto& decorations = stageData.m_decorationData;

		// --- 追加ボタンエリア ---
		if (ImGui::Button("+ Add Decoration", ImVec2(140, 0)))
		{
			IslandDecorationData newDeco;
			newDeco.m_modelPath = m_baseDecorationPath + m_decoPathData.begin()->m_islandType + m_decoPathData.begin()->m_path;
			newDeco.m_srtData = {};
			for (int i = BowlingSystemConsts::StartFrame; i < BowlingSystemConsts::FrameCount; i++)
			{
				newDeco.m_isActive[i] = i == currentLaneNumber;
			}

			decorations.push_back(newDeco);

			// 追加したデコレーションを選択状態にする
			ChangeSelectedObject(category, decorations.size() - 1);

			// 変更を取得
			isIslandOrDecorationChanged = true;
		}

		ImGui::Spacing();

		// 1. デコレーション一覧リストボックス
		ImGui::BeginChild("DecorationListChild", ImVec2(0, 120), true);
		for (int i = 0; i < static_cast<int>(decorations.size()); ++i)
		{
			// 番号をラベルに
			std::string label = "[" + std::to_string(i) + "] ";

			// カテゴリとインデックスが両方一致している場合のみハイライト
			bool isSelected = CheckCategoryAndIndex(category, i);

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				// 選択カテゴリとインデックスを更新（他カテゴリの選択は自動解除）
				ChangeSelectedObject(category, i);
			}
		}
		ImGui::EndChild();

		// 2. 範囲チェック（削除等でオーバーした場合の安全対策）
		if (CheckCategory(category))
		{
			if (decorations.empty())
			{
				// 選択オブジェクトを初期化
				ChangeSelectedObject();
			}
			else if (m_selectedIndex >= static_cast<int>(decorations.size()))
			{
				m_selectedIndex = static_cast<int>(decorations.size()) - 1;
			}

		}

		static std::string decoModelPathInput = "";
		static bool isDecoModelError = false;
		static int lastSelectedIndex = -1; // 選択切り替え検知用

		// 3. 選択中デコレーションの編集UI
		if (CheckCategory(category) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(decorations.size()))
		{
			// 選択項目が切り替わったら入力バッファとエラー状態を同期・リセット
			if (lastSelectedIndex != m_selectedIndex)
			{
				lastSelectedIndex = m_selectedIndex;
				isDecoModelError = false;

				decoModelPathInput = decorations[m_selectedIndex].m_modelPath;
			}

			ImGui::Separator();
			ImGui::Text("Edit Deco [%d]", m_selectedIndex + 1);

			auto& decoration = decorations[m_selectedIndex];

			ImGui::Text("Current Model Path: %s", decoration.m_modelPath.c_str());
			ImGui::InputText("Model Path##DecoInput", &decoModelPathInput);
			if (ImGui::ArrowButton("##DropdownArrow", ImGuiDir_Down))
			{
				ImGui::OpenPopup("##DropdownPopup");
			}

			if (isDecoModelError)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
			}

			// リスト選択機能を追加
			if (ImGui::BeginPopup("##DropdownPopup"))
			{
				for (const auto& item : m_decoPathData)
				{
					if (ImGui::Selectable(item.m_name.c_str(), false))
					{
						// 選択に合わせてパスを設定
						decoModelPathInput = m_baseDecorationPath + item.m_islandType + item.m_path;
						decoration.m_modelPath = decoModelPathInput;

						// 変更を取得
						isIslandOrDecorationChanged = true;
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Apply Model Path", ImVec2(150, 0)))
			{
				if (std::filesystem::exists(decoModelPathInput))
				{
					isDecoModelError = false;
					decoration.m_modelPath = decoModelPathInput;

					// 変更を取得
					isIslandOrDecorationChanged = true;
				}
				else
				{
					isDecoModelError = true;
				}
			}

			// SRT編集
			if (ImGui::DragFloat3("Position", &decoration.m_srtData.m_position.x, 0.01f)) isIslandOrDecorationChanged = true;
			if (ImGui::DragFloat3("Rotation", &decoration.m_srtData.m_rotation.x, 0.1f, 0.0f, 0.0f, "%.1f deg"))isIslandOrDecorationChanged = true;
			if (ImGui::DragFloat3("Scale", &decoration.m_srtData.m_scale.x, 0.01f)) isIslandOrDecorationChanged = true;

			ImGui::Separator();

			// 1. 複製ボタン（Duplicate / Copy）
			if (ImGui::Button("Duplicate Selected", ImVec2(200, 0)))
			{
				// 選択中のギミックデータを深層コピー
				auto& duplicateDecoration = decorations[m_selectedIndex];

				// （任意）重なって見づらくなるのを防ぐため座標を少しだけずらす
				duplicateDecoration.m_srtData.m_position.x += 0.5f;

				// 配列に追加
				decorations.push_back(duplicateDecoration);

				// 選択インデックスを新しく生成された末尾のギミックに変更
				ChangeSelectedObject(category, decorations.size() - 1);

				// 入力バッファやエラー状態をリセット
				lastSelectedIndex = m_selectedIndex;
				isDecoModelError = false;
				decoModelPathInput = decorations[m_selectedIndex].m_modelPath;

				// 変更を取得
				isIslandOrDecorationChanged = true;
			}

			ImGui::SameLine();

			// 2. 既存の削除ボタン
			if (ImGui::Button("Delete Selected"))
			{
				decorations.erase(decorations.begin() + m_selectedIndex);
				ChangeSelectedObject(category);

				// 変更を取得
				isIslandOrDecorationChanged = true;
			}
		}

		ImGui::TreePop();
	}

	// 値が更新された場合のみ再構築
	if (isIslandOrDecorationChanged)
	{
		STAGEMGR.BuildIsland(currentLaneNumber);
	}
}

void StageEditor::UpdateFrameObjectsSetting(int currentLaneNo, std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageOverallData& stageData)
{
	if (ImGui::TreeNode("Lane Placement Settings"))
	{
		// レーンデータ
		auto& frameData = stageData.m_stageLaneData[currentLaneNo];

		// ★ 各要素の区切り
		ImGui::Separator();

		// メイン地形
		if (UpdateLaneAssetPath(terrainPath_Stage, terrainPath_Frame, isError, frameData))
		{
			// 変更を確認したら地形再生成
			STAGEMGR.BuildTerrain(currentLaneNo);
		}

		// ★ 各要素の区切り
		ImGui::Separator();

		// 島デコレーション
		if (UpdateIslandDecorationsVisibility(currentLaneNo, stageData))
		{
			// 変更を確認したので再生成
			STAGEMGR.CheckIslandDecoActive(currentLaneNo);
		}

		// ★ 各要素の区切り
		ImGui::Separator();

		// ギミック
		if (UpdateGimmickSetting(frameData))
		{
			// 変更を確認したので再生成
			STAGEMGR.BuildGimmicks(currentLaneNo);
		}

		ImGui::Separator();

		// ピン
		if (UpdatePinsSetting(frameData))
		{
			// 変更を確認したので再生成
			STAGEMGR.BuildPins(currentLaneNo);
		}

		ImGui::Separator();

		// プレイヤー
		if (UpdatePlayerSetting(frameData))
		{
			// 変更を確認したので再生成
			STAGEMGR.BuildPlayer(currentLaneNo);
		}

		ImGui::TreePop(); // Lane Placement Settings (※1) の TreePop
	}

	// 右クリックで現在選択中のオブジェクトをカーソル位置に飛ばす
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// 移動処理
		MoveSelectedObjectToCursor(stageData, currentLaneNo);
	}

	// 選択中のオブジェクトにDebugOutlineを合わせる
	SetDebugOutlineToSelected(stageData, currentLaneNo);
}

bool StageEditor::UpdateLaneAssetPath(std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageLaneData& frameData)
{
	bool isTerrainAssetChanged = false;

	if (ImGui::TreeNode("Lane Terrain Asset Path"))
	{
		ImGui::Text("Current Terrain Path:\n%s", frameData.m_terrainPath.c_str());

		ImGui::InputText("Stage##TerrainInput", &terrainPath_Stage);
		ImGui::InputText("Frame##TerrainInput", &terrainPath_Frame);

		if (isError)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
		}

		if (ImGui::Button("Apply Terrain Path", ImVec2(150, 0)))
		{
			// パスを作成
			std::string BasePath = "Asset/Models/Terrain/MainTerrain/";
			std::string StagePath = "Stage" + terrainPath_Stage;
			// フレームの桁数チェック
			bool m_isFrameOneDig = terrainPath_Frame.size() == 1;
			std::string FramePath = StagePath + "-" + (m_isFrameOneDig ? "0" : "") + terrainPath_Frame;
			std::string TerrainPath = BasePath + StagePath + "/" + FramePath + "/" + FramePath + ".gltf";

			if (std::filesystem::exists(TerrainPath))
			{
				isError = false;
				frameData.m_terrainPath = TerrainPath;

				// 変更した
				isTerrainAssetChanged = true;
			}
			else
			{
				isError = true;
			}
		}

		ImGui::TreePop();
	}

	// 変更したかを返す
	return isTerrainAssetChanged;
}

bool StageEditor::UpdateIslandDecorationsVisibility(int currentLaneNo, StageOverallData& stageData)
{
	// 変更検知用
	bool isDecorationChanged = false;

	if (ImGui::TreeNode("Island Decorations Active"))
	{
		// デコレーションデータ
		auto& decorations = stageData.m_decorationData;

		// 1. 全選択 / 全解除ボタン
		if (ImGui::Button("Activate All"))
		{
			for (auto& decoration : decorations)
			{
				decoration.m_isActive[currentLaneNo] = true;
			}
			isDecorationChanged = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Deactivate All"))
		{
			for (auto& decoration : decorations)
			{
				decoration.m_isActive[currentLaneNo] = false;
			}
			isDecorationChanged = true;
		}

		ImGui::Spacing();

		// 2. デコレーション一覧リストボックス
		ImGui::BeginChild("DecorationListChild", ImVec2(0, 120), true);
		for (int i = 0; i < static_cast<int>(decorations.size()); ++i)
		{
			// 番号をラベルに
			std::string label = "[" + std::to_string(i) + "] ";

			// デコレーションタグ名
			ImGui::Text(label.c_str());

			ImGui::SameLine();

			// チェックボックスの描画
			std::string checkboxLabel = "Object Active##" + std::to_string(i);
			if (ImGui::Checkbox(checkboxLabel.c_str(), &decorations[i].m_isActive[currentLaneNo]))isDecorationChanged = true;
		}
		ImGui::EndChild();

		ImGui::TreePop();
	}

	// 変更したかを返す
	return isDecorationChanged;
}

bool StageEditor::UpdateGimmickSetting(StageLaneData& frameData)
{
	// カテゴリを定義
	const SelectedTargetCategory category = SelectedTargetCategory::Gimmick;
	bool isGimmickChanged = false;

	if (ImGui::TreeNode("Gimmick Management"))
	{
		auto& gimmicks = frameData.m_laneGimmickData;

		// --- 追加ボタンエリア ---
		if (ImGui::Button("+ Add Finish Area", ImVec2(140, 0)))
		{
			LaneGimmickData newGimmick;
			newGimmick.m_type = "Goal";
			newGimmick.m_param = FinishAreaParams{}; // FinishArea 用パラメータ

			gimmicks.push_back(newGimmick);

			// 追加したギミックを選択状態にする
			ChangeSelectedObject(category, (gimmicks.size()) - 1);

			// 変更を取得
			isGimmickChanged = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("+ Add SpeedUp Area", ImVec2(160, 0)))
		{
			LaneGimmickData newGimmick;
			newGimmick.m_type = "SpeedUp";
			newGimmick.m_param = SpeedUpAreaParams{}; // FinishArea 用パラメータ

			gimmicks.push_back(newGimmick);

			// 追加したギミックを選択状態にする
			ChangeSelectedObject(category, (gimmicks.size()) - 1);

			// 変更を取得
			isGimmickChanged = true;
		}

		if (ImGui::Button("+ Add Moving Terrain", ImVec2(160, 0)))
		{
			LaneGimmickData newGimmick;
			newGimmick.m_type = "MovingTerrain";

			// デフォルトの移動パラメータを設定
			MovingParams moveParam;
			newGimmick.m_param = moveParam;

			gimmicks.push_back(newGimmick);

			// 追加したギミックを選択状態にする
			ChangeSelectedObject(category, (gimmicks.size()) - 1);

			// 変更を取得
			isGimmickChanged = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("+ Add Rotating Terrain", ImVec2(160, 0)))
		{
			LaneGimmickData newGimmick;
			newGimmick.m_type = "RotatingTerrain";

			// デフォルトの回転パラメータを設定
			RotatingParams rotParam;
			newGimmick.m_param = rotParam;

			gimmicks.push_back(newGimmick);

			// 追加したギミックを選択状態にする
			ChangeSelectedObject(category, (gimmicks.size()) - 1);

			// 変更を取得
			isGimmickChanged = true;
		}

		ImGui::Spacing();

		// 1. ギミック一覧リストボックス
		ImGui::BeginChild("GimmickListChild", ImVec2(0, 120), true);
		for (int i = 0; i < static_cast<int>(gimmicks.size()); ++i)
		{
			// タイプ名もラベルに表示して見やすくする
			std::string label = "[" + std::to_string(i) + "] " + gimmicks[i].m_type;

			// カテゴリとインデックスが両方一致している場合のみハイライト
			bool isSelected = CheckCategoryAndIndex(category, i);

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				// 選択カテゴリとインデックスを更新（他カテゴリの選択は自動解除）
				ChangeSelectedObject(category, i);
			}
		}

		ImGui::EndChild();

		// 2. 範囲チェック（ギミック削除等でオーバーした場合の安全対策）
		if (CheckCategory(category))
		{
			if (gimmicks.empty())
			{
				// 選択リセット
				ChangeSelectedObject();
			}
			else if (m_selectedIndex >= static_cast<int>(gimmicks.size()))
			{
				ChangeSelectedObject(category, (gimmicks.size()) - 1);
			}
		}

		static std::string gimmickModelPathInput = "";
		static bool isGimmickModelError = false;
		static int lastSelectedIndex = -1; // 選択切り替え検知用

		// 3. 選択中ギミックの編集UI
		if (CheckCategory(category) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(gimmicks.size()))
		{
			// 選択項目が切り替わったら入力バッファとエラー状態を同期・リセット
			if (lastSelectedIndex != m_selectedIndex)
			{
				lastSelectedIndex = m_selectedIndex;
				isGimmickModelError = false;

				// 選択されたギミックから初期文字列を設定
				if (auto* rot = std::get_if<RotatingParams>(&gimmicks[m_selectedIndex].m_param))
				{
					gimmickModelPathInput = rot->m_modelPath;
				}

				if (auto* mov = std::get_if<MovingParams>(&gimmicks[m_selectedIndex].m_param))
				{
					gimmickModelPathInput = mov->m_modelPath;
				}
			}

			ImGui::Separator();
			ImGui::Text("Edit Gimmick [%d] (%s)", m_selectedIndex + 1, gimmicks[m_selectedIndex].m_type.c_str());

			auto& gimmick = gimmicks[m_selectedIndex];

			// 共通：Transform編集
			if (ImGui::DragFloat3("Position", &gimmick.m_data.m_position.x, 0.01f)) isGimmickChanged = true;
			if (ImGui::DragFloat3("Rotation", &gimmick.m_data.m_rotation.x, 0.1f, 0.0f, 0.0f, "%.1f deg"))isGimmickChanged = true;
			if (ImGui::DragFloat3("Scale", &gimmick.m_data.m_scale.x, 0.01f)) isGimmickChanged = true;

			ImGui::Separator();

			// 固有パラメータ編集
			if (auto* rot = std::get_if<RotatingParams>(&gimmick.m_param))
			{
				ImGui::TextUnformatted("--- Rotating Settings ---");
				ImGui::Text("Current Model Path: %s", rot->m_modelPath.c_str());

				// Sky Asset Settings と同じスタイルでモデルパスを入力
				ImGui::InputText("Model Path##GimmickModelInput", &gimmickModelPathInput);

				if (isGimmickModelError)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
				}

				if (ImGui::Button("Apply Model Path", ImVec2(150, 0)))
				{
					if (std::filesystem::exists(gimmickModelPathInput))
					{
						isGimmickModelError = false;
						rot->m_modelPath = gimmickModelPathInput;
						isGimmickChanged = true;
					}
					else
					{
						isGimmickModelError = true;
					}
				}

				// 回転のピボット
				if (ImGui::DragFloat3("RotatePivot", &rot->m_rotPivotPosOfs.x, 0.01f)) isGimmickChanged = true;

				// 回転速度（リアルタイム反映）
				if (ImGui::DragFloat("Rotate Speed", &rot->m_rotateSpeed, 0.05f, -100.0f, 100.0f))
				{
					isGimmickChanged = true;
				}

				// 回転モード
				static std::string ModeText;
				ModeText = (rot->m_rotateMode == RotatingParams::RotateMode::Circle) ? "Circle" : "Bounce";
				ImGui::Text("Move Pattern Current : %s", ModeText.c_str());

				if (ImGui::Button("Circle", ImVec2(70, 0)))
				{
					rot->m_rotateMode = RotatingParams::RotateMode::Circle;
					ModeText = "Circle";
					isGimmickChanged = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Bounce", ImVec2(70, 0)))
				{
					rot->m_rotateMode = RotatingParams::RotateMode::Bounce;
					rot->m_bounceDeg = 60.0f;
					ModeText = "Bounce";
					isGimmickChanged = true;
				}

				// バウンスなら追加設定
				if (rot->m_rotateMode == RotatingParams::RotateMode::Bounce)
				{
					if (ImGui::DragFloat("Bounce Deg", &rot->m_bounceDeg, 0.05f, 0.1f, 360.0f))
					{
						// 無限対策
						if (rot->m_bounceDeg < 0.1f)rot->m_bounceDeg = 0.1f;

						isGimmickChanged = true;
					}
				}
			}
			else if (auto* mov = std::get_if<MovingParams>(&gimmick.m_param))
			{
				ImGui::TextUnformatted("--- Moving Settings ---");
				ImGui::Text("Current Model Path: %s", mov->m_modelPath.c_str());

				// Sky Asset Settings と同じスタイルでモデルパスを入力
				ImGui::InputText("Model Path##GimmickModelInput", &gimmickModelPathInput);

				if (isGimmickModelError)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[Error] File does not exist!");
				}

				if (ImGui::Button("Apply Model Path", ImVec2(150, 0)))
				{
					if (std::filesystem::exists(gimmickModelPathInput))
					{
						isGimmickModelError = false;
						mov->m_modelPath = gimmickModelPathInput;
						isGimmickChanged = true;
					}
					else
					{
						isGimmickModelError = true;
					}
				}

				// 移動速度（リアルタイム反映）
				if (ImGui::DragFloat("Move Speed", &mov->m_speed, 0.05f, -100.0f, 100.0f))
				{
					isGimmickChanged = true;
				}

				// 移動方法
				static std::string ModeText;
				ModeText = (mov->m_movePattern == MovingParams::MovePattern::Linear) ? "Linear" : "Easing";
				ImGui::Text("Move Pattern Current : %s", ModeText.c_str());

				if (ImGui::Button("Linear", ImVec2(70, 0)))
				{
					mov->m_movePattern = MovingParams::MovePattern::Linear;
					ModeText = "Linear";
					isGimmickChanged = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("Easing", ImVec2(70, 0)))
				{
					mov->m_movePattern = MovingParams::MovePattern::Easing;
					ModeText = "Easing";
					isGimmickChanged = true;
				}

				// 停止時間
				if (ImGui::DragFloat("Stop Time", &mov->m_stopTime, 0.05f, 0.0f, 10.0f))
				{
					isGimmickChanged = true;
				}

				// スタートとゴールの位置
				if (ImGui::DragFloat3("Start Position", &mov->m_startPos.x, 0.01f)) isGimmickChanged = true;
				if (ImGui::DragFloat3("Goal Position", &mov->m_goalPos.x, 0.01f)) isGimmickChanged = true;

				// -------------------------------------------------------------
				// ウェイポイント（Waypoints）リスト管理処理
				// -------------------------------------------------------------
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::TextUnformatted("--- Waypoints Management ---");

				// 1. ウェイポイント追加ボタン
				if (ImGui::Button("+ Add Waypoint", ImVec2(130, 0)))
				{
					// 初期位置：最後のウェイポイントの位置、または Start Position の少し先
					Math::Vector3 newPoint = mov->m_startPos;
					if (!mov->m_waypoints.empty())
					{
						newPoint = mov->m_waypoints.back();
					}
					newPoint.x += 1.0f; // 少しずらして配置

					mov->m_waypoints.push_back(newPoint);
					isGimmickChanged = true;
				}

				// 2. ウェイポイント一覧の表示・編集
				int removeIndex = -1; // 削除予約用インデックス

				for (int wpIdx = 0; wpIdx < static_cast<int>(mov->m_waypoints.size()); ++wpIdx)
				{
					ImGui::PushID(wpIdx); // ループ内 ID の競合防止

					// ドラッグ入力で各座標を編集
					std::string wpLabel = "WP [" + std::to_string(wpIdx + 1) + "]";
					if (ImGui::DragFloat3(wpLabel.c_str(), &mov->m_waypoints[wpIdx].x, 0.01f))
					{
						isGimmickChanged = true;
					}

					// 削除ボタン
					ImGui::SameLine();
					if (ImGui::Button("X"))
					{
						removeIndex = wpIdx;
					}

					ImGui::PopID();
				}

				// 削除処理（ループ外で安全に実行）
				if (removeIndex != -1)
				{
					mov->m_waypoints.erase(mov->m_waypoints.begin() + removeIndex);
					isGimmickChanged = true;
				}
			}
			else if (auto* finish = std::get_if<FinishAreaParams>(&gimmick.m_param))
			{
				// ここは何もない
			}

			ImGui::Separator();

			// 1. 複製ボタン（Duplicate / Copy）
			if (ImGui::Button("Duplicate Selected Gimmick", ImVec2(200, 0)))
			{
				// 選択中のギミックデータを深層コピー
				auto duplicateGimmick = gimmicks[m_selectedIndex];

				// （任意）重なって見づらくなるのを防ぐため座標を少しだけずらす
				duplicateGimmick.m_data.m_position.x += 0.5f;

				// 配列に追加
				gimmicks.push_back(duplicateGimmick);

				// 選択インデックスを新しく生成された末尾のギミックに変更
				ChangeSelectedObject(category, (gimmicks.size()) - 1);

				// 入力バッファやエラー状態を新ギミック用にリセット
				lastSelectedIndex = m_selectedIndex;
				isGimmickModelError = false;
				if (auto* rot = std::get_if<RotatingParams>(&gimmicks[m_selectedIndex].m_param))
				{
					gimmickModelPathInput = rot->m_modelPath;
				}
				if (auto* mov = std::get_if<MovingParams>(&gimmicks[m_selectedIndex].m_param))
				{
					gimmickModelPathInput = mov->m_modelPath;
				}

				// 変更を取得
				isGimmickChanged = true;
			}

			ImGui::SameLine();

			// 2. 既存の削除ボタン
			if (ImGui::Button("Delete Selected Gimmick"))
			{
				gimmicks.erase(gimmicks.begin() + m_selectedIndex);
				m_selectedIndex = -1; // 選択解除

				// 変更を取得
				isGimmickChanged = true;
			}
		}

		ImGui::TreePop();
	}

	// 変更したかを返す
	return isGimmickChanged;
}

bool StageEditor::UpdatePinsSetting(StageLaneData& frameData)
{
	// カテゴリ定義
	const SelectedTargetCategory category = SelectedTargetCategory::Pin;
	// 変更確認用
	bool isPinChanged = false;

	if (ImGui::TreeNode("Pin List (10 Pins)"))
	{
		auto& pins = frameData.m_lanePinData;

		// 1. ピン一覧リストボックス
		ImGui::BeginChild("PinListChild", ImVec2(0, 120), true);
		for (int i = 0; i < static_cast<int>(pins.size()); ++i)
		{
			std::string label = "[" + std::to_string(i) + "] Pin " + std::to_string(i + 1);

			// カテゴリとインデックスが両方一致している場合のみハイライト
			bool isSelected = (CheckCategoryAndIndex(category, i));

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				// 選択カテゴリとインデックスを更新
				ChangeSelectedObject(category, i);
			}
		}
		ImGui::EndChild();

		// 2. 範囲チェック
		if (CheckCategory(category))
		{
			if (pins.empty())
			{
				// 選択中のオブジェクトをリセット
				ChangeSelectedObject();
			}
			else if (m_selectedIndex >= static_cast<int>(pins.size()))
			{
				ChangeSelectedObject(category, pins.size() - 1);
			}
		}

		// 3. 選択中ピンの編集UI
		if (CheckCategory(category) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(pins.size()))
		{
			ImGui::Separator();
			ImGui::Text("Edit Pin [%d]", m_selectedIndex + 1);

			auto& pin = pins[m_selectedIndex];

			if (ImGui::DragFloat3("Position", &pin.m_data.m_position.x, 0.01f)) isPinChanged = true;
			if (ImGui::DragFloat3("Rotation", &pin.m_data.m_rotation.x, 0.1f))  isPinChanged = true;
			if (ImGui::DragFloat3("Scale", &pin.m_data.m_scale.x, 0.01f)) isPinChanged = true;
		}

		ImGui::TreePop();
	}

	// 変更したかを返す
	return isPinChanged;
}

bool StageEditor::UpdatePlayerSetting(StageLaneData& frameData)
{
	// カテゴリ定義
	const SelectedTargetCategory category = SelectedTargetCategory::Player;
	// 変更検知用
	bool isPlayerChanged = false;

	if (ImGui::TreeNode("Player Transform Settings"))
	{
		auto& player = frameData.m_playerData;

		// 1. サイズ 1 の選択リストボックス（高さ 35px 程度で固定）
		ImGui::BeginChild("PlayerListChild", ImVec2(0, 35), true);

		bool isSelected = (CheckCategory(category));
		if (ImGui::Selectable("[Player] Start Position", isSelected))
		{
			ChangeSelectedObject(category, 0);
		}
		ImGui::EndChild();

		// 2. 選択中プレイヤーの編集UI
		if (CheckCategory(category))
		{
			ImGui::Separator();
			ImGui::Text("Edit Player Transform");

			if (ImGui::DragFloat3("Position", &player.m_position.x, 0.01f)) isPlayerChanged = true;
			if (ImGui::DragFloat3("Rotation", &player.m_rotation.x, 0.1f))  isPlayerChanged = true;
		}

		ImGui::TreePop();
	}

	// 変更したかを通知
	return isPlayerChanged;
}

void StageEditor::MoveSelectedObjectToCursor(StageOverallData& stageData, int currentLaneNo)const
{
	// レーンデータ
	auto& frameData = stageData.m_stageLaneData[currentLaneNo];

	// カメラ取得
	std::weak_ptr<CameraBase> parent = CAMERAMGR.GetCamera(CameraType::Debug);
	std::shared_ptr<PointTargetCamera> camera;
	static bool camCastSuccess = false;
	if (camera = std::dynamic_pointer_cast<PointTargetCamera>(parent.lock()))
	{
		camCastSuccess = true;
	}
	else camCastSuccess = false;

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

		// 選択中の種類と Index から直接データを特定
		const JPH::BodyID& targetBodyID = STAGEMGR.GetBodyID(m_selectedCategory, m_selectedIndex);

		// レイを飛ばす
		bool hasHit = false;
		JPH::RayCastResult hit;
		if (!targetBodyID.IsInvalid())
		{
			// Jolt標準の単一Body除外フィルターをそのまま生成して渡す
			JPH::IgnoreSingleBodyFilter selfFilter(targetBodyID);

			hasHit = PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastRay(rayCast, hit, {}, groundFilter, selfFilter);
		}
		else
		{
			hasHit = PHYSICSMGR.GetSystem().GetNarrowPhaseQuery().CastRay(rayCast, hit, {}, groundFilter);
		}

		if (hasHit)
		{
			// ★レイが何かに当たった！その「衝突点の3D座標」を取得
			JPH::RVec3 hitPos = rayCast.GetPointOnRay(hit.mFraction);
			Math::Vector3 hitVec3Pos = Math::Vector3(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());

			// 選択中の種類と Index から直接データを特定して移動
			if (CheckCategory(SelectedTargetCategory::Decoration))
			{
				if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(stageData.m_decorationData.size()))
				{
					auto& deco = stageData.m_decorationData[m_selectedIndex];
					deco.m_srtData.m_position = Math::Vector3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

					// 島の再生成
					STAGEMGR.BuildIsland(currentLaneNo);
				}
			}
			if (CheckCategory(SelectedTargetCategory::Gimmick))
			{
				if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(frameData.m_laneGimmickData.size()))
				{
					auto& gimmick = frameData.m_laneGimmickData[m_selectedIndex];
					gimmick.m_data.m_position = Math::Vector3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

					// ギミックの再生成
					STAGEMGR.BuildGimmicks(currentLaneNo);
				}
			}
			else if (CheckCategory(SelectedTargetCategory::Pin))
			{
				if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(frameData.m_lanePinData.size()))
				{
					auto& pin = frameData.m_lanePinData[m_selectedIndex];
					pin.m_data.m_position = Math::Vector3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

					// ピンの再生成
					STAGEMGR.BuildPins(currentLaneNo);
				}
			}
			else if (CheckCategory(SelectedTargetCategory::Player))
			{
				auto& player = frameData.m_playerData;
				player.m_position = Math::Vector3(hitVec3Pos.x, hitVec3Pos.y, hitVec3Pos.z);

				// プレイヤーの再生成
				STAGEMGR.BuildPlayer(currentLaneNo);
			}
		}
	}
}

void StageEditor::SetDebugOutlineToSelected(const StageOverallData& stageData, int currentLaneNo) const
{
	// レーンデータ
	const auto& frameData = stageData.m_stageLaneData[currentLaneNo];

	Math::Vector3 debugPos = { 0.0f, 0.0f, 0.0f };
	bool hasSelection = false;

	if (CheckCategory(SelectedTargetCategory::Decoration) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(stageData.m_decorationData.size()))
	{
		debugPos = stageData.m_decorationData[m_selectedIndex].m_srtData.m_position;
		hasSelection = true;
	}
	if (CheckCategory(SelectedTargetCategory::Gimmick) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(frameData.m_laneGimmickData.size()))
	{
		debugPos = frameData.m_laneGimmickData[m_selectedIndex].m_data.m_position;
		hasSelection = true;
	}
	else if (CheckCategory(SelectedTargetCategory::Pin) && m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(frameData.m_lanePinData.size()))
	{
		debugPos = frameData.m_lanePinData[m_selectedIndex].m_data.m_position;
		hasSelection = true;
	}
	else if (CheckCategory(SelectedTargetCategory::Player))
	{
		debugPos = frameData.m_playerData.m_position;
		hasSelection = true;
	}

	// STAGEMGR に選択中の座標を渡して DebugSphere を表示
	STAGEMGR.SetDebugOutlinePos(hasSelection, debugPos);
}

void StageEditor::ChangeSelectedObject(SelectedTargetCategory category, int index)
{
	m_selectedCategory = category;
	m_selectedIndex = index;
}

void StageEditor::Init()
{
	// 各種データのロード
	LoadDecorationPathData();
	LoadIslandPathData();
}

void StageEditor::LoadDecorationPathData()
{
	// パスはクラス内部に直書きで保持
	const std::string masterJsonPath = "Asset/Data/StageEditorData/DecorationsData.json";

	std::ifstream file(masterJsonPath);
	if (!file.is_open())
	{
		return; // ファイルが開けない場合
	}

	nlohmann::json rootJson;
	try
	{
		file >> rootJson;
	}
	catch (...)
	{
		file.close();
		return; // JSONの構文エラー等
	}
	file.close();

	// デコレーション関係の基本パス
	m_baseDecorationPath = Utf8ToMultiByte(rootJson.value("baseDecorationPath", "Error"));
	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson["decorations"])
	{
		DecorationPathData decoPath;

		// 各値を取得
		decoPath.m_name = Utf8ToMultiByte(item.value("name", "Error"));
		decoPath.m_islandType = Utf8ToMultiByte(item.value("decoType", "Error"));
		decoPath.m_path = Utf8ToMultiByte(item.value("path", "Error"));

		// 格納
		m_decoPathData.push_back(decoPath);
	}
}

void StageEditor::LoadIslandPathData()
{
	// パスはクラス内部に直書きで保持
	const std::string masterJsonPath = "Asset/Data/StageEditorData/IslandsData.json";

	std::ifstream file(masterJsonPath);
	if (!file.is_open())
	{
		return; // ファイルが開けない場合
	}

	nlohmann::json rootJson;
	try
	{
		file >> rootJson;
	}
	catch (...)
	{
		file.close();
		return; // JSONの構文エラー等
	}
	file.close();

	// デコレーション関係の基本パス
	m_baseIslandPath = Utf8ToMultiByte(rootJson.value("baseModelPath", "Error"));
	// 配列要素を1つずつ走査して構造体に格納
	for (const auto& item : rootJson["islands"])
	{
		IslandPathData islandPath;

		// 各値を取得
		islandPath.m_name = Utf8ToMultiByte(item.value("name", "Error"));
		islandPath.m_path = Utf8ToMultiByte(item.value("path", "Error"));

		// 格納
		m_islandPathData.push_back(islandPath);
	}
}
