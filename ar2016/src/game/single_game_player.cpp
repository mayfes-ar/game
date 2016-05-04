﻿#include "game/single_player_game.h"

std::shared_ptr<SinglePlayerGame::Teresa> SinglePlayerGame::makeTeresa(int x = 0, int y = 0, double size = 1.0) {
	if (x == 0 && y == 0) {
		x = rand() % (WIDTH - 100) + 50;
		y = rand() % (HEIGHT - 100) + 50;
	}
	auto enemy = std::make_shared<Teresa>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Teresa>(x, y - 200, *this, size);
	}
	enemyList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::RocketWanwan> SinglePlayerGame::makeRocketWanwan(int x, int y, double size = 1.0) {
auto enemy = std::make_shared<RocketWanwan>(x, y, *this, size);
if (player->isContacted(enemy)) {
enemy = std::make_shared<RocketWanwan>(x, y - 200, *this, size);
}
enemyList.push_back(enemy);
drawList.push_back(enemy);
return enemy;
}

std::shared_ptr<SinglePlayerGame::Inundation> SinglePlayerGame::makeInundation() {
	auto enemy = std::make_shared<Inundation>(0, HEIGHT+200, *this, 1);

	enemyList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Switch> SinglePlayerGame::makeSwitch(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Switch>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Switch>(x, y - 200, *this, size);
	}
	enemyList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

bool SinglePlayerGame::onStart() {
	using namespace std;
	fps.isShow = true;

	srand((unsigned int)time(NULL));

	// mode 0
	mode.setMode([this]() {
		class Title : public Object {
		public:
			Title() {
				layer = 50;
			}

			bool draw() {
				DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_op"], true);
				DrawExtendGraph(WIDTH / 2 - 429 / 2, 30, WIDTH / 2 + 429 / 2, 30 + 47, imgHandles["s_game_op_title"], true);
				// DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);
				return true;
			}
		};

		drawList.clear();
		drawList.push_back(make_shared<Title>());

		bgm = make_shared<BGM>(0);
		bgm->start();

		for (int i = 0; i < 20; i++);
	}, -1);

	// mode 1
	mode.setMode([this]() {
		drawList.clear();
		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<SingleGameBlockObject>(x, y, width, height, true);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		makeBlock(0 - 200, 600, WIDTH + 200 + 200, 50);

		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(share.handle));
		makeEffect("s_game_coin", 200, 200, 50, 50, true);
		makeEffect("s_game_coin", 250, 200, 50, 50, true, 150, 1, 3);
		makeEffect("s_game_coin", 300, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_coin", 350, 200, 50, 50, true, 150, 2, 3);

		share.rectMutex.lock();
		markerList.clear();
		markerList.shrink_to_fit();
		for (int i = 0; i < share.rects.size(); i++) {
			auto marker = std::make_shared<Marker>(share.rects[i], false, i, *this);
			markerList.push_back(marker);
			drawList.push_back(marker);
		}
		share.rectMutex.unlock();

		bgm = make_shared<BGM>(1);
		bgm->start();

	}, maxTime);

	// mode 2
	mode.setMode([this]() {
		class Title : public Object {
			bool hasPlayerWon = true;
			std::shared_ptr<SinglePlayerGame::Player> player;
			int maxTime;
			int timer;
			
		public:
			Title(bool hasPlayerWon_, std::shared_ptr<SinglePlayerGame::Player> player_, int maxTime_, int timer_) {
				layer = 50;
				hasPlayerWon = hasPlayerWon_;
				player = player_;
				maxTime = maxTime_;		
				timer = timer_;
			}

			bool draw() {
				if (hasPlayerWon) {
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_clear"], true);
					std::string clearScore = "Score : " + std::to_string(maxTime + (player->getMaxDamage() - player->getPlayerDamage()) * 50);
					DrawString(100, 150, clearScore.c_str(), GetColor(0, 0, 0));
				}
				else {
					DrawExtendGraph(WIDTH / 2 - 400, 30, WIDTH / 2 + 400, 30 + 296, imgHandles["s_game_result_dead"], true);
					DrawExtendGraph(WIDTH / 2 - 75, 400, WIDTH / 2 + 75, 400 + 150, imgHandles["s_game_dead"], true);
					std::string deadScore = "Score : " + std::to_string(maxTime - timer);
					std::string playTime = "Time : " + std::to_string((maxTime - timer) / 30);
					DrawString(100, 150, deadScore.c_str(), GetColor(255, 255, 255));
					DrawString(100, 200, playTime.c_str(), GetColor(255, 255, 255));
				}
				
				std::string damage = ("Damage : " + std::to_string(player->getPlayerDamage()));
				DrawString(100, 100, damage.c_str(), GetColor(255, 0, 0));
				
				return true;
			}
		};

		drawList.clear();
		drawList.push_back(make_shared<Title>(hasPlayerWon, player, maxTime, timer ));

		bgm->stop();
		bgm = make_shared<BGM>(2, hasPlayerWon);
		bgm->start();
	}, -1);

	return Game::onStart();
}

bool SinglePlayerGame::onUpdate() {
	bool willFinishMode = false;

	switch (mode.getMode()) {
	case 0: { // イントロダクション
		static int counterForWaiting = 10;
		if (key[KEY_INPUT_RETURN] && counterForWaiting == 0) {
			willFinishMode = true;
		}
		if (counterForWaiting > 0) {
			counterForWaiting--;
		}
		break;
	}
	case 1: { // playing
		timer -= 1;
		if (timer <= 0) {
			willFinishMode = true;
		}

		// 認識したマーカーを描画
		share.rectMutex.lock();
		for (auto marker : markerList) {
			marker->setRect(share.rects[marker->getIndex()]);
		}
		share.rectMutex.unlock();

		player->update(key);

		for (auto enemy : enemyList) {
			enemy->deathDecision();
			enemy->update();
		}

		if (player->deathDecision(enemyList)) {
			bgm->stop();
			bgm->playDeadSound();
			hasPlayerWon = false;
			willFinishMode = true;
		}

		// 敵の出現を管理する
		switch (maxTime - timer) {
		case 300:
			makeInundation();
			makeSwitch(100, 100, 1);
		case 500:
		case 100: {
			makeRocketWanwan(-RocketWanwan::width, HEIGHT / 2 + 50);
			break;
		}
		default: {

			break;
		}
		}

		// 定期的に実行する場合など
		if ((maxTime - timer) % 30 == 0) {
			makeTeresa();
		}
		break;

	}
	case 2: { // リザルト画面
		if (key[KEY_INPUT_RETURN]) {
			willFinishMode = true;
		}
		break;
	}

	default:

		break;
	}


	if (key[KEY_INPUT_ESCAPE]) {
		share.willFinish = true;
	}
	if (key[KEY_INPUT_N]) {
		willFinishMode = true;
	}


	for (auto& itr = enemyList.begin(); itr != enemyList.end();) {
		if ((*itr)->getIsAlive()) {
			++itr;
		}
		else {
			itr = enemyList.erase(itr);
		}
	}

	if (willFinishMode) {

		mode.goNext();
	}

	return Game::onUpdate();

}

bool SinglePlayerGame::onFinish() {
	bgm->stop();

	thread.join();
	return true;
}

