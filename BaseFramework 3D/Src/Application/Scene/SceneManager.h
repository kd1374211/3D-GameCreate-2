#pragma once

class BaseScene;
class CameraBase;

class SceneManager
{
public :

	// シーン情報
	enum class SceneType
	{
		Title,
		StageSelect,
		Game,
		Result
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行はしないでおく
	void Init()
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

	//終了
	void Release();

	//追加8/11
	//現在のシーン取得
	SceneType GetCurrentSceneType()const { return m_currentSceneType; }

	//追加8/20
	//ゲームスピード関連
	void SetGameSpeed(float speed) { m_gameSpeedMulti = speed; }
	float GetGameSpeed()const { return m_gameSpeedMulti; }
	float GetDeltaGameTime()const;

	//ステージ番号セッター・ゲッター
	void SetStageNo(int No) { m_stageNo = No; }
	int GetStageNo()const { return m_stageNo; }

private :

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Title;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	//追加8/11
	//ステージ番号
	int m_stageNo = 1;

	//追加8/20
	//ゲーム速度
	float m_gameSpeedMulti = 1.0f;

private:

	SceneManager() {}
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};

#define SCENEMGR SceneManager::Instance()