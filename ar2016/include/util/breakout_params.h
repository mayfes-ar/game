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

	const Eigen::Vector2i INFO_HIME_START_POS = Eigen::Vector2i(INFO_WIDTH/4, HEIGHT/2);
	const int INFO_HIME_WIDTH = INFO_WIDTH/2;
	const int INFO_HIME_HEIGHT = INFO_HIME_WIDTH * 1046 / 621;

	const Eigen::Vector2i FIELD_START_POS = Eigen::Vector2i(INFO_WIDTH, 0);
	const int FIELD_WIDTH = 3 * WIDTH / 4;
	const int FIELD_HEIGHT = HEIGHT;
	const Eigen::Vector2i EXPLANATION_START_POS = Eigen::Vector2i(INFO_WIDTH + FIELD_WIDTH, 0);
	const int EXPLANATION_WIDTH = WIDTH / 4;
	const int EXPLANATION_HEIGHT = HEIGHT;

	const Eigen::Vector2i RESULT_START_POS = Eigen::Vector2i::Zero();
	const int RESULT_WIDTH = WIDTH;
	const int RESULT_HEIGHT = HEIGHT;

	// CountDown
	const Eigen::Vector2i COUNT_DOWN_START_POS = Eigen::Vector2i(WIDTH / 3, HEIGHT / 5);
	const int COUNT_DOWN_WIDTH = WIDTH / 4;
	const int COUNT_DOWN_HEIGHT = WIDTH / 4;

	// Timerのパラメータ
	constexpr int TIMER_MAX_MIN = 1;
	constexpr int TIMER_MAX_SEC = 5;
	constexpr int TIMER_MAX_MSEC = 0;

	// Blockのパラメータ
	const int BLOCK_OFFSET_X = WIDTH / 4;
	const int BLOCK_OFFSET_Y = HEIGHT / 7;
	constexpr int BLOCK_WIDTH_NUM = 15;
	constexpr int BLOCK_HEIGHT_NUM = 5;
	const int BLOCK_WIDTH = (FIELD_WIDTH) / BLOCK_WIDTH_NUM;
	const int BLOCK_HEIGHT = (HEIGHT / 4) / BLOCK_HEIGHT_NUM;
	constexpr double BLOCK_GENERATE_PROB = 0.1; // Blockが生成される確率
												// BLOCKの種類
	constexpr double BLOCK_RED_PROB = 0.2;
	constexpr double BLOCK_BLUE_PROB = 0.3;
	constexpr double BLOCK_GREEN_PROB = 0.5;

	constexpr double NORMAL_BLOCK_PROB = 0.5;
	constexpr double HARD_BLOCK_PROB = 0.3;
	constexpr double UNBREAKABLE_BLOCK_PROB = 0.4;

	constexpr int NORMAL_BLOCK_LIFE_NUM = 1;
	constexpr int HARD_BLOCK_LIFE_NUM = 2;
	constexpr int UNBREAKABLE_BLOCK_LIFE_NUM = 100000;

	// Enemyのパラメータ
	const Eigen::Vector2f ENEMY_ACCEL{ 1, 0 };
	const int ENEMY_HEAD_WIDTH = 80;
	const Eigen::Vector2i ENEMY_HEAD_POS = FIELD_START_POS + Eigen::Vector2i{ FIELD_WIDTH / 2 - ENEMY_HEAD_WIDTH / 2 , 0 };

	const int ENEMY_HEAD_LIFE = 2;
	const int SNAKE_ENEMY_LIFE = 1;

	constexpr int MAX_ENEMY_NUM = 1;
	constexpr int SNAKE_WIDTH = 60;
	constexpr int SNAKE_HEIGHT = 60;

	// Fireballのパラメータ
	const Eigen::Vector2i FIREBALL_STARTPOS = ENEMY_HEAD_POS;
	const Eigen::Vector2f FIREBALL_STARTVEL = Eigen::Vector2f{ 5.0f, 10.0f };
	constexpr int FIREBALL_RADIUS = 20;
	constexpr int MAX_FIREBALL_NUM = 1;
	constexpr int MAX_FIREBALL_NUM_ON_NORMAL = 2;
	constexpr int MAX_FIREBALL_NUM_ON_HARD = 2;

	// Shipのパラメータ
	const int SHIP_OFFSET_X = FIELD_START_POS.x() + WIDTH / 4;
	const int SHIP_OFFSET_Y = HEIGHT * 4 / 5;
	const int SHIP_BLOCK_HEIGHT = (HEIGHT / 4) / BLOCK_HEIGHT_NUM;
	const int SHIP_BLOCK_WIDTH = SHIP_BLOCK_HEIGHT;
	const Eigen::Vector2i SHIP_START_POS = Eigen::Vector2i{ SHIP_OFFSET_X, SHIP_OFFSET_Y };
	constexpr int SHIP_LIFE_NUM = 4;

	// Potのパラメータ
	const Eigen::Vector2i POT_START_POS = Eigen::Vector2i{ BLOCK_OFFSET_X + FIELD_WIDTH / 2, HEIGHT - 200 };
	const int POT_EXHARE_DEPTH = 400;
	const int POT_APPEAR_DEPTH = 100;

	// House のパラメータ
	constexpr int HOUSE_NUM = 6;
	constexpr int CASTLE_NUM = 1;
	constexpr int NORMAL_HOUSE_NUM = HOUSE_NUM - CASTLE_NUM;
	constexpr int NORMAL_HOUSE_HEIGHT = 100;
	constexpr int NORMAL_HOUSE_WIDTH = 100;
	constexpr int CASTLE_HOUSE_WIDTH = 120;
	constexpr int CASTLE_HOUSE_HEIGHT = 120;
	const int HOUSE_OFFSET_X = WIDTH / 4;
	const int HOUSE_OFFSET_Y = HEIGHT - NORMAL_HOUSE_HEIGHT;
	const Eigen::Vector2i HOUSE_START_POS = Eigen::Vector2i{ HOUSE_OFFSET_X, HOUSE_OFFSET_Y };
	const int NORMAL_HOUSE_LIFE = 1;
	const int CASTLE_HOUSE_LIFE = 5;

	// Resident のパラメータ
	constexpr int RESIDENT_NUM = 4;
	constexpr int RESIDENT_HEIGHT = 80;
	constexpr int RESIDENT_WIDTH = 80;
	const int RESIDENT_OFFSET_X = WIDTH / 4;
	const int RESIDENT_OFFSET_Y = HEIGHT - RESIDENT_HEIGHT;
	const Eigen::Vector2i RESIDENT_START_POS = Eigen::Vector2i{ RESIDENT_OFFSET_X, RESIDENT_OFFSET_Y };
	const int RESIDENT_LIFE = 1;

	// Score
	constexpr int BLOCK_SCORE = 50;
	constexpr int ENEMY_SCORE = 100;
	constexpr int BOSS_HEAD_SCORE = 1000;
	constexpr int BOSS_LEFT_SCORE = 500;
	constexpr int BOSS_RIGHT_SCORE = 500;
	constexpr int HOUSE_SCORE = -100;
	constexpr int SCORE_OFFSET = 1000;

} // namespace Breakout
