#pragma once

#include "game.h"
#include "first_game.h"


std::shared_ptr<Game> startMenu();

// メニュー画面
// gameType = std::make_shared<起動するGameの子クラス>();
class Menu : Game {

	// 背景処理
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

	// タイトルなど静的オブジェクトの処理
	class Title : public Object {
	public:
		Title() {
			layer = 50;
		}

		bool draw() {
			DrawString(300, 300, "TITLE", GetColor(250, 250, 250));
			return true;
		}
	};

	// ゲーム選択の仕組み
	class GameDescription {
	public :
		std::string title;
		std::string description;
		std::string imageHandleKey;
		std::function<std::shared_ptr<Game>()> gameFunc;

		GameDescription(std::string title_,
						std::string description_,
						std::string imageHandleKey_,
						std::function<std::shared_ptr<Game>()> gameFunc_) :
			title(title_),
			description(description_),
			imageHandleKey(imageHandleKey_),
			gameFunc(gameFunc_){}
	};
	class GameList {
	public :
		int numOfGames;
		std::vector<GameDescription> gameList;
		GameList();
	};

	std::shared_ptr<GameList> gameList;
	int selectedGameIndex;

	std::shared_ptr<Game>&  gameType;

	// Menu クラスのループ処理
public:
	Menu(std::shared_ptr<Game>& gameType_) : gameType(gameType_) {
		gameList = std::make_shared<GameList>();
		selectedGameIndex = 0;
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;
		drawList.push_back(make_shared<Title>());
		drawList.push_back(make_shared<BackEffect>());
		return Game::onStart();
	}

	bool onUpdate() {

		if (key[KEY_INPUT_RETURN]) {
			// gameType = std::make_shared<FirstGame>();
			gameType = gameList->gameList[selectedGameIndex].gameFunc();
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
