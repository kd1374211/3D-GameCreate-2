#pragma once

// 共通のインライン関数（またはヘッダー専用関数）として定義
inline Math::Vector2 GetFixedCursorPos(POINT cursor)
{
	// 中心に指定
	cursor.x -= 640.0f;
	cursor.y -= 360.0f;

	// Y反転
	cursor.y *= -1.0f;

	return Math::Vector2(cursor.x, cursor.y);
}