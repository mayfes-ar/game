#pragma once

#include "game.h"
#include "first_game.h"


std::shared_ptr<Game> startMenu();


class Menu : Game {

	class Title : public Object {
	public:
		bool draw() const {
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
		return Game::onStart();
	}

	bool onUpdate() {

		if (key[KEY_INPUT_RETURN]) {
			gameType = std::make_shared<FirstGame>();
			isFinish = true;
		}
		if (key[KEY_INPUT_ESCAPE]) {
			isFinish = true;
		}

		return Game::onUpdate();
	}

	bool onFinish(){
		return true;
	}

};