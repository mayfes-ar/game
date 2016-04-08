#pragma once

#include "game.h"

class FirstGame : public Game {

	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() {
			DrawExtendGraph(160, 0, 1120, 720, handle, FALSE);
			return true;
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

		void update(const char key[], const std::vector<std::shared_ptr<BlockObject>> blockList) {
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

			for (auto block : blockList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottomHit() && prevY + height > block->topHit()) {
						if (prevX >= block->rightHit()) {
							x = prevX = block->right();
						} else if (prevX + width <= block->leftHit()) {
							x = prevX = block->left() - width;
						} else {
							// TODO
							//Sleep(2000);
						}
					} else {
						if (prevY >= block->bottomHit()) {
							y = prevY = block->bottom();
						} else if (prevY + height <= block->topHit()) {
							y = prevY = block->top() - height;
							isJumping = false;
						} else {
							// TODO
							//Sleep(2000);
						}
					}
				}
			}
			
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;
	std::vector<std::shared_ptr<BlockObject>> blockList;

	int timer = 3500;

public:
	FirstGame() {
		thread = std::thread::thread(capture, std::ref(share));
		player = std::make_shared<Player>(200, 0, 100, 150);
	}
	
	bool onStart() {
		using namespace std;
		fps.isShow = true;
		
		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<BlockObject>(x, y, width, height, true);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		
		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(share.handle));
		makeBlock(0, 600, 1280, 300);
		makeBlock(-100, 0, 150, 720);
		makeBlock(300, 300, 200, 50);

		return Game::onStart();
	}

	bool onUpdate() {
		timer -= 1;
		if (timer <= 0) {
			share.isFinish = true;
		}

		if (key[KEY_INPUT_ESCAPE]) {
			share.isFinish = true;
		}

		std::vector<std::shared_ptr<BlockObject>> allBlockList = blockList;
		share.rectMutex.lock();
		for (auto rect : share.rects) {
			auto block = std::make_shared<BlockObject>(rect, false);
			allBlockList.push_back(block);
			drawList.push_back(block);
		}

		share.rectMutex.unlock();


		player->update(key, allBlockList);

		return Game::onUpdate();
	}

	bool onFinish() {
		thread.join();
		return true;
	}
};