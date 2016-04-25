#pragma once

#include <Eigen/Core>
#include "const.h"

namespace Breakout {
	// Layout (画面を縦に４分割し)
	// 左　1/4 : Score, Timerを表示
	// 真ん中 2~3/4: Game
	// 右  4/4 : 未定
	const Eigen::Vector2i INFO_START_POS = Eigen::Vector2i::Zero();
	const int INFO_WIDTH = WIDTH / 4;
	const int INFO_HEIGHT = HEIGHT;
	const Eigen::Vector2i FIELD_START_POS = Eigen::Vector2i(INFO_WIDTH, 0);
	const int FIELD_WIDTH = 2 * WIDTH / 4;
	const int FIELD_HEIGHT = HEIGHT;
	const Eigen::Vector2i DEBUG_WINDOW_START_POS = Eigen::Vector2i(INFO_WIDTH + FIELD_WIDTH, 0);
	const int DEBUG_WINDOW_WIDTH = 3 * WIDTH / 4 - 50;
	const int DEBUG_WINDOW_HEIGHT = HEIGHT;

	// Blockのパラメータ
	const int BLOCK_OFFSET_X = WIDTH / 4;
	const int BLOCK_OFFSET_Y = HEIGHT / 10;
	constexpr int BLOCK_WIDTH_NUM = 10;
	constexpr int BLOCK_HEIGHT_NUM = 5;
	const int BLOCK_WIDTH = (WIDTH / 2) / BLOCK_WIDTH_NUM;
	const int BLOCK_HEIGHT = (HEIGHT / 4) / BLOCK_HEIGHT_NUM;

	// Fireballのパラメータ
	const Eigen::Vector2i FIREBALL_STRATPOS = Eigen::Vector2i{
		WIDTH / 2, HEIGHT - 200 };
	constexpr int FIREBALL_RADIUS = 30;

	// Shipのパラメータ
	const Eigen::Vector2i SHIP_START_POS = Eigen::Vector2i{ BLOCK_OFFSET_X, HEIGHT - 100 };
	constexpr int SHIP_LIFE_NUM = 4;
} // namespace Breakout
