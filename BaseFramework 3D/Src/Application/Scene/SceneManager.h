#pragma once

class BaseScene;

class SceneManager
{
public :

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
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

	//追加7/9
	//シーン生成後に追加するオブジェクトを待機
	void AddWaitingObject(const std::shared_ptr<KdGameObject>& _obj) { m_waitingObjList.push_back(_obj); }

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行はしないでおく
	void Init()
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

private :

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Game;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	//追加7/9
	//Init後に追加するオブジェクトを待機させておくリスト
	std::list<std::shared_ptr<KdGameObject>> m_waitingObjList;

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