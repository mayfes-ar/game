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
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["player"], true);
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
		bool isAlive = true;
	public:
		Enemy(int x_, int y_, int width_, int height_){
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["wanwan"], true);
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

			double acX = -0.5 * (1 - (diffX <= 0) - (diffX < 0));
			double acY = 4;

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
						} else {
							// TODO
						}
					}
				}
			}
		}

		// 死亡判定。マーカーで殴られたら死ぬ。
		bool deathDecision(const std::vector<std::shared_ptr<Marker>> markerList){
			// 生死判定
			// この場合、ブロックに当たったら死ぬ、的な
			for (auto marker : markerList) {
				if (left() < marker->right() && top() < marker->bottom() &&
					right() > marker->left() && bottom() > marker->top()) {
					isAlive = false;
					return true;
				}
			}
			return false;
		}
	};

	class Player : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;

	public:
		Player(int x_, int y_, int width_, int height_) {
			rect.x = prevX = x_;
			rect.y = prevY = y_;
			rect.width = width_;
			rect.height = height_;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["player"], true);
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

			if (key[KEY_INPUT_UP] && !isJumping) {
				acY = -50;
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				acX = 1.5 * (diffX < 15);
			}
			if (key[KEY_INPUT_LEFT]) {
				acX = -1.5 * (diffX > -15);
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
				return true;
			}
			if (prevY < field["top"] || prevY > field["bottom"]) {
				return true;
			}

			for (auto enemy : enemyList) {
				if (left() < enemy->right() && top() < enemy->bottom() &&
					right() > enemy->left() && bottom() > enemy->top()) {
					return true;
				}
			}
			return false;

			return false;
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;
	// std::shared_ptr<Enemy> enemy;
	std::vector<std::shared_ptr<BlockObject>> blockList;
	std::vector<std::shared_ptr<Enemy>> enemyList;

	int timer = 3500;

public:
	SinglePlayerGame() {
		thread = std::thread::thread(capture, std::ref(share));
		player = std::make_shared<Player>(200, 0, 100, 150);
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		// mode 0
		mode.setMode([this]() {
			auto makeBlock = [this](int x, int y, int width, int height) {
				auto block = make_shared<BlockObject>(x, y, width, height, true);
				blockList.push_back(block);
				drawList.push_back(block);
			};
			makeBlock(-100, 600, 700, 50);
			makeBlock(900, 600, 480, 50);
			makeBlock(10, 100, 30, 720);
			makeBlock(300, 300, 200, 50);

			auto makeEnemy = [this](int x, int y, int width, int height) {
				auto enemy = make_shared<Enemy>(x, y, width, height);
				enemyList.push_back(enemy);
				drawList.push_back(enemy);
			};
			makeEnemy(900, 0, 435/2, 349/2);

			drawList.push_back(player);
			drawList.push_back(make_shared<Background>(share.handle));
		}, -1);

		// mode 1
		mode.setMode([this]() {
			class Title : public Object {
			public:
				Title() {
					layer = 50;
				}

				bool draw() {
					DrawExtendGraph(WIDTH/2-400, 30, WIDTH/2+400, 30+296, imgHandles["s_game_result"], true);
					DrawExtendGraph(WIDTH/2-75, 400, WIDTH/2+75, 400+150, imgHandles["s_game_dead"], true);
					return true;
				}
			};

			drawList.clear();
			// drawList.push_back(make_shared<BlockObject>(300, 300, 200, 200, true));
			drawList.push_back(make_shared<Title>());
		}, -1);

		return Game::onStart();
	}

	bool onUpdate() {
		switch (mode.getMode())	{
		case 0: { // playing
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
				mode.goNext();
			}
			break;
		}
		case 1:{ // リザルト画面
			if (key[KEY_INPUT_RETURN]) {
				mode.goNext();
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

		return Game::onUpdate();
	}

	bool onFinish() {
		thread.join();
		return true;
	}
};
