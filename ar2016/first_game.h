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

	class Block : public Object {

	public:
		Block(int x_, int y_, int width_, int height_) {
			x = x_, y = y_, width = width_, height = height_;
			layer = 20;
		}

		bool draw() {
			DrawBox(left(), top(), right(), bottom(), GetColor(27, 195, 69), false);
			return true;
		}

	};

	class Player : public Object {
		double prevX;
		double prevY;
		bool isJumping = true;

	public:
		Player(int x_, int y_, int width_, int height_) {
			x = prevX = x_;
			y = prevY = y_; 
			width = width_;
			height = height_;
			layer = 100;
		}

		bool draw() {
			DrawExtendGraph(left(), top(), right(), bottom(), imgHandles["player"], true);
			return true;
		}

		void update(const char key[], const std::vector<std::shared_ptr<Block>> blockList) {
			const double vx = x - prevX;
			const double vy = y - prevY;

			double acX = -0.5 * (1 - (vx <= 0) - (vx < 0));
			double acY = 4;

			if (key[KEY_INPUT_UP] && !isJumping) {
				acY = -50;
			}
			isJumping = true;

			if (key[KEY_INPUT_RIGHT]) {
				acX = 1.5 * (vx < 15);
			}
			if (key[KEY_INPUT_LEFT]) {
				acX = -1.5 * (vx > -15);
			}

			const double tempX = x;
			x += vx + acX;
			prevX = tempX;
			const double tempY = y;
			y += vy + acY;
			prevY = tempY;

			for (auto block : blockList) {
				if (left() < block->right() && top() < block->bottom() &&
					right() > block->left() && bottom() > block->top()) {

					if (prevY < block->bottom() && prevY + height > block->top()) {
						if (prevX >= block->right()) {
							x = prevX = block->right();
						} else if (prevX + width <= block->left()) {
							x = prevX = block->left() - width;
						} else {
							// TODO
							//Sleep(2000);
						}
					} else {
						if (prevY >= block->bottom()) {
							y = prevY = block->bottom();
						} else if (prevY + height <= block->top()) {
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
	std::vector<std::shared_ptr<Block>> blockList;

	int timer = 500;
	int handle = -1;

public:
	FirstGame() {
		thread = std::thread::thread(capture, std::ref(drawMutex), std::ref(handle), std::ref(isFinish));
		player = std::make_shared<Player>(200, 0, 100, 150);
	}
	
	bool onStart() {
		using namespace std;
		fps.isShow = true;
		
		auto makeBlock = [this](int x, int y, int width, int height) {
			auto block = make_shared<Block>(x, y, width, height);
			blockList.push_back(block);
			drawList.push_back(block);
		};
		
		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(handle));
		makeBlock(0, 600, 1280, 300);
		makeBlock(-100, 0, 150, 720);
		makeBlock(300, 300, 200, 50);

		return Game::onStart();
	}

	bool onUpdate() {
		timer -= 1;
		if (timer <= 0) {
			isFinish = true;
		}

		if (key[KEY_INPUT_ESCAPE]) {
			isFinish = true;
		}

		player->update(key, blockList);

		return Game::onUpdate();
	}

	bool onFinish() {
		thread.join();
		return true;
	}
};