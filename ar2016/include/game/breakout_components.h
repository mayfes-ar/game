/*
 * BreakoutGameクラスが持つフィールド上のオブジェクト一覧
 */

#pragma once

#include <list>
#include <memory>
#include <vector>
#include "object/breakout_object.h"
#include "const.h"

// Blockのパラメータ
constexpr int BLOCK_WIDTH_NUM = 10;
constexpr int BLOCK_HEIGHT_NUM = 5;
constexpr int BLOCK_WIDTH= 50;
constexpr int BLOCK_HEIGHT = 50;

// Fireballのパラメータ
const Eigen::Vector2i FIREBALL_STRATPOS = Eigen::Vector2i{
    HEIGHT - 500, WIDTH / 2 - 100};
constexpr int FIREBALL_RADIUS = 50;

// Shipのパラメータ
constexpr int SHIP_START_X = 200;
constexpr int SHIP_START_Y = 600;
constexpr int SHIP_WIDTH = 100;
constexpr int SHIP_HEIGHT = 50;

class BreakoutComponents
{
public:
    BreakoutComponents() {}

	void setup()
	{
		for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
			for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
				const Rectan rec(x * BLOCK_WIDTH, y * BLOCK_HEIGHT,
					BLOCK_WIDTH, BLOCK_HEIGHT, 0, 0);

				auto block = std::make_shared<Breakout::Block>(rec);

				block_list.push_back(block);
			}
		}

        const auto circle 
            = Breakout::Circle( FIREBALL_STRATPOS, FIREBALL_RADIUS );
        fireball = std::make_shared<Breakout::Fireball>(circle);

        const Rectan ship_realm
            = Rectan(SHIP_START_X, SHIP_START_Y, SHIP_WIDTH, SHIP_HEIGHT, 
               0.0, 0);

        ship = std::make_shared<Breakout::Ship>(ship_realm);
		// Todo Player
    }

    std::vector<std::shared_ptr<Breakout::Block>> block_list;

    std::shared_ptr<Breakout::Fireball> fireball;

    std::shared_ptr<Breakout::Ship> ship;
};
