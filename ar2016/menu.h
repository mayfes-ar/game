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
	class SelectGame : public Object {
		class GameDescription {
		public :
			std::string title;
			std::string description;
			int imageHandle;
			std::function<std::shared_ptr<Game>()> gameFunc;

			GameDescription(std::string title_, std::string description_, int imageHandle_,  std::function<std::shared_ptr<Game>()> gameFunc_) :
				title(title_),
				description(description_),
				imageHandle(imageHandle_),
				gameFunc(gameFunc_){}
		};
	public :
		int numOfGames;
		int selectedGameIndex;
		std::vector<GameDescription> gameList;
		SelectGame();

		std::shared_ptr<Game>& startSelectedGame() {
			return gameList[0].gameFunc();
		}

		void setNextGame(){
			selectedGameIndex++;
			if (selectedGameIndex < numOfGames) {
				// pass
			} else {
				selectedGameIndex -= numOfGames;
			}
		}

		bool draw() {
			DrawString(500, 300, gameList[selectedGameIndex].title.c_str(), GetColor(250, 250, 250));
			DrawString(500, 350, gameList[selectedGameIndex].description.c_str(), GetColor(250, 250, 250));
			DrawExtendGraph(500, 400, 600, 550, gameList[selectedGameIndex].imageHandle, true);
			return true;
		}
	};

	std::shared_ptr<SelectGame> games;

	std::shared_ptr<Game>&  gameType;

	// Menu クラスのループ処理
public:
	Menu(std::shared_ptr<Game>& gameType_) : gameType(gameType_) {
		games = std::make_shared<SelectGame>();
	}

	bool onStart() {
		using namespace std;
		fps.isShow = true;
		drawList.push_back(make_shared<Title>());
		drawList.push_back(make_shared<BackEffect>());
		drawList.push_back(games);
		return Game::onStart();
	}

	bool onUpdate() {

		if (key[KEY_INPUT_RETURN]) {
			// gameType = std::make_shared<FirstGame>();
			// gameType = games->startSelectedGame();
			gameType = games->gameList[games->selectedGameIndex].gameFunc(); // :HELP 冗長な自覚はあるけど上のコードが動かないのでこう書いています。
			share.isFinish = true;
		}
		if (key[KEY_INPUT_ESCAPE]) {
			share.isFinish = true;
		}
		if (key[KEY_INPUT_RIGHT]) {
			games->setNextGame();
		}

		return Game::onUpdate();
	}

	bool onFinish(){
		return true;
	}

};
