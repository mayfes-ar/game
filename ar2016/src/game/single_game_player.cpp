#include "game/single_player_game.h"

std::shared_ptr<SinglePlayerGame::Teresa> SinglePlayerGame::makeTeresa(int x = 0, int y = 0, double size = 1.0) {
	if (x == 0 && y == 0) {
		x = rand() % (WIDTH - 100) + 50;
		y = rand() % (HEIGHT - 100) + 50;
	}
	auto enemy = std::make_shared<Teresa>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Teresa>(x, y - 100, *this, size);
	}
	enemyList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::RocketWanwan> SinglePlayerGame::makeRocketWanwan(int x, int y, double size = 1.0) {
auto enemy = std::make_shared<RocketWanwan>(x, y, *this, size);
if (player->isContacted(enemy)) {
enemy = std::make_shared<RocketWanwan>(x, y - 100, *this, size);
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
	}, 150);

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

		bgm = make_shared<BGM>(1);
		bgm->start();

	}, MAX_TIME);

	// mode 2
	mode.setMode([this]() {
		class Title : public Object {
			bool hasPlayerWon = true;
		public:
			Title(bool hasPlayerWon_) {
				layer = 50;
				hasPlayerWon = hasPlayerWon_;
			}

			bool draw() {
				if (hasPlayerWon) {
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_clear"], true);
				}
				else {
					DrawExtendGraph(WIDTH / 2 - 400, 30, WIDTH / 2 + 400, 30 + 296, imgHandles["s_game_result_dead"], true);
					DrawExtendGraph(WIDTH / 2 - 75, 400, WIDTH / 2 + 75, 400 + 150, imgHandles["s_game_dead"], true);
				}
				return true;
			}
		};

		drawList.clear();
		drawList.push_back(make_shared<Title>(hasPlayerWon));

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
		markerList.clear();
		markerList.shrink_to_fit();
		for (int i = 0; i < share.rects.size(); i++) {
			auto marker = std::make_shared<Marker>(share.rects[i], false, i);
			markerList.push_back(marker);
			drawList.push_back(marker);
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
		switch (MAX_TIME - timer) {
		case 300:
		case 500:
		case 100: {
			makeRocketWanwan(0, HEIGHT / 2 + 50);
			break;
		}
		default: {

			break;
		}
		}

		// 定期的に実行する場合など
		if ((MAX_TIME - timer) % 30 == 0) {
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

