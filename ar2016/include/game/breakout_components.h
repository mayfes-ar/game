

#pragma once

#include <list>
#include <memory>
#include <vector>
#include "object/breakout_object.h"
#include "const.h"
#include <Eigen/Core>

// Blockのパラメータ
const int BLOCK_OFFSET_X = WIDTH / 3;
const int BLOCK_OFFSET_Y = 10;
constexpr int BLOCK_WIDTH_NUM = 10;
constexpr int BLOCK_HEIGHT_NUM = 5;
constexpr int BLOCK_WIDTH= 50;
constexpr int BLOCK_HEIGHT = 50;

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
		for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
			for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
				const Eigen::Vector2i realm 
					= Eigen::Vector2i{BLOCK_OFFSET_X + x * BLOCK_WIDTH, BLOCK_OFFSET_Y + y * BLOCK_HEIGHT };
			
				const Shape::Rectangle rec(realm,BLOCK_WIDTH, BLOCK_HEIGHT);

				auto block = std::make_shared<Breakout::Block>(rec);

				block_list.push_back(block);
			}
		}

        const auto circle 
            = Shape::Circle( FIREBALL_STRATPOS, FIREBALL_RADIUS );
        fireball = std::make_shared<Breakout::Fireball>(circle);

		
        const Shape::Rectangle ship_realm
			= Shape::Rectangle(SHIP_START_POS, SHIP_WIDTH, SHIP_HEIGHT);

        ship = std::make_shared<Breakout::Ship>(ship_realm);
		// Todo Player
    }

    std::vector<std::shared_ptr<Breakout::Block>> block_list;

    std::shared_ptr<Breakout::Fireball> fireball;

    std::shared_ptr<Breakout::Ship> ship;
};
