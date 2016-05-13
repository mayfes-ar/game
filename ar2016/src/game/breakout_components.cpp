#include "game/breakout_components.h"
#include <random>

using namespace Breakout;


void BreakoutComponents::setup(ShareData& share)
{
	// Select画面
	{
		std::unordered_map<Breakout::Mode, std::string, Breakout::ModeEnumHash> mode_image_store;
		mode_image_store[Breakout::Mode::Easy] = "b_easy";
		mode_image_store[Breakout::Mode::Hard] = "b_hard";
		std::list<Breakout::Mode> mode_list;
		mode_list.push_back(Breakout::Mode::Easy);
		mode_list.push_back(Breakout::Mode::Hard);
		const Shape::Rectangle select_realm = Shape::Rectangle(SELECT_START_POS, SELECT_WIDTH, SELECT_HEIGHT);
		select = std::make_shared<Breakout::Select<Breakout::Mode, Breakout::ModeEnumHash>>(mode_list, mode_image_store, select_realm);
	}
	// Layoutの初期化
	{
		background = std::make_shared<Background>(share.handle);
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
	std::uniform_real_distribution<> block_kind_generator(0.0, 1.0);
	// Blockの初期化
	for (int x = 0; x < BLOCK_WIDTH_NUM; ++x) {
		for (int y = 0; y < BLOCK_HEIGHT_NUM; ++y) {
			const Eigen::Vector2i realm
				= Eigen::Vector2i{ BLOCK_OFFSET_X + x * BLOCK_WIDTH, BLOCK_OFFSET_Y + y * BLOCK_HEIGHT };

			const Shape::Rectangle rec(realm, BLOCK_WIDTH, BLOCK_HEIGHT);

			auto color = Breakout::Block::Color::Green;
			const auto ratio = block_color_generator(mt);

			if (ratio <= Breakout::BLOCK_RED_PROB) {
				color = Breakout::Block::Color::Red;
			}
			else if (ratio - Breakout::BLOCK_RED_PROB < Breakout::BLOCK_BLUE_PROB) {
				color = Breakout::Block::Color::Blue;
			}

			const auto kind_ratio = block_kind_generator(mt);
			std::shared_ptr<Breakout::Block> block = nullptr;

			if (kind_ratio <= Breakout::NORMAL_BLOCK_PROB) {
				block = std::make_shared<Breakout::NormalBlock>(rec, color);
			}
			else if (kind_ratio - Breakout::NORMAL_BLOCK_PROB < Breakout::HARD_BLOCK_PROB) {
				block = std::make_shared<Breakout::HardBlock>(rec, color);
			}
			else {
				block = std::make_shared<Breakout::UnbreakableBlock>(rec, color);
			}

			// 無駄な生成をしている(どうしよう)
			if (block_generator(mt) > BLOCK_GENERATE_PROB) {
				block->disappear();
			}

			block_list.push_back(block);
		}
	}

	// fireballの初期化
	{
		fireball_manager = std::make_shared<Breakout::FireballManager>(MAX_FIREBALL_NUM);
		/*std::uniform_real_distribution<> velocity_generator(FIREBALL_STARTVEL.x(), FIREBALL_STARTVEL.y());
		std::uniform_int<> position_generator(-10, 10);
		std::uniform_real_distribution<> fireball_mode_generator(0.0, 1.0);

		for (int i = 0; i < MAX_FIREBALL_NUM; i++) {
			const auto circle
				= Shape::Circle(FIREBALL_STARTPOS + Eigen::Vector2i(position_generator(mt), position_generator(mt)), FIREBALL_RADIUS);

			const Eigen::Vector2f start_vel = Eigen::Vector2f(velocity_generator(mt), velocity_generator(mt));
			const Eigen::Vector2f start_accel = Eigen::Vector2f::Zero();
			auto moving = std::make_shared<Moving>(1.0, std::make_shared<NewtonBehavior>(), start_vel * -1, start_accel);

			if (fireball_mode_generator(mt) > 0.7) {
				fireball_manager->add(std::make_shared<Breakout::Fireball>(circle, moving, EnemyStrong));
			}
			else {
				fireball_manager->add(std::make_shared<Breakout::Fireball>(circle, moving, EnemyWeak));
			}
		}*/
	}


	// potの初期化
	{
		const Shape::Rectangle realm(POT_START_POS, FIREBALL_RADIUS * 2, FIREBALL_RADIUS * 2);
		pot = std::make_shared<Breakout::Pot>(realm);
	}

	// shipの初期化
	{
		const Life life = Life{ SHIP_LIFE_NUM, SHIP_LIFE_NUM, SHIP_LIFE_NUM };
		ship = std::make_shared<Breakout::Ship>(SHIP_START_POS, life, pot, info);
	}

	// itemの初期化
	{
		std::uniform_real_distribution<> item_kind_generator(0.0, 1.0);

		for (auto &block : block_list) {
			const auto color = block->getColor();
			using Color = Breakout::Block::Color;

			switch (color) {
			case Color::Green: {
				std::shared_ptr<Breakout::Item> item;
				auto ratio = item_kind_generator(mt);
				if (ratio > 0.7) {
					item = std::make_shared<Item>(Breakout::ItemKind::RestoreTime);
				}
				else if (ratio > 0.4) {
					item = std::make_shared<Item>(Breakout::ItemKind::RestorePot);
				} else {
					item = std::make_shared<Item>(Breakout::ItemKind::EnhanceShip);
				}
				block->attachItem(item);
				item_list.push_back(item);
				break;
			}
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
	}

	// Enemyの初期化
	{
		std::shared_ptr<MovingBehavior> behavior_head = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		std::shared_ptr<MovingBehavior> behavior_left = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		std::shared_ptr<MovingBehavior> behavior_right = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		auto life = Life(ENEMY_HEAD_LIFE, ENEMY_HEAD_LIFE);

		auto enemy_left_hand = std::make_shared<Breakout::EnemyLeftHand>(Shape::Rectangle(ENEMY_HEAD_POS - Eigen::Vector2i(100, 0), ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_left, life);
		auto enemy_right_hand = std::make_shared<Breakout::EnemyRightHand>(Shape::Rectangle(ENEMY_HEAD_POS + Eigen::Vector2i(100, 0), ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_right, life);
		enemy = std::make_shared<Breakout::EnemyHead>(Shape::Rectangle(ENEMY_HEAD_POS, ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_head, life, enemy_left_hand, enemy_right_hand);
		
		enemy_manager = std::make_shared<EnemyManager>(MAX_ENEMY_NUM);
	}

	// Townの初期化　
	{
		for (int i = 0; i < HOUSE_NUM; i++) {
			auto realm = Shape::Rectangle(HOUSE_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / HOUSE_NUM, 0), HOUSE_WIDTH, HOUSE_HEIGHT);
			auto life = Life(HOUSE_LIFE, HOUSE_LIFE);
			auto house = std::make_shared<Breakout::House>(realm, life);
			house_list.push_back(house);
		}

		for (int i = 0; i < RESIDENT_NUM; i++) {
			auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
			auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
			std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
				FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
				FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
				FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
			auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior);
			resident_list.push_back(resident);
		}
	}
}
