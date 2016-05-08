#pragma once

#include <Eigen/Core>
#include "const.h"

namespace Breakout {
	// Select画面
	const Eigen::Vector2i SELECT_START_POS = Eigen::Vector2i(WIDTH / 3, 0);
	const int SELECT_WIDTH = WIDTH / 3;
	const int SELECT_HEIGHT = HEIGHT * 6 / 7;
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
		5.0f, 10.0f};
	constexpr int FIREBALL_RADIUS = 20;
	constexpr int MAX_FIREBALL_NUM = 2;

	// Shipのパラメータ
	const int SHIP_OFFSET_X = WIDTH / 4;
	const int SHIP_OFFSET_Y = HEIGHT - HEIGHT / 6;
	const Eigen::Vector2i SHIP_START_POS = Eigen::Vector2i{ SHIP_OFFSET_X, SHIP_OFFSET_Y };
	constexpr int SHIP_LIFE_NUM = 4;

	// Potのパラメータ
	const Eigen::Vector2i POT_START_POS = Eigen::Vector2i{ BLOCK_OFFSET_X + FIELD_WIDTH/2, HEIGHT - 200 };

	// Enemyのパラメータ
	const Eigen::Vector2f ENEMY_ACCEL{ 1, 0 };
	const int ENEMY_HEAD_WIDTH = 80;
	const Eigen::Vector2i ENEMY_HEAD_POS = FIELD_START_POS + Eigen::Vector2i{ FIELD_WIDTH / 2 - ENEMY_HEAD_WIDTH / 2 , 0 };

	const int ENEMY_HEAD_LIFE = 2;

	// House のパラメータ
	constexpr int HOUSE_NUM = 4;
	constexpr int HOUSE_HEIGHT = 100;
	constexpr int HOUSE_WIDTH = 100;
	const int HOUSE_OFFSET_X = WIDTH / 4;
	const int HOUSE_OFFSET_Y = HEIGHT - HOUSE_HEIGHT;
	const Eigen::Vector2i HOUSE_START_POS = FIELD_START_POS + Eigen::Vector2i{ HOUSE_OFFSET_X, HOUSE_OFFSET_Y };
	const int HOUSE_LIFE = 1;

	// Resident のパラメータ
	constexpr int RESIDENT_NUM = 4;
	constexpr int RESIDENT_HEIGHT = 80;
	constexpr int RESIDENT_WIDTH = 80;
	const int RESIDENT_OFFSET_X = WIDTH / 4;
	const int RESIDENT_OFFSET_Y = HEIGHT - RESIDENT_HEIGHT;
	const Eigen::Vector2i RESIDENT_START_POS = FIELD_START_POS + Eigen::Vector2i{ RESIDENT_OFFSET_X, RESIDENT_OFFSET_Y };
	const int RESIDENT_LIFE = 1;

} // namespace Breakout
