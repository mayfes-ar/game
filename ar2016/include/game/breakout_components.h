

#pragma once

#include <list>
#include <memory>
#include <vector>
#include "object/breakout_object.h"
#include "const.h"
#include <Eigen/Core>

// Layout (画面を縦に４分割し)
// 左　1/4 : Score, Timerを表示
// 真ん中 2~3/4: Game
// 右  4/4 : 未定
//const Eigen::Vector2i INFO_START_POS = Eigen::Vector2i::Zero();
//const int INFO_WIDTH = WIDTH / 4;
//const int INFO_HEIGHT = HEIGHT;
//const Eigen::Vector2i FIELD_START_POS = Eigen::Vector2i(INFO_WIDTH, 0);
//const int FIELD_WIDTH = 2 * WIDTH / 4;
//const int FIELD_HEIGHT = HEIGHT;
//const Eigen::Vector2i DEBUG_WINDOW_START_POS = Eigen::Vector2i(INFO_WIDTH + FIELD_WIDTH, 0);
//const int DEBUG_WINDOW_WIDTH = 3 * WIDTH / 4 - 50;
//const int DEBUG_WINDOW_HEIGHT = HEIGHT;
//
//// Blockのパラメータ
//const int BLOCK_OFFSET_X = WIDTH / 4;
//const int BLOCK_OFFSET_Y = HEIGHT / 10;
//constexpr int BLOCK_WIDTH_NUM = 10;
//constexpr int BLOCK_HEIGHT_NUM = 5;
//const int BLOCK_WIDTH= (WIDTH / 2) / BLOCK_WIDTH_NUM;
//const int BLOCK_HEIGHT = (HEIGHT / 4) / BLOCK_HEIGHT_NUM;

// Fireballのパラメータ
const Eigen::Vector2i FIREBALL_STRATPOS = Eigen::Vector2i{
    WIDTH /2, HEIGHT - 200};
constexpr int FIREBALL_RADIUS = 30;

// Shipのパラメータ
const Eigen::Vector2i SHIP_START_POS = Eigen::Vector2i{BLOCK_OFFSET_X, HEIGHT - 100};
constexpr int SHIP_WIDTH = 200;
constexpr int SHIP_HEIGHT = 50;

class BreakoutComponents
{
public:
    BreakoutComponents() {}

	void setup()
	{
		// Layoutの初期化
		{
			const auto info_realm = Shape::Rectangle(INFO_START_POS, INFO_WIDTH, INFO_HEIGHT);
			info = std::make_shared<Breakout::Info>(info_realm);

			const auto debug_realm = Shape::Rectangle(DEBUG_WINDOW_START_POS, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_HEIGHT);
			debug = std::make_shared<Breakout::Info>(debug_realm);

			const auto field_realm = Shape::Rectangle(FIELD_START_POS,FIELD_WIDTH, FIELD_HEIGHT);
			field = std::make_shared<Breakout::Field>(field_realm);
		}

		// Blockの初期化
		for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
			for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
				const Eigen::Vector2i realm 
					= Eigen::Vector2i{BLOCK_OFFSET_X + x * BLOCK_WIDTH, BLOCK_OFFSET_Y + y * BLOCK_HEIGHT };
			
				const Shape::Rectangle rec(realm,BLOCK_WIDTH, BLOCK_HEIGHT);

				auto block = std::make_shared<Breakout::Block>(rec);

				block_list.push_back(block);
			}
		}

		// fireballの初期化
		{
			const auto circle
				= Shape::Circle(FIREBALL_STRATPOS, FIREBALL_RADIUS);

			Eigen::Vector2d start_pos = Eigen::Vector2d(static_cast<double>(WIDTH) / 2.0, static_cast<double>(HEIGHT) - 200.0);
			Eigen::Vector2d start_vel = Eigen::Vector2d::UnitY();
			Eigen::Vector2d start_accel = Eigen::Vector2d::Zero();
			Moving moving = Moving(0.1, start_accel, start_vel, start_pos);

			fireball = std::make_shared<Breakout::Fireball>(circle, moving);
		}

		// shipの初期化
		{
			const Life ship_life = Life(5, 7);

			ship = std::make_shared<Breakout::Ship>(SHIP_START_POS, ship_life);
		}
		// Todo Player
    }

    std::shared_ptr<Breakout::Info> info = nullptr;
    std::shared_ptr<Breakout::Info> debug = nullptr;
    std::shared_ptr<Breakout::Field> field = nullptr;

	std::vector<std::shared_ptr<Breakout::Block>> block_list;

    std::shared_ptr<Breakout::Fireball> fireball = nullptr;

    std::shared_ptr<Breakout::Ship> ship = nullptr;
};
