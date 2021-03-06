﻿#include "game/breakout_components.h"
#include <random>

using namespace Breakout;


void BreakoutComponents::setup(ShareData& share)
{
	//InfoHime
	{
		Shape::Rectangle realm = Shape::Rectangle(Eigen::Vector2i(WIDTH / 2, HEIGHT / 4), INFO_HIME_WIDTH, INFO_HIME_HEIGHT);
		info_hime = std::make_shared<InfoHime>("難易度を選んでね♪", 100000);
	}

	// Select画面
	{
		std::unordered_map<Breakout::Mode, std::string, Breakout::ModeEnumHash> mode_image_store;
		mode_image_store[Breakout::Mode::Easy] = "b_easy";
		mode_image_store[Breakout::Mode::Normal] = "b_normal";
		mode_image_store[Breakout::Mode::Hard] = "b_hard";
		std::list<Breakout::Mode> mode_list;
		mode_list.push_back(Breakout::Mode::Easy);
		mode_list.push_back(Breakout::Mode::Normal);
		mode_list.push_back(Breakout::Mode::Hard);
		const Shape::Rectangle select_realm = Shape::Rectangle(SELECT_START_POS, SELECT_WIDTH, SELECT_HEIGHT);
		select = std::make_shared<Breakout::Select<Breakout::Mode, Breakout::ModeEnumHash>>(mode_list, mode_image_store, select_realm);
	}
	// Layoutの初期化
	{
		background = std::make_shared<Background>(share.handle);
		background->init();
		auto info_realm = Shape::Rectangle(INFO_START_POS, INFO_WIDTH, INFO_HEIGHT);
		std::shared_ptr<Timer> timer = std::make_shared<Timer>(TIMER_MAX_MIN, TIMER_MAX_SEC, TIMER_MAX_MSEC);
		info = std::make_shared<Breakout::Info>(info_realm, timer);

		info->addScoreCalc([&]() -> int {
			return SCORE_OFFSET;
		});

		info->addScoreCalc([&]() -> int {
			std::size_t block_dis_num = std::count_if(block_list.begin(), block_list.end(), 
				[](const std::shared_ptr<Block>& block) -> bool {
				return block->isDisappeared();
			});
		
			return BLOCK_SCORE * (block_dis_num - m_block_offset);
		});

		info->addScoreCalc([&]() -> int {
			std::size_t house_now_num = house_list.size();
			return HOUSE_SCORE * (HOUSE_NUM - house_now_num);
		});

		info->addScoreCalc([&]() -> int {
			return ENEMY_SCORE * enemy_manager->getGeneratedEnemy();
		});

		info->addScoreCalc([&]() -> int {
			int enemy_head = enemy->isAlive() ? 0 : 1;
			int enemy_left = enemy->hasLeft() ? 0 : 1;
			int enemy_right = enemy->hasRight() ? 0 : 1;

			return BOSS_HEAD_SCORE * enemy_head + BOSS_RIGHT_SCORE * enemy_right + BOSS_LEFT_SCORE * enemy_left;
		});


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
		const Shape::Rectangle realm(POT_START_POS, FIREBALL_RADIUS * 4, FIREBALL_RADIUS * 4);
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
				else /*if (ratio > 0.4)*/ {
					item = item = std::make_shared<Item>(Breakout::ItemKind::EnhanceShip);;
						//std::make_shared<Item>(Breakout::ItemKind::RestorePot);
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
		m_block_offset = std::count_if(block_list.begin(), block_list.end(), 
			[](const std::shared_ptr<Block>& block) -> bool {
			return block->isDisappeared(); });
	}

	// Enemyの初期化
	{
		std::shared_ptr<MovingBehavior> behavior_head = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		std::shared_ptr<MovingBehavior> behavior_left = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		std::shared_ptr<MovingBehavior> behavior_right = std::make_shared<StringBehavior>(Eigen::Vector2f{ 100.0f, 0.0f }, 0.03f);
		auto life = Life(ENEMY_HEAD_LIFE, ENEMY_HEAD_LIFE);

		auto enemy_left_hand = std::make_shared<Breakout::EnemyLeftHand>(Shape::Rectangle(ENEMY_HEAD_POS + Eigen::Vector2i(-100, 20), ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_left, life);
		auto enemy_right_hand = std::make_shared<Breakout::EnemyRightHand>(Shape::Rectangle(ENEMY_HEAD_POS + Eigen::Vector2i(100, 20), ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_right, life);
		enemy = std::make_shared<Breakout::EnemyHead>(Shape::Rectangle(ENEMY_HEAD_POS, ENEMY_HEAD_WIDTH, ENEMY_HEAD_WIDTH), behavior_head, life, enemy_left_hand, enemy_right_hand);
		enemy->setDeadEffect("b_boss_dead_effect", 4);
		enemy_manager = std::make_shared<EnemyManager>(MAX_ENEMY_NUM);
	}

	// Townの初期化　
	{
		for (int i = 0; i < NORMAL_HOUSE_NUM; i++) {
			if (i >= (int)NORMAL_HOUSE_NUM / 2) {
				auto realm = Shape::Rectangle(HOUSE_START_POS + Eigen::Vector2i(FIELD_WIDTH * (i+1) / HOUSE_NUM, 0), NORMAL_HOUSE_WIDTH, NORMAL_HOUSE_HEIGHT);
				auto life = Life(NORMAL_HOUSE_LIFE, NORMAL_HOUSE_LIFE);
				auto house = std::make_shared<Breakout::House>(realm, life);
				house_list.push_back(house);
				continue;
			}
			auto realm = Shape::Rectangle(HOUSE_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / HOUSE_NUM, 0), NORMAL_HOUSE_WIDTH, NORMAL_HOUSE_HEIGHT);
			auto life = Life(NORMAL_HOUSE_LIFE, NORMAL_HOUSE_LIFE);
			auto house = std::make_shared<Breakout::House>(realm, life);
			house_list.push_back(house);
		}

		auto realm = Shape::Rectangle(HOUSE_START_POS + Eigen::Vector2i(FIELD_WIDTH * (int)(NORMAL_HOUSE_NUM / 2) / HOUSE_NUM, 0), CASTLE_HOUSE_WIDTH, CASTLE_HOUSE_HEIGHT);
		auto life = Life(CASTLE_HOUSE_LIFE, CASTLE_HOUSE_LIFE);
		auto house = std::make_shared<Breakout::House>(realm, life, Castle);
		house_list.push_back(house);

		for (int i = 0; i < RESIDENT_NUM; i++) {
			if (i <= 1 ) {
				auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
				auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
					FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
				auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior, Boy);
				resident_list.push_back(resident);
				continue;
			} else if (i >= 3) {
				auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
				auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
					FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
				auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior, Girl);
				resident_list.push_back(resident);
				continue;
			} else {
				auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
				auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
				std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
					FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
					FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
				auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior, Hime);
				resident_list.push_back(resident);
				continue;
			}
		}
	}

	const auto count_down_realm = Shape::Rectangle(COUNT_DOWN_START_POS, COUNT_DOWN_WIDTH, COUNT_DOWN_HEIGHT);
	count_down = std::make_shared<CountDown>(count_down_realm);

}

void BreakoutComponents::increaseBlock(double ratio)
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_real_distribution<> block_reappear_generator(0.0, 1.0);
	std::for_each(block_list.begin(), block_list.end(), [&](const std::shared_ptr<Block>& block) {
		if (block->isDisappeared() && block_reappear_generator(mt) < ratio) {
			block->appear();
		}

	});
}
