#include "game/breakout_components.h"
#include "util/breakout_params.h"
#include <random>

using namespace Breakout;


void BreakoutComponents::setup()
{
	// Layoutの初期化
	{
		auto info_realm = Shape::Rectangle(INFO_START_POS, INFO_WIDTH, INFO_HEIGHT);
		std::shared_ptr<Timer> timer = std::make_shared<Timer>(TIMER_MAX_MIN, TIMER_MAX_SEC, TIMER_MAX_MSEC);
		info = std::make_shared<Breakout::Info>(info_realm, timer);

		result = std::make_shared<Breakout::Result>(RESULT_START_POINT);


		const auto field_realm = Shape::Rectangle(FIELD_START_POS, FIELD_WIDTH, FIELD_HEIGHT);
		field = std::make_shared<Breakout::Field>(field_realm);
	}

	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_real_distribution<> block_generator(0.0, 1.0);
	std::uniform_real_distribution<> item_generator(0.0, 1.0);
	// Blockの初期化
	for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
		for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
			const Eigen::Vector2i realm 
				= Eigen::Vector2i{BLOCK_OFFSET_X + x * BLOCK_WIDTH, BLOCK_OFFSET_Y + y * BLOCK_HEIGHT };
		
			const Shape::Rectangle rec(realm,BLOCK_WIDTH, BLOCK_HEIGHT);

			auto block = std::make_shared<Breakout::Block>(rec);

			if (block_generator(mt) > BLOCK_GENERATE_RATIO) {
				block->disappear();
			}

			block_list.push_back(block);
		}
	}

	// fireballの初期化
	{
		const auto circle
			= Shape::Circle(FIREBALL_STARTPOS, FIREBALL_RADIUS);

		const Eigen::Vector2f start_vel = FIREBALL_STARTVEL;
		const Eigen::Vector2f start_accel = Eigen::Vector2f::Zero();
		auto moving = std::make_shared<Moving>(1.0, start_accel, start_vel);

		fireball = std::make_shared<Breakout::Fireball>(circle, moving);
	}

	// shipの初期化
	{
		const Life life = Life{SHIP_LIFE_NUM, SHIP_LIFE_NUM, SHIP_LIFE_NUM};
		ship = std::make_shared<Breakout::Ship>(SHIP_START_POS, life);
	}

	// potの初期化
	{
		const Shape::Rectangle realm(POT_START_POS, FIREBALL_RADIUS * 2, FIREBALL_RADIUS * 2);
		pot = std::make_shared<Breakout::Pot>(realm);
	}

	// itemの初期化
	for (auto &block : block_list) {
		if (item_generator(mt) > 0.8) {
			auto item = std::make_shared<Item>(Breakout::ItemKind::RestoreShip);
			block->attachItem(item);
			item_list.push_back(item);
		}
		else if(item_generator(mt) > 0.6 ) {
			auto item = std::make_shared<Item>(Breakout::ItemKind::DamageShip);
			block->attachItem(item);
			item_list.push_back(item);
		}
	}

	// Todo Player
}
