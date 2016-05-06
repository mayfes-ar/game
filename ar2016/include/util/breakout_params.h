﻿#pragma once

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
	const Eigen::Vector2i EXPLANATION_START_POS = Eigen::Vector2i(INFO_WIDTH + FIELD_WIDTH, 0);
	const int EXPLANATION_WIDTH = WIDTH / 4;
	const int EXPLANATION_HEIGHT = HEIGHT;
	
	const Eigen::Vector2i RESULT_START_POS = Eigen::Vector2i::Zero();
	const int RESULT_WIDTH = WIDTH;
	const int RESULT_HEIGHT = HEIGHT;

	// Timerのパラメータ
	constexpr int TIMER_MAX_MIN = 1;
	constexpr int TIMER_MAX_SEC = 5;
	constexpr int TIMER_MAX_MSEC = 0;

	// Blockのパラメータ
	const int BLOCK_OFFSET_X = WIDTH / 4;
	const int BLOCK_OFFSET_Y = HEIGHT / 8;
	constexpr int BLOCK_WIDTH_NUM = 10;
	constexpr int BLOCK_HEIGHT_NUM = 5;
	const int BLOCK_WIDTH = (WIDTH / 2) / BLOCK_WIDTH_NUM;
	const int BLOCK_HEIGHT = (HEIGHT / 4) / BLOCK_HEIGHT_NUM;
	constexpr double BLOCK_GENERATE_PROB = 0.1; // Blockが生成される確率
	// BLOCKの種類
	constexpr double BLOCK_RED_PROB = 0.2;
	constexpr double BLOCK_BLUE_PROB = 0.3;
	constexpr double BLOCK_GREEN_PROB = 0.5;

	constexpr double NORMAL_BLOCK_PROB = 0.5;
	constexpr double HARD_BLOCK_PROB = 0.3;
	constexpr double UNBREAKABLE_BLOCK_PROB = 0.2;

	constexpr int NORMAL_BLOCK_LIFE_NUM = 1;
	constexpr int HARD_BLOCK_LIFE_NUM = 2;
	constexpr int UNBREAKABLE_BLOCK_LIFE_NUM = 100000;

	// Fireballのパラメータ
	const Eigen::Vector2i FIREBALL_STARTPOS = Eigen::Vector2i{
		WIDTH / 2, HEIGHT / 2 };
	const Eigen::Vector2f FIREBALL_STARTVEL = Eigen::Vector2f{
		2.0f, 4.0f};
	constexpr int FIREBALL_RADIUS = 20;
	constexpr int MAX_FIREBALL_NUM = 2;

	// Shipのパラメータ
	const Eigen::Vector2i SHIP_START_POS = Eigen::Vector2i{ BLOCK_OFFSET_X, HEIGHT - 100 };
	constexpr int SHIP_LIFE_NUM = 4;

	// Potのパラメータ
	const Eigen::Vector2i POT_START_POS = Eigen::Vector2i{ BLOCK_OFFSET_X , HEIGHT - 200 };

	// Enemyのパラメータ
	const Eigen::Vector2f ENEMY_ACCEL{ 1, 0 };
	const int ENEMY_HEAD_WIDTH = 80;
	const Eigen::Vector2i ENEMY_HEAD_POS = FIELD_START_POS + Eigen::Vector2i{ FIELD_WIDTH / 2 - ENEMY_HEAD_WIDTH / 2 , 0 };

	const int ENEMY_HEAD_LIFE = 2;
} // namespace Breakout
