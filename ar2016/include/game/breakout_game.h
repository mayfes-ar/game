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
		mode.setMode([&] {
			drawList.push_back(m_components->background);
			drawList.push_back(m_components->select);
		}, -1);

		mode.setMode([&]() {
			drawList.clear();
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
			m_components->result->init();
			drawList.clear();
			drawList.push_back(m_components->result);
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() override
	{
		switch (mode.getMode()) {
		case 1:
			updateCollisionDetection();
			updateFireballPosition();
			moveShip();
			updateBlockStatus();
			updatePotStatus();
			updateEnemy();
			updateTown();
			shipVSEnemy();
			EnemyVSTown();
			updateInfo();
			break;
		default:
			break;
		}

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
		m_components->info->init();
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
	void updateInfo();

	// ゲームをクリアしたかどうか
	// 現在はBlockが一つもない場合はクリアとみなす
	bool isGameClear() const;
};
