#pragma once

#include "game/game.h"

class SinglePlayerGame : public Game {

	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() {
			// DrawExtendGraph(CAP2IMG_SHIFT_X, CAP2IMG_SHIFT_Y, CAP2IMG_SHIFT_X + CAP2IMG_RATE*CAP_WIDTH, CAP2IMG_SHIFT_Y + CAP2IMG_RATE*CAP_HEIGHT, handle, FALSE);
			SetDrawBright(40, 40, 40);
			DrawExtendGraph(0, 0, WIDTH, HEIGHT, imgHandles["ar2016_logo"], true);
			SetDrawBright(255, 255, 255);
			return true;
		}
	};

	// BGM の処理
	class BGM : public Object {
		int mode;
		bool hasPlayerWon = true;
	public:
		BGM(int mode_) {
			mode = mode_;
		}
		BGM(int mode_, bool hasPlayerWon_) {
			mode = mode_;
			hasPlayerWon = hasPlayerWon_;
		}

		bool draw() {
			// 曲名でも画面のどこかに表示する？
			return true;
		}

		void start() {
			switch (mode) {
				case 0: {
					break;
				}
				case 1: {
					PlaySoundMem(soundHandles["s_game_bgm"] , DX_PLAYTYPE_LOOP, true);
					break;
				}
				case 2: {
					if (hasPlayerWon) {

					} else {
						PlaySoundMem(soundHandles["s_game_over"] , DX_PLAYTYPE_BACK, true);
					}
					break;
				}
				default: {
					break;
				}
			}
		}

		void stop() {
			switch (mode) {
				case 0: {
					break;
				}
				case 1: {
					StopSoundMem(soundHandles["s_game_bgm"]);
					break;
				}
				case 2: {
					StopSoundMem(soundHandles["s_game_over"]);
					break;
				}
				default: {
					break;
				}
			}
		}

		// プレイヤーが死んだときの効果音
		void playDeadSound() {
			PlaySoundMem(soundHandles["s_game_dead"] , DX_PLAYTYPE_NORMAL, true);
		}
	};

	// 認識したマーカーの扱い
	class Marker : public Object {

	public:
		Marker(int x_, int y_, int width_, int height_, bool willStay_) {
			rect.x = x_, rect.y = y_, rect.width = width_, rect.height = height_;
			layer = 101;
		}

		Marker(Rectan rect_, bool willStay_) {
			rect = rect_;
			layer = 101;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_marker"], true);
			return false;
		}

		double rightHit() const { return right() - rect.width / 3; }
		double leftHit() const { return left() + rect.width / 3; }
		double topHit() const { return top() + rect.height / 3; }
		double bottomHit() const { return bottom() - rect.height / 3; }
	};

	// 敵キャラクター
	class Enemy : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;
		bool isAlive = true;

		int enemyType; // 0 : 移動しない、 1 : 左右にぴょこぴょこ
		int turnCounter = 100;
		int moveDirection;

	public:
		Enemy(int x_, int y_, int width_, int height_, int enemyType_){
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			enemyType = enemyType_;
			layer = 100;
			switch (enemyType) {
				case 1: {
					moveDirection = 1;
					break;
				}
				case 2: {
					moveDirection = rand()%4;
					break;
				}
				default: {
					moveDirection = 0;
					break;
				}
			}
		}

		bool draw() {
			switch (enemyType) {
				case 3: {
					DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_teresa"], true);
					break;
				}
				default: {
					DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_wanwan"], true);
					break;
				}
			}
			return isAlive;
		}

		void update(const std::vector<std::shared_ptr<BlockObject>> blockList) {
			// 座標更新以外にも状態の更新を扱う？

			updateCoordinate(blockList);
		}

		// 座標の更新
		void updateCoordinate(const std::vector<std::shared_ptr<BlockObject>> blockList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			double acX = 0;
			double acY = 0;

			switch (enemyType) {
				case 0:{
					acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
					acY = 4;
					break;
				}
				case 1:{
					acY = 2;
					// turnCounter : 方向転換するまでの残りカウンタ数
					// moveDirection とあわせて、方向転換の制御に用いる。
					// moveDirection は x軸方向の正とわざわざ対応付けてはいません。
					if (turnCounter <= 0 && !isJumping) {
						moveDirection *= -1;
						turnCounter = 100;
					} else {
						acX = moveDirection*0.2 * (moveDirection*diffX <= 2);
						turnCounter--;
					}
					if (!isJumping) {
						acY = -15;
					}
					isJumping = true;
					break;
				}
				case 2:{
					switch(moveDirection) {
						case 0:{
							acX = 0;
							acY = 10;
							break;
						}
						case 1:{
							acX = -10;
							acY = 0;
							break;
						}
						case 2:{
							acX = 0;
							acY = -10;
							break;
						}
						case 3:{
							acX = 10;
							acY = 0;
							break;
						}
						default:{
							break;
						}
					}
					break;
				}
				case 3:{
					acX = rand() % 5 -2;
					acY = rand() % 5 -2;
					break;
				}
				default:{
					break;
				}
			}


			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			if (enemyType == 2) {

			} else {
				// ブロックとの当たり判定
				for (auto block : blockList) {
					if (left() < block->right() && top() < block->bottom() &&
						right() > block->left() && bottom() > block->top()) {

						if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
							if (prevX >= block->rightHit()) {
								x = block->right();
							} else if (prevX + width <= block->leftHit()) {
								x = block->left() - width;
							} else {
								// TODO
							}
						} else {
							if (prevY >= block->bottomHit()) {
								y = block->bottom();
							} else if (prevY + height <= block->topHit()) {
								y = block->top() - height;
								isJumping = false;
							} else {
								// TODO
							}
						}
					}
				}
			}
		}

		// 死亡判定。マーカーで殴られたら死ぬ。

		bool deathDecision(const std::vector<std::shared_ptr<Marker>> markerList){
			// 生死判定

			std::map<std::string, int> field = {
				{"left", 0 - 200},
				{"right", WIDTH + 200},
				{"top", 0 - 200},
				{"bottom", HEIGHT + 200}
			};

			if (prevX < field["left"] || prevX > field["right"]) {
				isAlive = false;
			}
			if (prevY < field["top"] || prevY > field["bottom"]) {
				isAlive = false;
			}

			// この場合、ブロックに当たったら死ぬ、的な
			for (auto marker : markerList) {
				if (left() < marker->right() && top() < marker->bottom() &&
					right() > marker->left() && bottom() > marker->top()) {
					if (isAlive) {
						PlaySoundMem(soundHandles["s_game_attack"] , DX_PLAYTYPE_BACK, true);
					}
					isAlive = false;
					return !isAlive;
				}
			}
			return !isAlive;
		}

		bool getIsAlive() {
			return isAlive;
		}
	};

	class Player : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;
		bool isAlive = true;
		bool isToJump = false;
		int frameCount;

	public:
		Player(int x_, int y_, int width_, int height_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["s_game_player"], true);
			if (isToJump) {
				DrawExtendGraph(left(), top()-150, right(), bottom()-150, imgHandles["s_game_balloon"], true);
			}
			return true;
		}

		void update(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList) {

			updateCoordinate(key, blockList, markerList);

		}

		// プレイヤーキャラクターの座標更新
		void updateCoordinate(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList, const std::vector<std::shared_ptr<Marker>> markerList) {
			double& x = rect.x;
			double& y = rect.y;
			const int& width = rect.width;
			const int& height = rect.height;

			const double diffX = x - prevX;
			const double diffY = y - prevY;

			double acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			double acY = 4;

			if (key[KEY_INPUT_LSHIFT]) {
				isToJump = false;
				if (key[KEY_INPUT_UP] && !isJumping) {
					acY = -50;
					PlaySoundMem(soundHandles["jump"], DX_PLAYTYPE_BACK, true);
					isJumping = true;
				}				
				if (key[KEY_INPUT_RIGHT]) {
					acX = 1.5 * (diffX < 15);
				}
				if (key[KEY_INPUT_LEFT]) {
					acX = -1.5 * (diffX > -15);
				}
			}
			else if (acX == 0 && !isJumping && !isToJump) {
				if (rand() % 3 == 0) {
					frameCount = 0;
					isToJump = true;
				}
				else {
					acX = rand() % 30 - 15;
				}


			}

			if (isToJump) {
				if (frameCount >= 30) {
					acY = -50;
					PlaySoundMem(soundHandles["jump"], DX_PLAYTYPE_BACK, true);
					isJumping = true;
				}
				else {
					frameCount++;
							}
			}

			if (isJumping) {
				isToJump = false;
			}

			// verlet法
			const double tempX = x;
			x += diffX + acX;
			prevX = tempX;
			const double tempY = y;
			y += diffY + acY;
			prevY = tempY;

			// ブロックとの当たり判定
			for (auto block : blockList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = block->right();
						} else if (prevX + width <= block->leftHit()) {
							x = block->left() - width;
						} else {
							// TODO
						}
					} else {
						if (prevY >= block->bottomHit()) {
							y = block->bottom();
						} else if (prevY + height <= block->topHit()) {
							y = block->top() - height;
							isJumping = false;
						} else {
							// TODO
						}
					}
				}
			}
			// マーカーとの当たり判定
			for (auto marker : markerList) {
				if (left() < marker->right() && top() < marker->bottom() &&
					right() > marker->left() && bottom() > marker->top()) {

					if (prevY < marker->bottomHit() && prevY + height > marker->topHit()) {
						if (prevX >= marker->rightHit()) {
							x = marker->right();
						} else if (prevX + width <= marker->leftHit()) {
							x = marker->left() - width;
						} else {
							// TODO
						}
					} else {
						if (prevY >= marker->bottomHit()) {
							y = marker->bottom();
						} else if (prevY + height <= marker->topHit()) {
							y = marker->top() - height;
							isJumping = false;
						} else {
							// TODO
						}
					}
				}
			}
		}

		// プレイヤーキャラクターの生死判定更新
		bool deathDecision(std::vector<std::shared_ptr<Enemy>> enemyList) {

			std::map<std::string, int> field = {
				{"left", 0 - 200},
				{"right", WIDTH + 200},
				{"top", 0 - 200},
				{"bottom", HEIGHT + 200}
			};

			if (prevX < field["left"] || prevX > field["right"]) {
				isAlive = false;
			}
			if (prevY < field["top"] || prevY > field["bottom"]) {
				isAlive = false;
			}

			for (auto enemy : enemyList) {
				if (left() < enemy->right() && top() < enemy->bottom() &&
					right() > enemy->left() && bottom() > enemy->top()) {
					isAlive = false;
				}
			}

			return !isAlive;
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;

	std::vector<std::shared_ptr<BlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;

	// 敵作成。enemyType については Enemy クラスを参照
	void makeEnemy(int x, int y, int width, int height, int enemyType) {
		auto enemy = std::make_shared<Enemy>(x, y, width, height, enemyType);
		enemyList.push_back(enemy);
		drawList.push_back(enemy);
	}

	std::shared_ptr<BGM> bgm;
	int timer = 3500;
	bool hasPlayerWon;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));
		player = std::make_shared<Player>(WIDTH/2 - 100/2, HEIGHT/2-150/2, 100, 150);
		hasPlayerWon = true;
	}

	bool onStart() {
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
					DrawExtendGraph(WIDTH/2-429/2, 30, WIDTH/2+429/2, 30+47, imgHandles["s_game_op_title"], true);
					DrawExtendGraph(WIDTH/2-50, 400, WIDTH/2+50, 400+150, imgHandles["s_game_player"], true);
					return true;
				}
			};

			drawList.clear();
			drawList.push_back(make_shared<Title>());

			bgm = make_shared<BGM>(0);
			bgm->start();

			for(int i = 0; i < 20; i++);
		}, 150);

		// mode 1
		mode.setMode([this]() {
			drawList.clear();
			auto makeBlock = [this](int x, int y, int width, int height) {
				auto block = make_shared<BlockObject>(x, y, width, height, true);
				blockList.push_back(block);
				drawList.push_back(block);
			};
			makeBlock(0-200, 600, WIDTH+200+200, 50);
			// makeBlock(900, 600, 480, 50);
			// makeBlock(10, 100, 30, 720);
			// makeBlock(300, 300, 200, 50);

			makeEnemy(350, 200, 435/4, 349/4, 0);
			makeEnemy(900, 0, 435/2, 349/2, 1);

			drawList.push_back(player);
			drawList.push_back(make_shared<Background>(share.handle));

			bgm = make_shared<BGM>(1);
			bgm->start();

		}, 300);

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
					} else {
						DrawExtendGraph(WIDTH/2-400, 30, WIDTH/2+400, 30+296, imgHandles["s_game_result_dead"], true);
						DrawExtendGraph(WIDTH/2-75, 400, WIDTH/2+75, 400+150, imgHandles["s_game_dead"], true);
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

	bool onUpdate() {
		bool willFinishMode = false;

		switch (mode.getMode())	{
			case 0:{ // イントロダクション
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
					// share.willFinish = true;
					mode.goNext();
				}

				std::vector<std::shared_ptr<Marker>> markerList;

				// 認識したマーカーを描画
				share.rectMutex.lock();
				for (auto rect : share.rects) {
					auto marker = std::make_shared<Marker>(rect, false);
					markerList.push_back(marker);
					drawList.push_back(marker);
				}
				share.rectMutex.unlock();

				player->update(key, blockList, markerList);

				for (auto enemy : enemyList) {
					enemy->update(blockList);
					enemy->deathDecision(markerList);
				}

				if(player->deathDecision(enemyList)){
					bgm->stop();
					bgm->playDeadSound();
					// mode.goNext();
					hasPlayerWon = false;
					willFinishMode = true;
				}
				break;
			}
			case 2:{ // リザルト画面
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
			mode.goNext();
		}
		if (key[KEY_INPUT_0]) {
			makeEnemy(350, 200, 435/4, 349/4, 1);
		}
		if (key[KEY_INPUT_1]) {
			makeEnemy(900, 0, 435 / 2, 349 / 2, 1);
		}
		if (key[KEY_INPUT_2]) {
			makeEnemy(WIDTH/2, HEIGHT/2, 435/2, 349/2, 2);
		}
		if (key[KEY_INPUT_3]) {
			makeEnemy(WIDTH/2, HEIGHT/2, 120, 120, 3);
		}

		for ( auto& itr = enemyList.begin(); itr != enemyList.end();) {
			if ((*itr)->getIsAlive()) {
				++itr;
			} else {
				itr = enemyList.erase(itr);
			}
		}

		if (willFinishMode) {
			mode.goNext();
		}

		return Game::onUpdate();
	}

	bool onFinish() {
		bgm->stop();

		thread.join();
		return true;
	}
};
