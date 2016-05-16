#pragma once

#include <thread>
#include "game/game.h"
#include "game/image_process.h"
#include "object/breakout_object.h"
#include "game/breakout_components.h"
#include <random>
#include <typeinfo>


class BreakoutGame : public Game
{
public:
	// コンストラクタ
	explicit BreakoutGame() : m_components(new BreakoutComponents) {}

	~BreakoutGame() {
		m_components->~BreakoutComponents();
		/*m_components->background.~shared_ptr();
		m_components->block_list.~vector();
		m_components->enemy.~shared_ptr();
		m_components->ship.~shared_ptr();
		m_components->enemy_manager.~shared_ptr();
		m_components->explanation.~shared_ptr();
		m_components->field.~shared_ptr();
		m_components->fireball_manager.~shared_ptr();
		m_components->house_list.~vector();
		m_components->info.~shared_ptr();
		m_components->pot.~shared_ptr();
		m_components->result.~shared_ptr();
		m_components->item_list.~vector();
		m_components->resident_list.~vector();
		m_components->select.~shared_ptr();*/
	}

	bool onStart() override
	{
		init();

		//select
		mode.setMode([&] {
			drawList.push_back(m_components->background);
			drawList.push_back(m_components->select);
			drawList.push_back(m_components->info_hime);
		}, -1);

		//tutorial_ship
		mode.setMode([&] {
			setup_tutorial_ship();
		}, -1);

		//tutorial_pot
		mode.setMode([&] {
			setup_tutorial_pot();
		}, -1);

		mode.setMode([&] {
			m_components->count_down->init();
			drawList.clear();
			drawList.push_back(m_components->background);
			drawList.push_back(m_components->count_down);
		}, -1);


		mode.setMode([&]() {
			
			m_components->ship->resetShip();
			drawList.clear();
			m_components->info->init();
			drawList.push_back(m_components->background);
			drawList.push_back(m_components->info);
			drawList.push_back(m_components->field);
			drawList.push_back(m_components->explanation);


			for (const auto& block : m_components->block_list) {
				drawList.push_back(block);
			}
			drawList.push_back(m_components->fireball_manager);
			drawList.push_back(m_components->ship);
			drawList.push_back(m_components->pot);
			for (const auto& item : m_components->item_list) {
				drawList.push_back(item);
			}
			drawList.push_back(m_components->enemy);
			for (const auto& house : m_components->house_list) {
				drawList.push_back(house);
			}
			for (const auto& resident : m_components->resident_list) {
				drawList.push_back(resident);
			}
			drawList.push_back(m_components->enemy_manager);
		}, -1);

		// Result画面
		mode.setMode([this]() {
			using namespace Breakout;
			m_components->ship.~shared_ptr();
			m_components->fireball_manager.~shared_ptr();
			drawList.clear();
			if (m_components->result->isClear()) {
				m_components->resident_list.clear();
				m_components->himes.clear();
				m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  あなたのおかげで\n町の平和は\n取り戻されたわ!!\nありがとう!!", 200, "b_hime"));
				m_components->himes.push_back(std::make_shared<Breakout::InfoHime>("  あなたの得点は", 200, "b_hime"));

				drawList.push_back(m_components->himes[0]);
				for (int i = 0; i < RESIDENT_NUM; i++) {
					if (i <= 2) {
						auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
						auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
						std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
							FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
							FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
							FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
						auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior, Boy);
						m_components->resident_list.push_back(resident);
						continue;
					}
					else {
						auto realm = Shape::Rectangle(RESIDENT_START_POS + Eigen::Vector2i(FIELD_WIDTH * i / RESIDENT_NUM, 0), RESIDENT_WIDTH, RESIDENT_HEIGHT);
						auto life = Life(RESIDENT_LIFE, RESIDENT_LIFE);
						std::shared_ptr<MovingBehavior> rnd_behavior = std::make_shared<RandomBehavior>(FIELD_START_POS.x(),
							FIELD_START_POS.x() + FIELD_WIDTH - RESIDENT_WIDTH,
							FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT,
							FIELD_START_POS.y() + FIELD_HEIGHT - RESIDENT_HEIGHT);
						auto resident = std::make_shared<Breakout::Resident>(realm, life, rnd_behavior, Girl);
						m_components->resident_list.push_back(resident);
						continue;
					}
				}
				m_components->result->init();
				for (auto& resident : m_components->resident_list) {
					drawList.push_back(resident);
				}
			}
			drawList.push_back(m_components->result);
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() override
	{
		if (key[KEY_INPUT_ESCAPE]) {
			share.willFinish = true;
		}
		updateGameState();

		return Game::onUpdate();
	}

	bool onFinish() override
	{
		drawList.clear();
		m_detect_thread.join();
		//m_detect_thread.detach();
		return true;
	}
private:
	bool m_is_mode_selected = false;
    std::thread m_detect_thread;

	BreakoutComponents* m_components = new BreakoutComponents();

	void init()
	{
		fps.isShow = true;
		// 認識スレッドを回す
		m_detect_thread = std::thread(capture, std::ref(share));

		m_components->setup(share);
		m_is_mode_selected = false;
    }

	// すべての衝突判定
	// Fireball, Itemなど
	void updateCollisionDetection();

	// マーカからの情報から舟を移動
	void moveShip();

	// Game画面の状態更新
	void updateGameState();

	// FireBallとBlockのあたり判定をし、blockを消すかを決める
	void updateBlockStatus();

	// PotとFireBallの当たり判定をし、fireballを吸い込むかを決める
	void updatePotStatus();

	// Enemyのupdate
	void updateEnemy();

	// Townのupdate
	void updateTown();

	// ShipとEnemyの接触判定
	void shipVSEnemy();

	// EnemyとTownの戦闘
	void EnemyVSTown();

	// firebalのupdate
	void updateFireballPosition();

	// Infoのupdate
	void updateInfoHime();

	// shipのチュートリアル
	bool tutorial_ship();
	void setup_tutorial_ship();

	// potのチュートリアル
	bool tutorial_pot();
	void setup_tutorial_pot();

	bool returnResult();

	// ゲームをクリアしたかどうか
	// 現在はBlockが一つもない場合はクリアとみなす
	bool isGameClear() const;
};
