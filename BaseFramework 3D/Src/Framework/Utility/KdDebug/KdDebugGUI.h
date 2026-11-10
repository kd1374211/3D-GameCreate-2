#pragma once
#include "../../../Application/Const/StageDataConsts.h"

struct ImGuiAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
	bool                AutoScroll;
	bool                ScrollToBottom;

	ImGuiAppLog()
	{
		AutoScroll = true;
		ScrollToBottom = false;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
		if (AutoScroll)
			ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			if (ImGui::Checkbox("Auto-scroll", &AutoScroll))
				if (AutoScroll)
					ScrollToBottom = true;
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (clear)
			Clear();
		if (copy)
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();
		if (Filter.IsActive())
		{
			for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				if (Filter.PassFilter(line_start, line_end))
					ImGui::TextUnformatted(line_start, line_end);
			}
		}
		else
		{
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
		ImGui::End();
	}
};

// 11/09
enum class SelectedTargetCategory
{
	None,
	Decoration,
	Gimmick,
	Pin,
	Player
};

// 追加9/8
class ScoreHandler;
class PointTargetCamera;

class KdDebugGUI
{
public:
	void GuiInit(int w, int h);
	void GuiProcess();

	void AddLog(const char* fmt, ...);
	void ClearLog();

	// 11-09
	// 各処理分け
	void ToggleStageEditor(int& currentStageNo, int& currentLaneNo);				// エディター開始・終了
	void UpdateCamera(std::shared_ptr<PointTargetCamera> camera);
	void ToggleGimmickStop();
	void UpdateStageEditor(int& currentStageNo, int& currentLaneNo, std::shared_ptr<PointTargetCamera> camera);						// 1. エディター本体
	void UpdateFrameSelection(int& currentStageNo, int& currentLaneNo, StageOverallData& stageData, std::shared_ptr<PointTargetCamera> camera);					// 1-1. 編集ステージ切り替え
	void UpdateSaveAndLoad(int currentStageNo, int currentLaneNo, StageOverallData& stageData);						// 1-2. セーブロード・コピー等
	void UpdateStageEdit(int currentStageNo, int currentLaneNo, StageOverallData& stageData);							// 2. ステージ内の編集
	void UpdateSkyAssetSettings(std::string& skyAssetPathStr, bool& isError, StageOverallData& stageData);					// 2-1. 天球アセット
	void UpdateIslandAndDecorations(StageOverallData& stageData);				// 2-2. 島アセットとデコレーション関連
	void UpdateFrameObjectsSetting(int currentLaneNo, std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageOverallData& stageData);				// 3. フレーム内オブジェクト編集
	bool UpdateLaneAssetPath(std::string& terrainPath_Stage, std::string& terrainPath_Frame, bool& isError, StageLaneData& frameData);						// 3-1. レーンのアセット
	bool UpdateIslandDecorationsVisibility(int currentLaneNo, StageOverallData& stageData);		// 3-2. 島デコレーションの表示切替
	bool UpdateGimmickSetting(StageLaneData& frameData);					// 3-3. ギミック編集
	bool UpdatePinsSetting(StageLaneData& frameData);						// 3-4. ピン配置
	bool UpdatePlayerSetting(StageLaneData& frameData);						// 3-5. プレイヤー配置
	void MoveSelectedObjectToCursor(StageOverallData& stageData, int currentLaneNo)const;				// 4-1. 選択中のオブジェクトをカーソル位置に移動
	void SetDebugOutlineToSelected(const StageOverallData& stageData, int currentLaneNo)const;				// 4-2. デバッグアウトラインを選択中のオブジェクトに表示

	// カテゴリ・インデックスの変更
	void ChangeSelectedObject(SelectedTargetCategory category = SelectedTargetCategory::None, int index = -1);
	// カテゴリ・インデックスが合っているかの確認
	bool CheckCategory(SelectedTargetCategory category)const { return m_selectedCategory == category; }
	bool CheckIndex(int index)const { return m_selectedIndex == index; }
	bool CheckCategoryAndIndex(SelectedTargetCategory category, int index)const { return CheckCategory(category) && CheckIndex(index); }

	// 追加9/8
	// 仮スコア表示用
	void RegistScoreHandler(std::shared_ptr<ScoreHandler> handler) { m_wpScoreHandler = handler; }

private:
	void GuiRelease();

	// ImGui
	std::unique_ptr<ImGuiAppLog> m_uqLog = nullptr;

	// 追加9/8
	// 仮スコア表示用
	void DrawDebugScoreGUI();
	std::weak_ptr<ScoreHandler> m_wpScoreHandler;

	// 現在選択中のカテゴリ・インデックス
	SelectedTargetCategory m_selectedCategory = SelectedTargetCategory::None;
	int m_selectedIndex = -1;

//=====================================================
// シングルトンパターン
//=====================================================
private:
	KdDebugGUI();
	~KdDebugGUI();

public:
	static KdDebugGUI& Instance() {
		static KdDebugGUI Instance;
		return Instance;
	}
};
