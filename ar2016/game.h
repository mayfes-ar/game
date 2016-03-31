#pragma once

#include "object.h"
#include "fps.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>

void capture(std::mutex& mutex, int& handle, bool& isFinish);

class Game {
protected:
	std::mutex drawMutex;

	bool isFinish = false;

public:
	virtual bool onStart() = 0;

	virtual bool onUpdate() {
		drawMutex.lock();
		ScreenFlip();
		drawMutex.unlock();
		return !isFinish;
	}

	virtual bool onFinish() = 0;

};

	
class FirstGame : public Game {
	std::thread thread;

	int timer = 500;
	int handle = -1;
	
	char key[256];

public:
	FirstGame() {
		thread = std::thread::thread(capture, std::ref(drawMutex),std::ref(handle), std::ref(isFinish));
	}

	bool onStart() {
		return true;
	}

	bool onUpdate() {
		timer -= 1;
		if (timer <= 0) {
			isFinish = true;
		}

		drawMutex.lock();
		DrawExtendGraph(0, 0, 960, 720, handle, false);
		drawMutex.unlock();

		return Game::onUpdate();
	}
	
	bool onFinish() {
		thread.join();
		return true;
	}
};
