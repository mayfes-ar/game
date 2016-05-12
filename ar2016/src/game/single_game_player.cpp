#include "game/single_player_game.h"
#include <typeinfo.h>

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


std::shared_ptr<SinglePlayerGame::tutoHeiho> SinglePlayerGame::maketutoHeiho(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<tutoHeiho>(x, y, *this, size);
	if (tutoplayer->isContacted(enemy)) {
		enemy = std::make_shared<tutoHeiho>(x, y - 200, *this, size);
	}
	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}
	
std::shared_ptr<SinglePlayerGame::tutoFire> SinglePlayerGame::maketutoFire(int x, int y, double size = 1.0) {
	auto enemy = std::make_shared<tutoFire>(x, y, *this, size);

	enemySubList.push_back(enemy);
	drawList.push_back(enemy);
	return enemy;
}



bool willFinishMode = false;

bool SinglePlayerGame::onStart() {
	using namespace std;
	fps.isShow = true;

	srand((unsigned int)time(NULL));

	// INTRO
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
				DrawString(50, 50, "EASY", difficulty == EASY ? GetColor(255, 0, 0) : GetColor(255, 255, 255));
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

	// TUTORIAL
	mode.setMode([this]() {
		maxPlayerDamage = difficulty == EASY ? 5 : difficulty == HARD ? 10 : 20;
		tutoplayer = std::make_shared<tutoPlayer>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, tutoPlayer::width, tutoPlayer::height, "s_game_player", maxPlayerDamage, *this);


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


		drawList.push_back(tutoplayer);
		drawList.push_back(make_shared<Background>(share.handle));


		share.rectMutex.lock();
		markerList.clear();
		markerList.shrink_to_fit();
		for (int i = 0; i < share.rects.size(); i++) {
			auto marker = std::make_shared<Marker>(share.rects[i], false, i, *this);
			markerList.push_back(marker);
			drawList.push_back(marker);
		}
		share.rectMutex.unlock();

		class Title : public Object {
			bool start = false;
			int timer = 0;
			std::shared_ptr<SinglePlayerGame::tutoHeiho> tutoenemy;
			Tutorial *tutorial;
			std::string tutosen1 = "あ、危ない！！";
			std::string tutosen2 = "お姫様に火の玉が当たりそうです！！";
			std::string tutosen3 = "お姫様を守るために盾を使ってみましょう";
			std::string tutosen4 = "マーカーを火の玉に当ててみてください";
			std::string tutosen5 = "剣を使えば敵を倒すこともできます";
			std::string tutosen6 = "盾と剣を使ってお姫様を敵から守りましょう！！";
			std::string tutosen7 = "これでチュートリアルを終了します";


		public:
			Title(std::shared_ptr<SinglePlayerGame::tutoHeiho> &tutoenemy_, SinglePlayerGame& game_) {
				layer = 50;
				tutoenemy = tutoenemy_;
				tutorial = &game_.tutorial;
			}

			void update() {

				if (tutoenemy->childfire != NULL) {
					if (tutoenemy->childfire->getIsFreezed() && start == false) {
						start = true;
					}
					if (start == true) {
						timer++;
					}
				}
			}

			bool draw() {
				update();
				if (timer <= FPS * 2 / 3) {

				}
				else if (timer <= FPS * 2) {
					DrawString(550, 350, tutosen1.c_str(), GetColor(0, 0, 0));
				}
				else if (timer <= FPS * 4) {
					DrawString(550, 350, tutosen2.c_str(), GetColor(0, 0, 0));
				}
				else if (timer <= FPS * 6) {
					DrawString(550, 350, tutosen3.c_str(), GetColor(0, 0, 0));
				}
				else if (timer <= FPS * 8) {
					*tutorial = BEATFIRE;
					DrawString(550, 350, tutosen4.c_str(), GetColor(0, 0, 0));
					if (timer == FPS * 8 && tutoenemy->childfire->getIsAlive()) {
						timer--;
					}
				}
				else if (timer <= FPS * 10) {
					*tutorial = BEATHEIHO;
					DrawString(550, 350, tutosen5.c_str(), GetColor(0, 0, 0));
					if (timer == FPS * 10 && tutoenemy->getIsAlive()) {
						timer--;
					}
				}
				else if (timer <= FPS * 12) {
					DrawString(550, 350, tutosen6.c_str(), GetColor(0, 0, 0));
				}
				else if (timer <= FPS * 14) {
					DrawString(550, 350, tutosen7.c_str(), GetColor(0, 0, 0));
				}
				else {
					*tutorial = END;

				}
				return true;
			}

		};
		if (tutorial == END) {
			makeEffect("s_game_curtain_close", 0, 0, WIDTH, HEIGHT, false, 160, 2, 1);

		}
		tutoenemy = maketutoHeiho(WIDTH, 300, 1);
		drawList.push_back(make_shared<Title>(tutoenemy, *this));


	}, -1);

	// GAME
	mode.setMode([this]() {
		
		drawList.clear();
		makeEffect("s_game_curtain_open", 0, 0, WIDTH, HEIGHT, false, 160, 2, 0);

		maxPlayerDamage = difficulty == EASY ? 5 : difficulty == HARD ? 10 : 20;
		player = std::make_shared<Player>(WIDTH / 2 - 100 / 2, HEIGHT / 2 - 150 / 2, Player::width, Player::height, "s_game_player", maxPlayerDamage, *this);




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

	// RESULT
	mode.setMode([this]() {
		
	
		drawList.clear();	
		makeEffect("s_game_curtain_open", 0, 0, WIDTH, HEIGHT, false, 150, 2, 0);


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

			//クリアの文
			const std::string clear1 = "おめでとうございます。";
			const std::string clear2 = "見事お姫様を守り抜くことができました。";
			const std::string clear3 = "彼女はあなたのことを「好き」と言っています。";
			const std::string clear4 = "あなたはどうですか。";
			const std::string clear5 = "姫を守るために取った「剣」の重さ";
			const std::string clear6 = "どうか覚えていてくださいね。";
			const std::string clear7 = "遊んでくれてありがとうございました。";

			//ゲームオーバーの文
			const std::string dead1 = "あなたは姫を守り切れませんでした。";
			const std::string dead2 = "これは単なる「ゲーム」でしたか？";
			const std::string dead3 = "あなたの握った剣はニセモノでしたか？";
			const std::string dead4 = "";
			const std::string dead5 = "姫はどこかできっとあなたを待ち続けています。";
			const std::string dead6 = "次があったら、ちゃんと守れるといいですね。";
			const std::string dead7 = "遊んでくれてありがとうございました。";


			bool draw() {
				rect.y += 2;//文章スクロール用
				
				if (hasPlayerWon) {

					//********勝利画面
					//画像
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_background1"], true);
					DrawExtendGraph(650, 450, 650 + 320, 450 + 311, imgHandles["s_game_result_castle"], true);
					DrawExtendGraph(900, 170, WIDTH-80, 380, imgHandles["s_game_result_frame_star"], true);					
					DrawExtendGraph(800, 20, WIDTH, 230, imgHandles["s_game_result_rainbow"], true);
					DrawExtendGraph(1100, 1046 / 3 + 100, 1100 + 621 / 3, 1046 / 3 + 1046 / 3 + 100, imgHandles["s_game_player"], true);
					DrawExtendGraph(20, 20, WIDTH/2-20, HEIGHT-20, imgHandles["s_game_result_background_gameover1"], true);
					//スクロール文章
					
					DrawString(40, HEIGHT - rect.y, clear1.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 150 - rect.y, clear2.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 300 - rect.y, clear3.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 450 - rect.y, clear4.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 600 - rect.y, clear5.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 750 - rect.y, clear6.c_str(), GetColor(25, 25, 25));
					DrawString(40, HEIGHT + 900 - rect.y, clear7.c_str(), GetColor(25, 25, 25));

					//スコア
					std::string clearScore = "得点 : " + std::to_string(maxTime + (player->getMaxDamage() - player->getPlayerDamage()) * 50);
					DrawString(940, 230, clearScore.c_str(), GetColor(225, 225, 225));
					std::string damage = ("受けたダメージ : " + std::to_string(player->getPlayerDamage()));
					DrawString(940, 300, damage.c_str(), GetColor(255, 225, 225));

					
					
				}

				else {

					//******ゲームオーバー画面

					//画像・ゲームオーバー
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_background2"], true);
					DrawExtendGraph(170, 300, 170 + 1600 / 5, HEIGHT, imgHandles["s_game_result_sketch"], true);
					DrawExtendGraph(621 / 3 - 50, 450, 621 / 3 + 621 / 4 - 50, HEIGHT, imgHandles["s_game_player_drowned"], true);
					DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_stage1"], true);
					DrawExtendGraph(220, 100, 520, 320, imgHandles["s_game_result_frame_blackstar"], true);

					//スクロール文章・ゲームオーバー
					DrawString(600, HEIGHT - rect.y, dead1.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+150 - rect.y, dead2.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+300 - rect.y, dead3.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+450 - rect.y, dead4.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT+600 - rect.y, dead5.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT + 750 - rect.y, dead6.c_str(), GetColor(225, 225, 225));
					DrawString(600, HEIGHT + 900 - rect.y, dead7.c_str(), GetColor(225, 225, 225));

					//スコア
					std::string deadScore = "得点 : " + std::to_string(maxTime - timer);
					std::string playTime = "記録 : " + std::to_string((maxTime - timer) / 30) + "秒";
					DrawString(260, 150, deadScore.c_str(), GetColor(225, 225, 225));
					DrawString(260, 200, playTime.c_str(), GetColor(225, 225, 225));
					std::string damage = ("受けたダメージ : " + std::to_string(player->getPlayerDamage()));
					DrawString(260, 250, damage.c_str(), GetColor(225, 10, 10));

					//DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["s_game_result_frame_blackstar"], true);
				}
							
				return true;
			}
		};

		drawList.push_back(make_shared<Title>(hasPlayerWon, player, maxTime, timer));

		//yu
		if (hasPlayerWon) {
			//勝利画面のエフェクト・リザルト
			makeEffect("s_game_result_hanabi", 400, 100, 500, 500, true, 50, 1, 4);
			makeEffect("s_game_result_hanabi", 700, 50, 300, 300, true, 50, 1, 3);

			}
		else {
			//ゲームオーバーのエフェクト・リザルト	
			makeEffect("s_game_result_kirakira", 150, 600, 600, 600, true, 150, 1, 4);
			
		}

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
	case INTRO: { // イントロダクション
		static int counterForWait = 5;
		if (counterForWait == 0) {
			counterForWait = 5;
			if (key[KEY_INPUT_RETURN]) {
				willFinishMode = true;
				drawList.clear();
				makeEffect("s_game_curtain_close", 0, 0, WIDTH, HEIGHT, false, 400, 2, 1);
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
	case TUTORIAL: {

		if (tutorial != START) {
			// 認識したマーカーを描画
			share.rectMutex.lock();
			for (auto marker : markerList) {
				marker->setRect(share.rects[marker->getIndex()]);
			}
			share.rectMutex.unlock();
			
		}
		
		switch (tutorial) {
		case START: {
			if (heihoFreezeTimeRemain >= 0) {					
				heihoFreezeTimeRemain--;
				
			}
			if(heihoFreezeTimeRemain == FPS*4 - 25){
				makeEffect("s_game_curtain_open", 0, 0, WIDTH, HEIGHT, false, 400, 2, 0);
			}

			if (heihoFreezeTimeRemain == 0) {
				tutoenemy->freeze();
				tutoenemy->childfire->freeze();

			}
			break;
		}
		case BEATFIRE: {//Titleクラス
			if (tutoenemy->childfire->getIsAlive()) {
				tutoenemy->childfire->deathDecision();
			}
			break;
		}
		case BEATHEIHO: {//Titleクラス
			if (tutoenemy->getIsAlive()) {
				tutoenemy->deathDecision();
			}
			break;
		}
		case END: {//Titleクラス
			willFinishMode = true;
			//makeEffect("s_game_curtain_close", 0, 0, WIDTH, HEIGHT, false, 150, 2, 1);
			break;
		}
		}


		for (auto enemy : enemyList) {
			enemy->update();
		}
		for (auto enemy : enemySubList) {
			enemyList.push_back(enemy);
		}
		enemySubList.clear();
		enemySubList.shrink_to_fit();
		tutoplayer->update(key);

		//////////////////デバッグ用チート設定//////////////////////////
		if (key[KEY_INPUT_1]) {
			willFinishMode = true;
		}
		break;
	}
	case GAME: { // playing

		////////////////////デバッグ用チート////////////////////////////////
		if (key[KEY_INPUT_2]) {
			timer = 0;
		}
		if (key[KEY_INPUT_3]) {
			willFinishMode = true;
			hasPlayerWon = false;
		}


		timer -= 1;
		
		if (timer <= 0) {
			willFinishMode = true;
			makeEffect("s_game_curtain_close", 0, 0, WIDTH, HEIGHT, false, 300, 2, 0);
			//////////////////////////////////////////////////////////////////////////////////////////////////
			//デバッグ時、クリアしたとき挙動がおかしかったら（途中で消えてしまうなど）このカーテンはやめる
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
			case 950: {
				makeCloud(0, 50, 1);
				break;
			}
			case 1100: {
				makeUfo(0, 50, 1);
				break;
			}
			case 1260: {
				makeEagle(0, 0, 1);
				break;
			}
			case 1400: {
				makeInundation();
			}
			case 1600: {
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
	case RESULT: { // リザルト画面

		
		result_timer -= 1;

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
	static int waitNCounter = FPS / 3;
	if (key[KEY_INPUT_N] && waitNCounter == 0) {
		willFinishMode = true;
		waitNCounter = FPS / 3;
	}
	if (waitNCounter > 0) { waitNCounter--; }


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

