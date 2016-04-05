#pragma once

#include "game.h"

class FirstGame : public Game {

	class Background : public Object {
		int& handle;

	public:
		Background(int& handle_) : handle(handle_) {
			layer = 0;
		}

		bool draw() const {
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

		bool draw() const {
			DrawBox(left(), top(), right(), bottom(), GetColor(27, 195, 69), true);
			return true;
		}

		bool isHit() {

		}
	};

	class Player : public Object {
		double prevX;
		double prevY;
		bool isJumping = false;

	public:
		Player(int x_, int y_, int width_, int height_) {
			x = prevX = x_;
			y = prevY = y_; 
			width = width_;
			height = height_;
			layer = 100;
		}

		bool draw() const {
			DrawExtendGraph(x, y, x+ width, y + height, imgHandles["mario"], true);
			return true;
		}

		void update(const char key[]) {
			double acX = -0.5 * (1 - (x - prevX <= 0) - (x - prevX < 0));
			double acY = 4;


			if (!isJumping && key[KEY_INPUT_RIGHT]) {
				acX = 1.5;
			}
			if (!isJumping && key[KEY_INPUT_LEFT]) {
				acX = -1.5;
			}

			
			if (key[KEY_INPUT_UP] && !isJumping) {
				isJumping = true;
				acY = -40;
			}

			const double tempX = x;
			x += (x - prevX) + acX;
			prevX = tempX;

			const double tempY = y;
			y += (y - prevY) + acY;
			prevY = tempY;

			if (y >= 600) {
				isJumping = false;
				y = 600;
			}
			
			
		}
	};

	std::thread thread;
	std::shared_ptr<Player> player;

	int timer = 500;
	int handle = -1;

public:
	FirstGame() {
		thread = std::thread::thread(capture, std::ref(drawMutex), std::ref(handle), std::ref(isFinish));
		player = std::make_shared<Player>(200, 200, 150, 150);
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(handle));

		Sleep(1000);
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

		player->update(key);

		return Game::onUpdate();
	}

	bool onFinish() {
		thread.join();
		return true;
	}
};