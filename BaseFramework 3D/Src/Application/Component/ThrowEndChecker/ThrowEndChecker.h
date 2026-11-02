#pragma once

class PinHandler;
class CharaHandler;

class ThrowEndChecker
{
public:

	ThrowEndChecker() = default;
	~ThrowEndChecker() = default;

	// 投球終了の確認
	bool CheckThrowEnd();

	// 各ハンドラー登録
	void RegistPinHandler(std::shared_ptr<PinHandler> wpPinHandler) { m_wpPinHandler = wpPinHandler; }
	void RegistCharaHandler(std::shared_ptr<CharaHandler> wpCharaHandler) { m_wpCharaHandler = wpCharaHandler; }

private:

	struct ThrowEndCheckerConsts
	{
		static constexpr float ThrowEndCheckTime = 1.0f; // 投球終了条件を連続で満たす必要がある時間（秒）
	};

	// ピンハンドラー
	std::weak_ptr<PinHandler> m_wpPinHandler;

	// キャラハンドラー
	std::weak_ptr<CharaHandler> m_wpCharaHandler;

	// 終了条件を連続で満たしている時間
	float  m_throwEndCheckTimer = 0.0f;
};