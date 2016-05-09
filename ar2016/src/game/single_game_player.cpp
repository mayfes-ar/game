#include "game/single_player_game.h"

std::shared_ptr<SinglePlayerGame::Teresa> SinglePlayerGame::makeTeresa(int x = 0, int y = 0, double size = 1.0) {
	if (x == 0 && y == 0) {
		x = rand() % (WIDTH - 100) + 50;
		y = rand() % (HEIGHT - 100) + 50;
	}
	auto enemy = std::make_shared<Teresa>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Teresa>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::RocketWanwan> SinglePlayerGame::makeRocketWanwan(int x, int y, double size = 1.0) {
auto enemy = std::make_shared<RocketWanwan>(x, y, *this, size);
if (player->isContacted(enemy)) {
enemy = std::make_shared<RocketWanwan>(x, y - 200, *this, size);
}
enemySubList.push_back(enemy);
drawList.push_back(enemy);
return enemy;
}

std::shared_ptr<SinglePlayerGame::Inundation> SinglePlayerGame::makeInundation() {
	auto enemy = std::make_shared<Inundation>(0, HEIGHT+200, *this, 1);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Switch> SinglePlayerGame::makeSwitch(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Switch>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Switch>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Ufo> SinglePlayerGame::makeUfo(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Ufo>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Ray> SinglePlayerGame::makeRay(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Ray>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Cloud> SinglePlayerGame::makeCloud(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Cloud>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Drop> SinglePlayerGame::makeDrop(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Drop>(x, y, *this, size);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Eagle> SinglePlayerGame::makeEagle(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Eagle>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Eagle>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Heiho> SinglePlayerGame::makeHeiho(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Heiho>(x, y, *this, size);
	if (player->isContacted(enemy)) {
		enemy = std::make_shared<Heiho>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

std::shared_ptr<SinglePlayerGame::Fire> SinglePlayerGame::makeFire(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<Fire>(x, y, *this, size);
	
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}

bool willFinishMode = false;

bool SinglePlayerGame::onStart() {
	using namespace std;
	fps.isShow = true;
	
	srand((unsigned int)time(NULL));

	// mode 0
	mode.setMode([this]() {
		class Title : public Object {
			Difficulty& difficulty;
		public:
			Title(Difficulty& difficulty_) : difficulty(difficulty_) {
				layer = 50;
			}

			bool draw() {
				DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_op"], true);
				DrawExtendGraph(WIDTH / 2 - 429 / 2, 30, WIDTH / 2 + 429 / 2, 30 + 47, imgHandles["s_game_op_title"], true);
				// DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);
				DrawString(50, 50, "EASY", difficulty == EASY ? GetColor(255,0,0) : GetColor(255, 255, 255));
				DrawString(50, 100, "HARD", difficulty == HARD ? GetColor(255, 0, 0) : GetColor(255, 255, 255));
				DrawString(50, 150, "NIGHTMARE", difficulty == NIGHTMARE ? GetColor(255, 0, 0) : GetColor(255, 255, 255));
				return true;
			}
		};

		drawList.clear();
		drawList.push_back(make_shared<Title>(difficulty));

		bgm = make_shared<BGM>(0);
		bgm->start();
	}, -1);

	// mode 1
	mode.setMode([this]() {
		drawList.clear();
		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<SingleGameBlockObject>(x, y, width, height, true);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		makeBlock(0 - 200, 650, 200, 200);
		makeBlock(0, 650, 200, 200);
		makeBlock(0 + 200, 650, 200, 200);
		makeBlock(0 + 400, 650, 200, 200);
		makeBlock(0 + 600, 650, 200, 200);
		makeBlock(0 + 800, 650, 200, 200);
		makeBlock(0 + 1000, 650, 200, 200);
		makeBlock(0 + 1200, 650, 200, 200);
		makeBlock(0 + 1400, 650, 200, 200);
		makeBlock(0 - 400, 0, 250, HEIGHT);
		makeBlock(WIDTH + 150, 0, 250, HEIGHT);

		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(share.handle));
		/*
		makeEffect("s_game_coin", 200, 200, 50, 50, true);
		makeEffect("s_game_coin", 250, 200, 50, 50, true, 150, 1, 3);
		makeEffect("s_game_coin", 300, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_coin", 350, 200, 50, 50, true, 150, 2, 3);
		makeEffect("s_game_koumori", 400, 200, 50, 50, true);
		makeEffect("s_game_koumori", 450, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_koumori", 500, 200, 50, 50, true, 150, 3);
		makeEffect("s_game_koumori", 550, 200, 50, 50, true, 150, 4);
		makeEffect("s_game_hit", 600, 200, 50, 50, true, 150, 2);
		makeEffect("s_game_enemy_over", 650, 200, 50, 50, true, 150, 3);
		makeEffect("s_game_sword", 700, 200, 50, 50, true, 150, 2);
		*/

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
	willFinishMode = false;
	funcTimer.update();

	switch (mode.getMode()) {
	case 0: { // イントロダクション
		static int counterForWait = 5;
		if (counterForWait == 0) {
			counterForWait = 5;
			if (key[KEY_INPUT_RETURN]) {
				willFinishMode = true;
			}
			else if (key[KEY_INPUT_1]) {
				difficulty = EASY;
			}
			else if (key[KEY_INPUT_2]) {
				difficulty = HARD;
			}
			else if (key[KEY_INPUT_3]) {
				difficulty = NIGHTMARE;
			}
			else if (key[KEY_INPUT_UP]) {
				difficulty = difficulty == EASY ? NIGHTMARE : difficulty == HARD ? EASY : HARD;
			}
			else if (key[KEY_INPUT_DOWN]) {
				difficulty = difficulty == EASY ? HARD : difficulty == HARD ? NIGHTMARE : EASY;
			}
			else {
				counterForWait = 0;
			}
		}
		else if (counterForWait > 0) {
			counterForWait--;
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
		for (auto enemy : enemySubList) {
			enemyList.push_back(enemy);
		}
		enemySubList.clear();
		enemySubList.shrink_to_fit();

		if (hasPlayerWon && player->deathDecision(enemyList)) {
			hasPlayerWon = false;
			bgm->stop();
			funcTimer.set([this]() {
				willFinishMode = true;
			}, FPS*5);
		}

		// 敵の出現を管理する
		switch (difficulty) {
		case EASY: {
			switch (maxTime - timer) {
			
			case 100: {
				makeRocketWanwan(-RocketWanwan::width/2, HEIGHT / 2 + 50);
				break;
			}
			case 200: {
				makeEagle(0, 0, 1);
				break;
			}
			case 300: {
				makeUfo(0, 50, 1);
				break;
			}
			case 600: {
				makeCloud(0, 50, 1);
				break;
			}
			case 900: {
				makeInundation();
			}
			case 1200:
			case 1230:
			case 1260:
			case 1400: {
				makeHeiho(WIDTH, 300, 1);
				break;
			}
			default: {
			}
			}
			// 定期的に実行する場合など
			if (timer < 150 && (maxTime - timer) % 20 == 0) {
				makeTeresa();
			}
			break;
		}
		case HARD: {
			switch (maxTime - timer) {
			case 100: {
				makeRocketWanwan(-RocketWanwan::width, HEIGHT / 2 + 50);
				break;
			}
			case 200: {
				makeInundation();
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeEagle(400, 0, 1);
				makeEagle(600, 0, 1);
				break;
			}
			case 300: {
				makeUfo(0, 50, 1);
				makeRocketWanwan(WIDTH + RocketWanwan::width, HEIGHT / 2 + 50);
				makeRocketWanwan(WIDTH + RocketWanwan::width, HEIGHT / 2 + 50 + RocketWanwan::height);
				makeRocketWanwan(WIDTH + RocketWanwan::width, HEIGHT / 2 + 50 - RocketWanwan::height);
				makeRocketWanwan(WIDTH+RocketWanwan::width, HEIGHT / 2 + 50 - RocketWanwan::height);
				break;
			}
					  
			case 600: {
				makeEagle(0, 0, 1);
				makeEagle(200, 0, 1);
				makeEagle(400, 0, 1);
				makeEagle(600, 0, 1);
				makeCloud(0, 50, 1);
				makeCloud(200, 50, 1);
				makeCloud(800, 50, 1);
				break;
			}
			case 900: {
				makeInundation();
			}
			case 1410:
				makeRocketWanwan(-RocketWanwan::width, HEIGHT / 2 + 50);
			case 1420:
			case 1430:
			case 1440:
			case 1200:
			case 1230:
			case 1260:
			case 1400: {
				makeHeiho(WIDTH, 300, 1);
				break;
			}
			case 1500: {
				makeRocketWanwan(WIDTH + RocketWanwan::width, HEIGHT / 2 + 50);
				makeRocketWanwan(-RocketWanwan::width, HEIGHT / 2 + 50);
				break;
			}

			default: {
			}
			}

			// 定期的に実行する場合など
			if (timer < 150 && (maxTime - timer) % 5 == 0) {
				makeTeresa();
			}
			break;
		}
		case NIGHTMARE: {
			switch (maxTime - timer) {
			case 400:
			case 700:
			case 1000:
			case 1200:
			case 1400:
			case 100: {
				makeInundation();
			}
			default: {
			}
			}

			// 定期的に実行する場合など
			if (timer < 150 && (maxTime - timer) % 3 == 0) {
				makeTeresa();
			}
			else if ((maxTime - timer) % 30 == 0){
				makeTeresa();
			}
			if ((maxTime - timer) % 10 == 0 && rand()%2 == 0) {
				makeHeiho(WIDTH, 250 + rand()%100, 1);
			}
			if ((maxTime - timer) % 30 == 0 && rand() % 3 == 0) {
				makeRocketWanwan(rand()%2 == 0 ? -RocketWanwan::width : WIDTH+RocketWanwan::width, HEIGHT / 2 + 50);
			}
			if ((maxTime - timer) % 30 == 0 && rand() % 3 == 0) {
				if (rand() % 10 == 0) {
					makeEagle(-1000, -1000, 10);
				}
				else {
					makeEagle(-600, 0, 1);
					makeEagle(-400, 0, 1);
					makeEagle(-200, 0, 1);
					makeEagle(0, 0, 1);
					makeEagle(200, 0, 1);
					makeEagle(400, 0, 1);
					makeEagle(600, 0, 1);
				}
			}
			if ((maxTime - timer) % 100 == 0 && rand() % 2 == 0) {
				makeCloud(0, 50, 1);
			}
			if ((maxTime - timer) % 100 == 0 && rand() % 2 == 0) {
				makeUfo(WIDTH, 100, 1);
			}
			break;
		}
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

