#pragma once

#include "object.h"
#include "fps.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>


void capture(std::mutex& mutex, int& handle, bool& isFinish);

// abstract class
class Game {
protected:
	std::mutex drawMutex;
	std::list<std::shared_ptr<Object>> drawList;

	char key[256];
	Fps fps;

	bool isFinish = false;

	virtual ~Game() {
		drawList.clear();
	}

public:
	virtual bool onStart() {
		GetHitKeyStateAll(key);
		return true;
	}

	virtual bool onUpdate() {
		fps.update();
		drawList.sort([](const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) -> bool { return a->getLayer() < b->getLayer(); });

		drawMutex.lock();
		ClearDrawScreen();

		for ( auto& itr = drawList.begin(); itr != drawList.end();) {
			if ((*itr)->draw()) {
				++itr;
			} else {
				itr = drawList.erase(itr);
			}
		}

		fps.draw();
		ScreenFlip();
		drawMutex.unlock();

		fps.wait();
		GetHitKeyStateAll(key);
		return !isFinish;
	}

	virtual bool onFinish() = 0;

};

	
class FirstGame : public Game {
	std::thread thread;
	std::shared_ptr<Player> player;

	int timer = 500;
	int handle = -1;
	
public:
	FirstGame() {
		thread = std::thread::thread(capture, std::ref(drawMutex),std::ref(handle), std::ref(isFinish));
		player = std::make_shared<Player>(200, 200, 200, 200);
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;

		drawList.push_back(player);
		drawList.push_back(make_shared<Background>(handle));

		Sleep(400);
		return Game::onStart();
	}

	bool onUpdate() {
		timer -= 1;
		if (timer <= 0) {
			isFinish = true;
		}

		player->action(key);

		return Game::onUpdate();
	}
	
	bool onFinish() {
		thread.join();
		return true;
	}
};
