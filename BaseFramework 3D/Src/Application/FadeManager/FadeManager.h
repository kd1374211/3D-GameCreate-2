#pragma once

enum FadeState
{
	None,
	FadeIn,
	FadeOut
};

// FadeManager内の仮定義
struct FadeManagerConsts
{
	// alphaChange
	static constexpr float AlphaChange = 2.5f;
};

class FadeManager
{
public:

	void Init();
	void Update();
	void DrawFade();	//最後に呼ぶ

	//フェードイン・アウト
	void StartFadeIn(bool* flg);
	void StartFadeOut(bool* flg);

	//外部からの確認用
	bool IsFadeInEnd()const
	{
		return (m_fadeJustEnded && m_alpha <= 0.0f);
	}
	bool IsFadeOutEnd()const
	{
		return (m_fadeJustEnded && m_alpha >= 1.0f);
	}

	//ゲッター
	bool GetIsFade()const { return m_isFade; }

private:

	FadeManager() {}
	~FadeManager() {}

	//ステート
	FadeState m_fadeState = FadeState::None;

	//フラグ
	bool m_isFade = false;

	//透明度
	float m_alpha = 0.0f;

	//このフレームで終了したか
	bool m_fadeJustEnded = false;

	// 終了時に遠隔でオンにするフラグ（召喚時に設定）
	bool* m_remoteActiveFlg = nullptr;

public:

	static FadeManager& Instance()
	{
		static FadeManager instance;
		return instance;
	}

};

#define FADEMGR FadeManager::Instance()
