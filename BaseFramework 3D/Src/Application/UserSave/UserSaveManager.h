#pragma once
#include "../Const/BowlingSystemConst.h"

struct StageSaveData
{
	int m_stageNo = 0;				// ステージ番号
	int m_bestScore = 0;				// 最高得点
};

//リザルト
struct GameResult
{
	float m_stageTimer;	//残り時間
	bool m_isCleared;	//クリアフラグ
	int m_fallenPinCnt; //倒したピン数
	int m_totalPinCnt; //全体のピン数
};

class UserSaveManager
{
public:

	// 初期化（データロード）
	void Init();

	// セーブデータ更新
	bool UpdateSave(int score);

	// ステージ番号からセーブ情報を取得（存在しない場合は nullptr）
	const StageSaveData* GetUserSave(int stageNo) const;
	const StageSaveData* GetUserSave() const;
	// セーブデータ更新用（現在のステージしか呼ばないので引数無し）
	StageSaveData* WorkUserSave();

private:

	// セーブデータ読み込み
	bool LoadUserSaveData();

	// セーブデータ保存
	bool SaveUserSaveData();

	UserSaveManager() {}
	~UserSaveManager() {}

	// stageNo をキーにしたセーブデータ保持用マップ
	std::unordered_map<int, StageSaveData> m_stageSave;

public:

	static UserSaveManager& Instance()
	{
		static UserSaveManager instance;
		return instance;
	}

};

#define SAVEMGR UserSaveManager::Instance()