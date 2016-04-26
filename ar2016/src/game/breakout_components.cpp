#include "game/breakout_components.h"
#include "util/breakout_params.h"

using namespace Breakout;


void BreakoutComponents::setup()
{
	// Layoutの初期化
	{
		const auto info_realm = Shape::Rectangle(INFO_START_POS, INFO_WIDTH, INFO_HEIGHT);
		info = std::make_shared<Breakout::Info>(info_realm);

		const auto debug_realm = Shape::Rectangle(DEBUG_WINDOW_START_POS, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_HEIGHT);
		debug = std::make_shared<Breakout::Info>(debug_realm);

		const auto field_realm = Shape::Rectangle(FIELD_START_POS, FIELD_WIDTH, FIELD_HEIGHT);
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

		Eigen::Vector2d start_vel = Eigen::Vector2d{0.0, 50.0};
		Eigen::Vector2d start_accel = Eigen::Vector2d::Zero();
		auto moving = std::make_shared<Moving>(0.1, start_accel, start_vel);

		fireball = std::make_shared<Breakout::Fireball>(circle, moving);
	}

	// shipの初期化
	{
		const Life life = Life{SHIP_LIFE_NUM, SHIP_LIFE_NUM, SHIP_LIFE_NUM};
		ship = std::make_shared<Breakout::Ship>(SHIP_START_POS, life);
	}
	// Todo Player
}
