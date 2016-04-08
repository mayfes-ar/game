#pragma once

#include "game.h"
#include "first_game.h"


std::shared_ptr<Game> startMenu();

// メニュー画面
// gameType = std::make_shared<起動するGameの子クラス>();
class Menu : Game {

	class BackEffect : public Object {
		int counter = 0;
		const int countMax = 2 * effectHandles["effect1"].size();

	public:
		bool draw() {
			DrawExtendGraph(0, 0, 1280, 720, effectHandles["effect1"][counter/2], true);
			counter++;
			if (counter == countMax) { counter = 0; }
			return true;
		}
	};

	class Title : public Object {
	public:
		Title() {
			layer = 50;
		}

		bool draw() {
			DrawString(300, 300, "TITLE", GetColor(255, 255, 255));
			return true;
		}
	};

	std::shared_ptr<Game>&  gameType;

public:
	Menu(std::shared_ptr<Game>& gameType_) : gameType(gameType_) {}

	bool onStart() {
		using namespace std;
		fps.isShow = true;
		drawList.push_back(make_shared<Title>());
		drawList.push_back(make_shared<BackEffect>());
		return Game::onStart();
	}

	bool onUpdate() {

		if (key[KEY_INPUT_RETURN]) {
			gameType = std::make_shared<FirstGame>();
			share.isFinish = true;
		}
		if (key[KEY_INPUT_ESCAPE]) {
			share.isFinish = true;
		}

		return Game::onUpdate();
	}

	bool onFinish(){
		return true;
	}

};