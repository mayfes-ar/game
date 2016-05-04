﻿#include "game/breakout_components.h"
#include "util/breakout_params.h"
#include "moving/newton_behavior.h"
#include "moving/spring_behavior.h"
#include <random>

using namespace Breakout;


void BreakoutComponents::setup()
{
	// Select画面
	// Layoutの初期化
	std::unordered_map<std::string, int> mode_image_store;
	mode_image_store["easy"] = 1;
	mode_image_store["hard"] = 1;
	{
		background = std::make_shared<Background>();
		background->init();
		const auto info_realm = Shape::Rectangle(INFO_START_POS, INFO_WIDTH, INFO_HEIGHT);
		std::shared_ptr<Timer> timer = std::make_shared<Timer>(TIMER_MAX_MIN, TIMER_MAX_SEC, TIMER_MAX_MSEC);
		info = std::make_shared<Breakout::Info>(info_realm, timer);

		const auto result_realm = Shape::Rectangle(RESULT_START_POS, RESULT_WIDTH, RESULT_HEIGHT);
		result = std::make_shared<Breakout::Result>(result_realm);

		const auto exp_realm = Shape::Rectangle(EXPLANATION_START_POS, EXPLANATION_WIDTH, EXPLANATION_HEIGHT);
		explanation = std::make_shared<Breakout::Explanation>(exp_realm);

		const auto field_realm = Shape::Rectangle(FIELD_START_POS, FIELD_WIDTH, FIELD_HEIGHT);
		field = std::make_shared<Breakout::Field>(field_realm);
	}

	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_real_distribution<> block_generator(0.0, 1.0);
	std::uniform_real_distribution<> block_color_generator(0.0, 1.0);
	// Blockの初期化
	for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
		for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
			const Eigen::Vector2i realm 
				= Eigen::Vector2i{BLOCK_OFFSET_X + x * BLOCK_WIDTH, BLOCK_OFFSET_Y + y * BLOCK_HEIGHT };
		
			const Shape::Rectangle rec(realm, BLOCK_WIDTH, BLOCK_HEIGHT);

			auto color = Breakout::Block::Color::Green;
			const auto ratio = block_color_generator(mt);

			if (ratio <= Breakout::BLOCK_RED_PROB) {
				color = Breakout::Block::Color::Red;
			}
			else if (ratio - Breakout::BLOCK_RED_PROB < Breakout::BLOCK_BLUE_PROB) {
				color = Breakout::Block::Color::Blue;
			}

			auto block = std::make_shared<Breakout::Block>(rec, color);

			// 無駄な生成をしている(どうしよう)
			if (block_generator(mt) > BLOCK_GENERATE_PROB) {
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
		// time_step * frq = 1e-3 が最適
		auto moving = std::make_shared<Moving>(1.0f, std::make_shared<StringBehavior>(Eigen::Vector2f{200.0f, 200.0f}, 1e-3f)/*std::make_shared<NewtonBehavior>()*/, start_vel, start_accel);

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
		const auto color = block->getColor();
		using Color = Breakout::Block::Color;

		switch (color) {
			case Color::Green:
				break;
			case Color::Blue: {
				auto item = std::make_shared<Item>(Breakout::ItemKind::RestoreShip);
				block->attachItem(item);
				item_list.push_back(item);
				break;
			}
			case Color::Red: {
				auto item = std::make_shared<Item>(Breakout::ItemKind::DamageShip);
				block->attachItem(item);
				item_list.push_back(item);
				break;
			}
			default:
				break;
		}

	}

	// Todo Player
}
